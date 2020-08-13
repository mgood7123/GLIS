//
// Created by smallville7123 on 19/07/20.
//

#include <glis/glis.hpp>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

using namespace Magnum;

typedef GL::Framebuffer SurfaceFramebuffer;
typedef ImageView2D SurfaceImageView2D;
typedef Containers::ArrayView<const void> SurfaceImageData;
typedef GL::Texture2D SurfaceTexture2D;
typedef Shaders::Flat2D SurfaceShader;

class Surface {
public:
    SurfaceFramebuffer * framebuffer_ = nullptr;
    SurfaceImageView2D * image_ = nullptr;
    SurfaceImageData * data_ = nullptr;
    SurfaceTexture2D * texture2DRead = nullptr;
    SurfaceTexture2D * texture2DDraw = nullptr;
    SurfaceShader * shaderRead = nullptr;
    SurfaceShader * shaderDraw = nullptr;
    
    void release() {
        delete framebuffer_;
        framebuffer_ = nullptr;
        delete image_;
        image_ = nullptr;
        delete data_;
        data_ = nullptr;
        delete texture2DRead;
        texture2DRead = nullptr;
        delete texture2DDraw;
        texture2DDraw = nullptr;
        delete shaderRead;
        shaderRead = nullptr;
        delete shaderDraw;
        shaderDraw = nullptr;
    }
    
    void newTexture2D(const VectorTypeFor<2, int> & size) {
        if (texture2DDraw == nullptr) texture2DDraw = new SurfaceTexture2D;
        texture2DDraw->setStorage(1, GL::TextureFormat::RGBA8, size);
    }

    void newFramebuffer(const Magnum::VectorTypeFor<2, int> & size) {
        if (framebuffer_ == nullptr) framebuffer_ = new SurfaceFramebuffer {{{}, size}};
        newTexture2D(size);
        framebuffer_->attachTexture(GL::Framebuffer::ColorAttachment{0}, *texture2DDraw, 0);
        framebuffer_->mapForDraw({{0, {GL::Framebuffer::ColorAttachment{0}}}});
    }
    
    void setTextureData(int texture_width, int texture_height, const void * data) {
        if (data_ == nullptr) data_ = new SurfaceImageData(data, texture_width*texture_height);
        if (image_ == nullptr) image_ = new ImageView2D(PixelFormat::RGBA8Unorm, {texture_width, texture_height}, *data_);
        // do mipmaps need to be regenerated if the data changes?
        newTexture2D({texture_width, texture_height});
        texture2DDraw->setSubImage(0, {}, *image_).generateMipmap();
    }
    
    void genTextureFromGLTexture(const GLuint & id) {
        if (texture2DDraw != nullptr) delete texture2DDraw;
        texture2DDraw = new GL::Texture2D;
        *texture2DDraw = GL::Texture2D::wrap(id);
    }
    
    void clear() {
        if (framebuffer_ != nullptr) {
            framebuffer_->bind();
            framebuffer_->clear(GL::FramebufferClear::Color);
        } else {
            GL::defaultFramebuffer.bind();
            GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
        }
    }
    
    void drawSquare() {
        if (texture2DRead != nullptr) {
            if (shaderRead == nullptr) shaderRead = new SurfaceShader(Shaders::Flat2D::Flag::Textured);
            
            shaderRead
                ->setColor({1.0f, 1.0f, 1.0f, 1.0f})
                .bindTexture(*texture2DRead)
                .draw(MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates)));
        } else {
            if (shaderDraw == nullptr) shaderDraw = new SurfaceShader;
            shaderDraw
                ->setColor({0.0f, 1.0f, 1.0f, 1.0f})
                .draw(MeshTools::compile(Primitives::planeSolid()));
        }
    }
    
    void drawSquare(const Surface & surface) {
        SurfaceTexture2D * tmp = texture2DRead;
        texture2DRead = surface.texture2DDraw;
        drawSquare();
        texture2DRead = tmp;
    }
};

Surface surfaceMain;
Surface surfaceTemporary;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, screen, font, fps) {
    surfaceTemporary.clear();
    surfaceTemporary.drawSquare();
    surfaceMain.clear();
    surfaceMain.drawSquare(surfaceTemporary);
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, screen, font, fps) {
    GL::defaultFramebuffer.setViewport({{}, {screen.width, screen.height}});
    surfaceTemporary.newFramebuffer({screen.width, screen.height});
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, screen, font, fps) {
    glis.destroyX11Window(screen);
    surfaceTemporary.release();
    surfaceMain.release();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 400, 400);
    glis.GLIS_setupOnScreenRendering(screen);
    screen.contextMagnum.create();
    surfaceTemporary.newFramebuffer({screen.width, screen.height});
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
