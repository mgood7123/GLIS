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
typedef Color4 SurfaceColor;

constexpr SurfaceColor surfaceTextureColor = {1.0f,  1.0f,  1.0f,  1.0f};

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
    
    void draw(SurfaceShader * shader, const SurfaceColor & color, GL::Mesh && mesh) {
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
        const SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
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
        const SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
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
    
    void drawWireframe(SurfaceShaderVisualizer * shader, const SurfaceColor & color, GL::Mesh && mesh) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shader
            ->setColor(color)
            .setWireframeColor({0.0f,1.0f,0.0f,1.0f})
            .draw(mesh);
        glDisable(GL_BLEND);
    }
    
    GL::Mesh buildPlaneWireframeMesh(
        const Vector2 & topLeft,
        const Vector2 & topRight,
        const Vector2 & bottomRight,
        const Vector2 & bottomLeft
    ) {
        const struct Vertex {
            Vector2 position;
        } vertex[] {
            {topRight},    {bottomRight},
            {bottomRight}, {bottomLeft},
            {bottomLeft},  {topLeft},
            {topLeft},     {topRight}
        };
        
        GL::Buffer vertices(vertex);
        
//         glm::translate(glm::mat4(1), glm::vec3(0.375, 0.375, 0.0f));
        
        GL::Mesh mesh;
        mesh
            .setPrimitive(MeshPrimitive::Lines)
            .addVertexBuffer(std::move(vertices), 0, GL::Attribute<0, Vector2> {})
            .setCount(sizeof(vertex)/sizeof(Vertex));
        return mesh;
    }

    void drawPlaneWireframe(
        const SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & topLeft =     {-1.0f,  1.0f},
        const Vector2 & topRight =    { 1.0f,  1.0f},
        const Vector2 & bottomRight = { 1.0f, -1.0f},
        const Vector2 & bottomLeft =  {-1.0f, -1.0f}
    ) {
        draw(
            newShaderRead(),
            color,
            buildPlaneWireframeMesh(topLeft, topRight, bottomRight, bottomLeft)
        );
    }
    
    GL::Mesh buildPlaneMesh(
        const Vector2 & topLeft,
        const Vector2 & topRight,
        const Vector2 & bottomRight,
        const Vector2 & bottomLeft
    ) {
    const struct Vertex {
        Vector2 position;
        Vector2 textureCoordinates;
    } vertex[] {
        {topRight,   {1.0f, 1.0f}},
        {bottomRight,{1.0f, 0.0f}},
        {bottomLeft, {0.0f, 0.0f}},
        {topLeft,    {0.0f, 1.0f}}
    };
    
    UnsignedInt indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    GL::Buffer vertexBuffer(vertex);
    GL::Buffer indexBuffer(indices);

    GL::Mesh mesh;
    mesh
        .setPrimitive(MeshPrimitive::TriangleStrip)
        .addVertexBuffer(
            std::move(vertexBuffer), 0,
            GL::Attribute<0, Vector2> {}, GL::Attribute<1, Vector2> {}
        )
        .setCount(sizeof(vertex)/sizeof(Vertex))
        .setIndexBuffer(
            std::move(indexBuffer), 0, GL::MeshIndexType::UnsignedInt
            )
        .setCount(sizeof(indices)/sizeof(UnsignedInt))
        ;
        return mesh;
    }
    
    void drawPlane(
        const SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & topLeft =     {-1.0f,  1.0f},
        const Vector2 & topRight =    { 1.0f,  1.0f},
        const Vector2 & bottomRight = { 1.0f, -1.0f},
        const Vector2 & bottomLeft =  {-1.0f, -1.0f}
    ) {
        if (texture2DRead != nullptr) {
            draw(
                newShaderReadTexture(),
                surfaceTextureColor,
                buildPlaneMesh(topLeft, topRight, bottomRight, bottomLeft)
            );
        } else {
            draw(
                newShaderRead(),
                color,
                buildPlaneMesh(topLeft, topRight, bottomRight, bottomLeft)
            );
        }
    }
    
    void drawPlane(
        const Surface & surface,
        const SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & topLeft =     {-1.0f,  1.0f},
        const Vector2 & topRight =    { 1.0f,  1.0f},
        const Vector2 & bottomRight = { 1.0f, -1.0f},
        const Vector2 & bottomLeft =  {-1.0f, -1.0f}
    ) {
        SurfaceTexture2D * tmp = texture2DRead;
        texture2DRead = surface.texture2DDraw;
        drawPlane(color, topLeft, topRight, bottomRight, bottomLeft);
        texture2DRead = tmp;
    }
};

Surface surfaceMain;
Surface surfaceTemporary;
Surface surfaceTemporary2;
Surface surfaceTemporary3;
Surface surfaceTemporary4;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, screen, font, fps) {
//     surfaceTemporary3.clear();
//     surfaceTemporary3.drawTriangle();
//     surfaceTemporary3.drawPlaneWireframe({1.0f, 0.0f,  0.0f,  0.0f});
//     
//     surfaceTemporary2.clear();
//     surfaceTemporary2.drawPlane(surfaceTemporary3);
//     surfaceTemporary2.drawPlaneWireframe({1.0f, 0.0f,  0.0f,  0.0f});
//     
//     surfaceTemporary.clear();
//     surfaceTemporary.drawTriangle(surfaceTemporary2);
//     surfaceTemporary.drawPlaneWireframe({1.0f, 0.0f,  0.0f,  0.0f});
//     
//     surfaceTemporary.clear();
//     surfaceTemporary.drawTriangle();
//     surfaceTemporary.drawPlaneWireframe({1.0f, 0.0f,  0.0f,  0.0f});
    surfaceMain.clear();
    surfaceMain.drawTriangle();
    surfaceMain.drawPlaneWireframe({1.0f, 0.0f,  0.0f,  0.4f});
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, screen, font, fps) {
    surfaceTemporary4.resize({screen.width, screen.height});
    surfaceTemporary3.resize({screen.width, screen.height});
    surfaceTemporary2.resize({screen.width, screen.height});
    surfaceTemporary.resize({screen.width, screen.height});
    surfaceMain.resize({screen.width, screen.height});
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, screen, font, fps) {
    glis.destroyX11Window(screen);
    surfaceTemporary4.release();
    surfaceTemporary3.release();
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
    surfaceTemporary3.newFramebuffer({screen.width, screen.height});
    surfaceTemporary4.newFramebuffer({screen.width, screen.height});
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
