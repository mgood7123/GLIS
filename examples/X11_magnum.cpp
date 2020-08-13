//
// Created by smallville7123 on 19/07/20.
//

#include <glis/glis.hpp>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

using namespace Magnum;

typedef GL::Framebuffer SurfaceFramebuffer;
typedef ImageView2D SurfaceImageView2D;
typedef Containers::ArrayView<const void> SurfaceImageData;
typedef GL::Texture2D SurfaceTexture2D;

class Surface {
public:
    SurfaceFramebuffer * framebuffer_ = nullptr;
    SurfaceImageView2D * image_ = nullptr;
    SurfaceImageData * data_ = nullptr;
    SurfaceTexture2D * texture2D_ = nullptr;
    
    void newTexture2D(const VectorTypeFor<2, int> & size) {
        if (texture2D_ != nullptr) delete texture2D_;
        texture2D_ = new SurfaceTexture2D;
        texture2D_->setStorage(1, GL::TextureFormat::RGBA8, size);
    }

    void newFramebuffer(const Magnum::VectorTypeFor<2, int> & size) {
        if (framebuffer_ != nullptr) delete framebuffer_;
        framebuffer_ = new SurfaceFramebuffer {{{}, size}};
        if (texture2D_ == nullptr) newTexture2D(size);
        framebuffer_->attachTexture(GL::Framebuffer::ColorAttachment{0}, *texture2D_, 0);
    }
    
    void setTextureData(int texture2D_width, int texture2D_height, const void * data) {
        if (image_ != nullptr) delete image_;
        if (data_ != nullptr) delete data_;
        data_ = new SurfaceImageData(data, texture2D_width*texture2D_height);
        image_ = new ImageView2D(PixelFormat::RGBA8Unorm, {texture2D_width, texture2D_height}, *data_);
        // do mipmaps need to be regenerated if the data changes?
        if (texture2D_ == nullptr) newTexture2D({texture2D_width, texture2D_height});
        texture2D_->setSubImage(0, {}, *image_).generateMipmap();
    }
    
    void genTextureFromGLTexture(const GLuint & id) {
        if (texture2D_ != nullptr) delete texture2D_;
        texture2D_ = new GL::Texture2D;
        *texture2D_ = GL::Texture2D::wrap(id);
    }
    
    void clear() {
        if (framebuffer_ != nullptr) {
            framebuffer_->clear(GL::FramebufferClear::Color);
        } else {
            GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
        }
    }
    
    void drawSquare() {
        if (texture2D_ != nullptr) {
            Shaders::Flat2D shader (Shaders::Flat2D::Flag::Textured);
            shader
                .bindTexture(*texture2D_)
                .setColor({1.0f, 0.0f, 0.0f, 1.0f})
                .draw(MeshTools::compile(Primitives::planeSolid()));
        } else {
            Shaders::Flat2D shader;
            shader
                .setColor({1.0f, 0.0f, 0.0f, 1.0f})
                .draw(MeshTools::compile(Primitives::planeSolid()));
        }
    }
    
    void drawSquare(const Surface & surface) {
        Shaders::Flat2D shader (Shaders::Flat2D::Flag::Textured);
        if (surface.texture2D_ != nullptr) shader.bindTexture(*surface.texture2D_);
        shader.setColor({1.0f, 1.0f, 1.0f, 1.0f});
        shader.draw(MeshTools::compile(Primitives::planeSolid()));
    }
};

Surface surfaceMain;
Surface surfaceTemporary;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
//     surfaceTemporary.newFramebuffer({400, 400});
//     surfaceTemporary.clear();
//     surfaceTemporary.drawSquare();
    surfaceMain.clear();
    surfaceMain.drawSquare(/*surfaceTemporary*/);
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glis.GLIS_Viewport(renderer);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(screen);
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 400, 400);
    glis.GLIS_setupOnScreenRendering(screen);
    screen.contextMagnum.create();
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
