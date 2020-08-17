#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>

using namespace Magnum;

typedef GL::Framebuffer GLIS_SurfaceFramebuffer;
typedef ImageView2D GLIS_SurfaceImageView2D;
typedef Containers::ArrayView<const void> GLIS_SurfaceImageData;
typedef GL::Texture2D GLIS_SurfaceTexture2D;
typedef Shaders::Flat2D GLIS_SurfaceShader;
typedef Color4 GLIS_SurfaceColor;
constexpr GLIS_SurfaceColor surfaceTextureColor = {1.0f,  1.0f,  1.0f,  1.0f};

// https://www.geeksforgeeks.org/window-to-viewport-transformation-in-computer-graphics-with-implementation/

class GLIS_Surface {
private:
    GLIS_SurfaceFramebuffer * framebuffer_ = nullptr;
    GLIS_SurfaceImageView2D * image_ = nullptr;
    GLIS_SurfaceImageData * data_ = nullptr;
    GLIS_SurfaceTexture2D * texture2DRead = nullptr;
    GLIS_SurfaceTexture2D * texture2DDraw = nullptr;
    GLIS_SurfaceShader * shaderRead = nullptr;
    GLIS_SurfaceShader * shaderReadTexture = nullptr;
    GLIS_SurfaceShader * shaderDraw = nullptr;
    
public:
    int width = 0;
    int height = 0;
    static constexpr float globalScale = 1.0f;
    
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
    }
    
    void newTexture2D(const VectorTypeFor<2, int> & size) {
        delete texture2DDraw;
        texture2DDraw = new GLIS_SurfaceTexture2D;
        texture2DDraw->setStorage(1, GL::TextureFormat::RGBA8, size);
    }

    void newFramebuffer(const Magnum::VectorTypeFor<2, int> & size) {
        width = size[0];
        height = size[1];
        if (framebuffer_ == nullptr) framebuffer_ = new GLIS_SurfaceFramebuffer {{{}, size}};
        framebuffer_->setViewport({{}, size});
        newTexture2D(size);
        framebuffer_->attachTexture(GL::Framebuffer::ColorAttachment{0}, *texture2DDraw, 0);
        framebuffer_->mapForDraw({{0, {GL::Framebuffer::ColorAttachment{0}}}});
    }
    
    void resize(const Magnum::VectorTypeFor<2, int> & size) {
        if (framebuffer_ != nullptr) {
            newFramebuffer(size);
        } else {
            width = size[0];
            height = size[1];
            GL::defaultFramebuffer.setViewport({{}, size});
        }
    }
    
    void setTextureData(int texture_width, int texture_height, const void * data) {
        if (data_ == nullptr) data_ = new GLIS_SurfaceImageData(data, texture_width*texture_height);
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
    
    Range2Di getViewport() {
        return framebuffer_ != nullptr ? framebuffer_->viewport() : GL::defaultFramebuffer.viewport();
    }
    
    GLIS_SurfaceShader * newShaderReadTexture() {
        if (shaderReadTexture == nullptr) shaderReadTexture = new GLIS_SurfaceShader(Shaders::Flat2D::Flag::Textured);
        return shaderReadTexture;
    }

    GLIS_SurfaceShader * newShaderRead() {
        if (shaderRead == nullptr) shaderRead = new GLIS_SurfaceShader;
        return shaderRead;
    }
    
    void draw(GLIS_SurfaceShader * shader, const GLIS_SurfaceColor & color, GL::Mesh && mesh) {
        if (texture2DRead != nullptr) shader->bindTexture(*texture2DRead);
        shader
            ->setColor(color)
//             glOrtho(-0.5, (surfaceMain.width - 1) + 0.5, -0.5, (surfaceMain.height - 1) + 0.5);
            .draw(mesh);
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
            {Left , {0.0f, 0.0f}}, /* Left position and texture coordinate for mapping textures */
            {Right, {1.0f, 0.0f}}, /* Right position and texture coordinate for mapping textures */
            {Top  , {0.5f,  1.0f}}  /* Top position and texture coordinate for mapping textures */
        };
        
        GL::Buffer buffer(data);
        
        GL::Mesh mesh;
        mesh
            .setCount(3)
            .addVertexBuffer(
                std::move(buffer), 0,
                GL::Attribute<0, Vector2> {},
                GL::Attribute<1, Vector2> {}
        );
        return mesh;
    }
        
    void drawTriangle(
        const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & Left  = {-globalScale, -globalScale},
        const Vector2 & Right = { globalScale, -globalScale},
        const Vector2 & Top   = { 0.0f,  globalScale}
    ) {
        if (texture2DRead != nullptr) {
            draw(newShaderReadTexture(), surfaceTextureColor, buildTriangleMesh(Left, Right, Top));
        } else {
            draw(newShaderRead(), color, buildTriangleMesh(Left, Right, Top));
        }
    }
    
    void drawTriangle(
        const GLIS_Surface & surface,
        const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & Left  = {-globalScale, -globalScale},
        const Vector2 & Right = { globalScale, -globalScale},
        const Vector2 & Top   = { 0.0f,  globalScale}
    ) {
        GLIS_SurfaceTexture2D * tmp = texture2DRead;
        texture2DRead = surface.texture2DDraw;
        drawTriangle(color, Left, Right, Top);
        texture2DRead = tmp;
    }
    
    GL::Mesh buildPlaneWireframeMesh(
        const Vector2 & topLeft,
        const Vector2 & topRight,
        const Vector2 & bottomRight,
        const Vector2 & bottomLeft
    ) {
        // offset left and top lines by one pixel
        
        // offset x axis of bottom left vector
        const Vector2 & bl = Vector2((float)(((bottomLeft[0] * (width/2))+1) / (width/2)), bottomLeft[1]);
        // offset y axis of top left vector
        const Vector2 & tl = Vector2(topLeft[0], (float)(((topLeft[1] * (height/2))-1) / (height/2)));
        // offset y axis of top right vector
        const Vector2 & tr = Vector2(topRight[0], (float)(((topRight[1] * (height/2))-1) / (height/2)));
        
        const struct Vertex {
            Vector2 position;
        } vertex[] {
            {topRight},    {bottomRight},
            {bottomRight}, {bottomLeft},
            {bottomLeft},  {topLeft},
            {topLeft},  {topRight}
//             {bl},  {tl},
//             {tl},  {tr}
        };
        
        GL::Buffer vertices(vertex);
        
        GL::Mesh mesh;
        mesh
            .setPrimitive(MeshPrimitive::Lines)
            .addVertexBuffer(std::move(vertices), 0, GL::Attribute<0, Vector2> {})
            .setCount(sizeof(vertex)/sizeof(Vertex));
        return mesh;
    }

    void drawPlaneWireframe(
        const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & topLeft =     {-globalScale,  globalScale},
        const Vector2 & topRight =    { globalScale,  globalScale},
        const Vector2 & bottomRight = { globalScale, -globalScale},
        const Vector2 & bottomLeft =  {-globalScale, -globalScale}
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
        const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & topLeft =     {-globalScale,  globalScale},
        const Vector2 & topRight =    { globalScale,  globalScale},
        const Vector2 & bottomRight = { globalScale, -globalScale},
        const Vector2 & bottomLeft =  {-globalScale, -globalScale}
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
        const GLIS_Surface & surface,
        const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & topLeft =     {-globalScale,  globalScale},
        const Vector2 & topRight =    { globalScale,  globalScale},
        const Vector2 & bottomRight = { globalScale, -globalScale},
        const Vector2 & bottomLeft =  {-globalScale, -globalScale}
    ) {
        GLIS_SurfaceTexture2D * tmp = texture2DRead;
        texture2DRead = surface.texture2DDraw;
        drawPlane(color, topLeft, topRight, bottomRight, bottomLeft);
        texture2DRead = tmp;
    }
};
