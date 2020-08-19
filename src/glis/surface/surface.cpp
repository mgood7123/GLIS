#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Math/Matrix3.h>

using namespace Magnum;

typedef GL::Framebuffer GLIS_SurfaceFramebuffer;
typedef ImageView2D GLIS_SurfaceImageView2D;
typedef Containers::ArrayView<const void> GLIS_SurfaceImageData;
typedef GL::Texture2D GLIS_SurfaceTexture2D;
typedef Shaders::Flat2D GLIS_SurfaceShader;
typedef Color4 GLIS_SurfaceColor;
constexpr GLIS_SurfaceColor surfaceTextureColor = {1.0f,  1.0f,  1.0f,  1.0f};

// https://gamedev.stackexchange.com/a/110358 software rendering and opengl rendering details



// https://www.geeksforgeeks.org/window-to-viewport-transformation-in-computer-graphics-with-implementation/
// https://www.khronos.org/opengl/wiki/Compute_eye_space_from_window_space
// https://www.geeksforgeeks.org/window-to-viewport-transformation-in-computer-graphics-with-implementation/
// https://www.javatpoint.com/computer-graphics-window-to-viewport-co-ordinate-transformation

// [02:27] <derhass> TacoCodedSalad: you could start by understanding the current situation.
// draw it on paper.
// i.e.
// just assume your viewport would be 10 by 6 pixels
// and that understand what it means to draw lines
// resulting in a rectangle (-1,-1) to (1,1) in NDC coords.
// you should immediately see the isse
// as soon as you understood the window space coordinates of these points

// [23:09] <derhass> TacoCodedSalad_: you need to take a sheet of paper, assume the square cells are pixels, just make up some window of some arbitrary number of pixels in width and height (ideally so that it fits on the sheet)
// [23:09] <derhass> TacoCodedSalad_: then, calculate where (-1,1) and (1,1) end up in your window space
// [23:10] <derhass> TacoCodedSalad_: mark thise points on your paper, take a ruler and connect them with a straight line segment
// [23:10] <derhass> TacoCodedSalad_: and then, figure out which pixels it is supposed to affect

// http://print-graph-paper.com/virtual-graph-paper

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
    
    void resize(const Magnum::VectorTypeFor<2, int> & size1, const Magnum::VectorTypeFor<2, int> & size2) {
        width = size2[0];
        height = size2[1];
        GL::defaultFramebuffer.setViewport({size1, size2});
    }

    template <typename T> void setTextureData(const T * data, int texture_width, int texture_height) {
        
        Containers::ArrayView<const T> data_ (data, texture_width*texture_height);
        ImageView2D image_ (PixelFormat::RGBA8Unorm, {texture_width, texture_height}, std::move(data_));
        
        newTexture2D({texture_width, texture_height});
        
        texture2DDraw
            ->setSubImage(0, {}, std::move(image_))
            .generateMipmap();
    }
    
    void genTextureFromGLTexture(const GLuint & id) {
        if (texture2DDraw != nullptr) delete texture2DDraw;
        texture2DDraw = new GL::Texture2D;
        *texture2DDraw = GL::Texture2D::wrap(id);
    }
    
    void bind() {
        framebuffer_ != nullptr ? framebuffer_->bind() : GL::defaultFramebuffer.bind();
    }
    
    void clear() {
        if (framebuffer_ != nullptr) framebuffer_->clear(GL::FramebufferClear::Color);
        else GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
    }
    
    Range2Di getViewport() {
        return framebuffer_ != nullptr ? framebuffer_->viewport() : GL::defaultFramebuffer.viewport();
    }
    
    GLIS_SurfaceShader * newShaderReadTexture() {
        if (shaderReadTexture == nullptr) shaderReadTexture = new GLIS_SurfaceShader(Shaders::Flat2D::Flag::Textured);
        return shaderReadTexture;
    }

    GLIS_SurfaceShader * newShaderRead() {
        if (shaderRead == nullptr) {
            shaderRead = new GLIS_SurfaceShader;
            shaderRead->setTransformationProjectionMatrix(Matrix3::projection({2.005f, 2.005f}));
//             shaderRead->setTransformationProjectionMatrix(Matrix3::projection({2.0f, 2.0f}));
        }
        return shaderRead;
    }
    
    void draw(GLIS_SurfaceShader * shader, const GLIS_SurfaceColor & color, GL::Mesh && mesh) {
        if (texture2DRead != nullptr) shader->bindTexture(*texture2DRead);
        shader
            ->setColor(color)
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
            {topRight},    {bottomRight}, // right
            {bottomRight}, {bottomLeft},  // bottom
            {bottomLeft},  {topLeft},     // left
            {topLeft},     {topRight}     // top
//             {bl},  {tl},                  // left
//             {tl},  {tr}                   // top
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
    
    
    
    // texture manipulation
    
    Vector2i NDC_to_WindowSpace(const Vector2 & xy) {
        // https://www.khronos.org/opengl/wiki/Vertex_Post-Processing#Viewport_transform
        // https://wikimedia.org/api/rest_v1/media/math/render/svg/138401cb60a7c667336fb41d47d22f37c8f2a569

        // NDC to Window Space: x: (width/2)*X_NDC + X + (width/2)
        // NDC to Window Space: y: (height/2)*Y_NDC + Y + (height/2)

        // NDC {X: -1, Y: 1}, {X: 1, Y: 1}

        // width: 400
        // height: 400

        // window space: x: ((400/2) * -1) + 0 + (400/2)
        // window space: x: (200 * -1) + 0 + 200
        // window space: x: -200 + 200
        // window space: x: 0

        // window space: y: ((400/2) * 1) + 0 + (400/2)
        // window space: y: (200 * 1) + 0 + 200
        // window space: y: 200 + 200
        // window space: y: 400

        // window space: {X: 0, Y: 400}, {X: 400, Y: 400}
        
        // https://magcius.github.io/xplain/article/rast1.html
            
        // determine what side of a pixel the given NDC coordinate will draw to
        // and fix it if it is off screen
        
        // https://www.khronos.org/registry/OpenGL/specs/gl/glspec46.core.pdf
        // 14.5 Line Segments, pdf page 493, printed page 471
        // also see section 13.7 in the GL 4.6 spec ("Primitive Clipping")
        
        // almost all GPU's support line widths of up to 32 pixels,
        // and most GPU's support widths of up to 2048 pixels
        // 10 is the max width on nvidia, 7 is max on intel
        
        float widthdiv2 = static_cast<float>(width/2);
        float heightdiv2 = static_cast<float>(height/2);
        float xf = (widthdiv2 * xy[0]) + 0.0f + widthdiv2;
        LOG_MAGNUM_INFO_FUNCTION(xf);
        float yf = (heightdiv2 * xy[1]) + 0.0f + heightdiv2;
        LOG_MAGNUM_INFO_FUNCTION(yf);
        int xi = static_cast<int>(xf);
        int yi = static_cast<int>(yf);
        return {xi, yi};
    }
    
    struct BITMAP_FORMAT_RGBA8 {
        uint8_t R = 0;
        uint8_t G = 0;
        uint8_t B = 0;
        uint8_t A = 255;
    };
    
    int getPixelIndex(int column, int row) {
        // index 0 is column 0 row 0
        // index width is column 0 row 1
        return (width * row) + column;
    }
    
    BITMAP_FORMAT_RGBA8 & getPixel(BITMAP_FORMAT_RGBA8 * data, int column, int row) {
//         assert(1205 == getPixelIndex(5, 3));
        int index = getPixelIndex(column, row);
        
        // if ((400/2) * 1) + 0 + (400/2) is window space 400,
        // but the equivilant pixel location on my screen is 399,
        // and ((400/2) * -1) + 0 + (400/2) is both
        // window space 0 and pixel location 0,
        // what can i do about this?
        
        // specifically, in my application, pixel 400 is drawn at the END of my screen, while pixel 0 is drawn OFF SCREEN from the start of my application, meaning that pixel 0 is offscreen while pixel 1 is the real START of my screen, HOWEVER is conflicts with the window space of NDC 1.0f is pixel 0
        
        // in which, doing the math for NDC Y    ((400/2) * 1) + 0 + (400/2)    ->    (200 * 1) + 0 + 200    ->    200 + 200    ->    400,    and NDC X    ((400/2) * -1) + 0 + (400/2)    ->    (200 * -1) + 0 + 200    ->    -200 + 200    ->    0
        
        // which, based on the actual observed behaviour, either 0 is INVALID, or 400 is INVALID
        
        LOG_MAGNUM_INFO << "column " << column << ", row " << row << " is at index " << index;
        return data[index];
    }
    
    void setPixel(BITMAP_FORMAT_RGBA8 * data, int column, int row, const BITMAP_FORMAT_RGBA8 & pixel) {
        getPixel(data, column, row) = pixel;
    }
    
    void drawRectangleLines(
        BITMAP_FORMAT_RGBA8 * data,
        const Vector2 & topLeft,
        const Vector2 & topRight,
        const Vector2 & bottomRight,
        const Vector2 & bottomLeft
    ) {
        const Vector2i & pixelsTopLeft = NDC_to_WindowSpace(topLeft);
        const Vector2i & pixelsTopRight = NDC_to_WindowSpace(topRight);
        const Vector2i & pixelsBottomRight = NDC_to_WindowSpace(bottomRight);
        const Vector2i & pixelsBottomLeft = NDC_to_WindowSpace(bottomLeft);
        LOG_MAGNUM_INFO << "pixels top left:     " << pixelsTopLeft;
        LOG_MAGNUM_INFO << "pixels top right:    " << pixelsTopRight;
        LOG_MAGNUM_INFO << "pixels bottom right: " << pixelsBottomRight;
        LOG_MAGNUM_INFO << "pixels bottom left:  " << pixelsBottomLeft;
//         setPixel(data, pixelsTopLeft[0], pixelsTopLeft[1], {255, 0, 0, 255});
//         setPixel(data, pixelsTopRight[0], pixelsTopRight[1], {255, 0, 0, 255});
        setPixel(data, pixelsBottomRight[0], pixelsBottomRight[1], {255, 0, 0, 255});
        setPixel(data, pixelsBottomLeft[0], pixelsBottomLeft[1], {255, 0, 0, 255});
    }
    
    void drawTextureRectangle() {
        BITMAP_FORMAT_RGBA8 data[width*height];
        
        const Vector2 & topLeft =     {-globalScale,  globalScale};
        const Vector2 & topRight =    { globalScale,  globalScale};
        const Vector2 & bottomRight = { globalScale, -globalScale};
        const Vector2 & bottomLeft =  {-globalScale, -globalScale};

        drawRectangleLines(data, topLeft, topRight, bottomRight, bottomLeft);

        setTextureData(reinterpret_cast<uint32_t*>(data), width, height);

        GLIS_SurfaceTexture2D * tmp = texture2DRead;
        texture2DRead = texture2DDraw;
        drawPlane();
        texture2DRead = tmp;
    }
    
};
