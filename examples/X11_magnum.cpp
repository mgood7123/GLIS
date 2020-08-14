//
// Created by smallville7123 on 19/07/20.
//

#include <glis/glis.hpp>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/ImageView.h>
#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Shaders/MeshVisualizer.h>

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
typedef Shaders::MeshVisualizer2D SurfaceShaderVisualizer;

constexpr Color4 surfaceTextureColor = {1.0f,  1.0f,  1.0f,  1.0f};

class Surface {
private:
    SurfaceFramebuffer * framebuffer_ = nullptr;
    SurfaceImageView2D * image_ = nullptr;
    SurfaceImageData * data_ = nullptr;
    SurfaceTexture2D * texture2DRead = nullptr;
    SurfaceTexture2D * texture2DDraw = nullptr;
    SurfaceShader * shaderRead = nullptr;
    SurfaceShader * shaderReadTexture = nullptr;
    SurfaceShader * shaderDraw = nullptr;
    SurfaceShaderVisualizer * shaderVisualizerRead = nullptr;
    SurfaceShaderVisualizer * shaderVisualizerReadTexture = nullptr;
    SurfaceShaderVisualizer * shaderVisualizerDraw = nullptr;

public:
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
        delete shaderReadTexture;
        shaderReadTexture = nullptr;
        delete shaderDraw;
        shaderDraw = nullptr;
        delete shaderVisualizerRead;
        shaderVisualizerRead = nullptr;
        delete shaderVisualizerReadTexture;
        shaderVisualizerReadTexture = nullptr;
        delete shaderVisualizerDraw;
        shaderVisualizerDraw = nullptr;
    }
    
    void newTexture2D(const VectorTypeFor<2, int> & size) {
        delete texture2DDraw;
        texture2DDraw = new SurfaceTexture2D;
        texture2DDraw->setStorage(1, GL::TextureFormat::RGBA8, size);
    }

    void newFramebuffer(const Magnum::VectorTypeFor<2, int> & size) {
        // setting the viewport size on a GL::Framebuffer seems to cause it to display incorrectly
        // even if the framebuffer is re-created
        if (framebuffer_ == nullptr) framebuffer_ = new SurfaceFramebuffer {{{}, size}};
        framebuffer_->setViewport({{}, size});
        newTexture2D(size);
        framebuffer_->attachTexture(GL::Framebuffer::ColorAttachment{0}, *texture2DDraw, 0);
        framebuffer_->mapForDraw({{0, {GL::Framebuffer::ColorAttachment{0}}}});
    }
    
    void resize(const Magnum::VectorTypeFor<2, int> & size) {
        if (framebuffer_ != nullptr) {
            newFramebuffer(size);
        } else {
            GL::defaultFramebuffer.setViewport({{}, size});
        }
    }
    
    void setTextureData(int texture_width, int texture_height, const void * data) {
        if (data_ == nullptr) data_ = new SurfaceImageData(data, texture_width*texture_height);
        if (image_ == nullptr) image_ = new ImageView2D(PixelFormat::RGBA8Unorm, {texture_width, texture_height}, *data_);
        newTexture2D({texture_width, texture_height});
        texture2DDraw
            ->setSubImage(0, {}, *image_)
            .generateMipmap();
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
    
    SurfaceShader * newShaderReadTexture() {
        if (shaderReadTexture == nullptr) shaderReadTexture = new SurfaceShader(Shaders::Flat2D::Flag::Textured);
        return shaderReadTexture;
    }

    SurfaceShader * newShaderRead() {
        if (shaderRead == nullptr) shaderRead = new SurfaceShader;
        return shaderRead;
    }
    
    void draw(SurfaceShader * shader, const Color4 & color, GL::Mesh && mesh) {
        if (texture2DRead != nullptr) shader->bindTexture(*texture2DRead);
        shader->setColor(color).draw(mesh);
    }
    
    GL::Mesh buildTriangleMesh(
        const Vector2 & Left,
        const Vector2 & Right,
        const Vector2 & Top
    ) {
        struct TriangleVertex {
            Vector2 position;
            Vector2 textureCoordinates;
        };
        
        const TriangleVertex data[]{
            {Left , {0.0f, 0.0f}}, /* Left position and texture coordinate */
            {Right, {1.0f, 0.0f}}, /* Right position and texture coordinate */
            {Top  , {0.5f,  1.0f}}  /* Top position and texture coordinate */
        };
        
        GL::Buffer buffer;
        buffer.setData(data);
        
        GL::Mesh mesh;
        mesh.setCount(3).addVertexBuffer(
            std::move(buffer), 0,
            GL::Attribute<0, Vector2> {}, GL::Attribute<1, Vector2> {}
        );
        return mesh;
    }
    
    void drawTriangle(
        const Color4 & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & Left  = {-0.5f, -0.5f},
        const Vector2 & Right = { 0.5f, -0.5f},
        const Vector2 & Top   = { 0.0f,  0.5f}
    ) {
        if (texture2DRead != nullptr) {
            draw(newShaderReadTexture(), surfaceTextureColor, buildTriangleMesh(Left, Right, Top));
        } else {
            draw(newShaderRead(), color, buildTriangleMesh(Left, Right, Top));
        }
    }
    
    void drawTriangle(
        const Surface & surface,
        const Color4 & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & Left  = {-0.5f, -0.5f},
        const Vector2 & Right = { 0.5f, -0.5f},
        const Vector2 & Top   = { 0.0f,  0.5f}
    ) {
        SurfaceTexture2D * tmp = texture2DRead;
        texture2DRead = surface.texture2DDraw;
        drawTriangle(color, Left, Right, Top);
        texture2DRead = tmp;
    }
    
    SurfaceShaderVisualizer * newShaderVisualizerRead() {
        if (shaderVisualizerRead == nullptr) shaderVisualizerRead = new SurfaceShaderVisualizer {Shaders::MeshVisualizer2D::Flag::Wireframe|Shaders::MeshVisualizer2D::Flag::NoGeometryShader};
        return shaderVisualizerRead;
    }
    
    SurfaceShaderVisualizer * newShaderVisualizerReadTexture() {
        if (shaderVisualizerReadTexture == nullptr) shaderVisualizerReadTexture = new SurfaceShaderVisualizer {Shaders::MeshVisualizer2D::Flag::Wireframe|Shaders::MeshVisualizer2D::Flag::NoGeometryShader};
        return shaderVisualizerReadTexture;
    }
    
    void drawVisualized(SurfaceShaderVisualizer * shader, const Color4 & color, GL::Mesh && mesh) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shader
            ->setColor(color)
            .setWireframeColor({0.0f,1.0f,0.0f,1.0f})
            .draw(mesh);
        glDisable(GL_BLEND);
    }
    
    GL::Mesh buildPlaneVisualMesh(
        const Vector2 & Left1,
        const Vector2 & Right1,
        const Vector2 & Top1,
        const Vector2 & Left2,
        const Vector2 & Right2,
        const Vector2 & Top2
    ) {
        struct TriangleVertex {
            Vector2 position;
        };

        const TriangleVertex data[]{
            {Left1}, /* Left position and no texture coordinate */
            {Right1}, /* Right position and no texture coordinate */
            {Top1}, /* Top position and no texture coordinate */
            {Left2}, /* Left position and no texture coordinate */
            {Right2}, /* Right position and no texture coordinate */
            {Top2}  /* Top position and no texture coordinate */
        };

        GL::Buffer buffer;
        buffer.setData(data);

        GL::Mesh mesh;
        mesh.setCount(6);
        mesh.addVertexBuffer(std::move(buffer), 0, Shaders::MeshVisualizer2D::Position{});
        return mesh;
    }

    void drawPlaneVisualized(
        const Color4 & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & Left1  = {-1.0f,  1.0f},
        const Vector2 & Right1 = {-1.0f, -1.0f},
        const Vector2 & Top1   = { 1.0f, -1.0f},
        const Vector2 & Left2  = { 1.0f,  1.0f},
        const Vector2 & Right2 = { 1.0f, -1.0f},
        const Vector2 & Top2   = {-1.0f,  1.0f}
    ) {
        if (texture2DRead != nullptr) {
            drawVisualized(
                newShaderVisualizerReadTexture(),
                surfaceTextureColor,
                buildPlaneVisualMesh(Left1, Right1, Top1, Left2, Right2, Top2)
            );
        } else {
            drawVisualized(
                newShaderVisualizerRead(),
                color,
                buildPlaneVisualMesh(Left1, Right1, Top1, Left2, Right2, Top2)
            );
        }
    }
    
    void drawPlaneVisualized(
        const Surface & surface,
        const Color4 & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & Left1  = {-1.0f,  1.0f},
        const Vector2 & Right1 = {-1.0f, -1.0f},
        const Vector2 & Top1   = { 1.0f, -1.0f},
        const Vector2 & Left2  = { 1.0f,  1.0f},
        const Vector2 & Right2 = { 1.0f, -1.0f},
        const Vector2 & Top2   = {-1.0f,  1.0f}
    ) {
        SurfaceTexture2D * tmp = texture2DRead;
        texture2DRead = surface.texture2DDraw;
        drawPlaneVisualized(color, Left1, Right1, Top1, Left2, Right2, Top2);
        texture2DRead = tmp;
    }
    
    GL::Mesh buildPlaneMesh(
        const Vector2 & Left1,
        const Vector2 & Right1,
        const Vector2 & Top1,
        const Vector2 & Left2,
        const Vector2 & Right2,
        const Vector2 & Top2
    ) {
        struct TriangleVertex {
            Vector2 position;
            Vector2 textureCoordinates;
        };
        
        const TriangleVertex data[]{
            {Left1 , {0.0f, 0.0f}}, /* Left position and texture coordinate */
            {Right1, {1.0f, 0.0f}}, /* Right position and texture coordinate */
            {Top1  , {0.5f,  1.0f}}, /* Top position and texture coordinate */
            {Left2 , {0.0f, 0.0f}}, /* Left position and texture coordinate */
            {Right2, {1.0f, 0.0f}}, /* Right position and texture coordinate */
            {Top2  , {0.5f,  1.0f}}  /* Top position and texture coordinate */
        };
        
        GL::Buffer buffer;
        buffer.setData(data);
        
        GL::Mesh mesh;
        mesh
            .setCount(6)
            .addVertexBuffer(
                std::move(buffer), 0,
                GL::Attribute<0, Vector2> {}, GL::Attribute<1, Vector2> {}
            );
        return mesh;
    }
    
    void drawPlane(
        const Color4 & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & Left1  = {-1.0f,  1.0f},
        const Vector2 & Right1 = {-1.0f, -1.0f},
        const Vector2 & Top1   = { 1.0f, -1.0f},
        const Vector2 & Left2  = { 1.0f,  1.0f},
        const Vector2 & Right2 = { 1.0f, -1.0f},
        const Vector2 & Top2   = {-1.0f,  1.0f}
    ) {
        if (texture2DRead != nullptr) {
            draw(
                newShaderReadTexture(),
                surfaceTextureColor,
                buildPlaneMesh(Left1, Right1, Top1, Left2, Right2, Top2)
            );
        } else {
            draw(
                newShaderRead(),
                color,
                buildPlaneMesh(Left1, Right1, Top1, Left2, Right2, Top2)
            );
        }
    }
    
    void drawPlane(
        const Surface & surface,
        const Color4 & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & Left1  = {-1.0f,  1.0f},
        const Vector2 & Right1 = {-1.0f, -1.0f},
        const Vector2 & Top1   = { 1.0f, -1.0f},
        const Vector2 & Left2  = { 1.0f,  1.0f},
        const Vector2 & Right2 = { 1.0f, -1.0f},
        const Vector2 & Top2   = {-1.0f,  1.0f}
    ) {
        SurfaceTexture2D * tmp = texture2DRead;
        texture2DRead = surface.texture2DDraw;
        drawPlane(color, Left1, Right1, Top1, Left2, Right2, Top2);
        texture2DRead = tmp;
    }

    void drawSquare() {
        if (texture2DRead != nullptr) {
            if (shaderRead == nullptr) shaderRead = new SurfaceShader(Shaders::Flat2D::Flag::Textured);
            
            shaderRead
                ->setColor({1.0f,  1.0f,  1.0f,  1.0f})
                .bindTexture(*texture2DRead)
                .draw(MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates)));
        } else {
            if (shaderDraw == nullptr) shaderDraw = new SurfaceShader;
            shaderDraw
                ->setColor({0.0f,  1.0f,  1.0f,  1.0f})
                .draw(MeshTools::compile(Primitives::planeSolid(Primitives::PlaneFlag::TextureCoordinates)));
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
Surface surfaceTemporary2;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, screen, font, fps) {
    surfaceTemporary2.clear();
    surfaceTemporary2.drawTriangle();
//     surfaceTemporary.clear();
//     surfaceTemporary.drawPlane();
    surfaceMain.clear();
    surfaceMain.drawPlane();
    surfaceMain.drawPlaneVisualized({1.0f, 0.0f,  0.0f,  0.0f});
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, screen, font, fps) {
    surfaceTemporary2.resize({screen.width, screen.height});
    surfaceTemporary.resize({screen.width, screen.height});
    surfaceMain.resize({screen.width, screen.height});
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, screen, font, fps) {
    glis.destroyX11Window(screen);
    surfaceTemporary2.release();
    surfaceTemporary.release();
    surfaceMain.release();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 400, 400);
    glis.GLIS_setupOnScreenRendering(screen);
    screen.contextMagnum.create();
    surfaceTemporary.newFramebuffer({screen.width, screen.height});
    surfaceTemporary2.newFramebuffer({screen.width, screen.height});
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
