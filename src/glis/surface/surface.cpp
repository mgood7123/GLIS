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

class GLIS_NDCGrid {
public:
    int width;
    int height;
    float * x = nullptr;
    float * y = nullptr;

    GLIS_NDCGrid() {};
    
    GLIS_NDCGrid(int width, int height) {
        this->width = width;
        this->height = height;
        x = new float[width+1];
        y = new float[height+1];
        float widthdiv2 = static_cast<float>((width)/2);
        float heightdiv2 = static_cast<float>((height)/2);
        for (int i = 0; i <= width; i++) {
            float pixel = static_cast<float>(i);
            float step_1 = pixel - widthdiv2 - 0.0f;
            float step_2 = step_1 / widthdiv2;
            x[i] = step_2;
        }
        for (int i = 0; i <= height; i++) {
            float pixel = static_cast<float>(i);
            float step_1 = pixel - heightdiv2 - 0.0f;
            float step_2 = step_1 / heightdiv2;
            y[i] = step_2;
        }
    }
    
    ~GLIS_NDCGrid() {
        delete[] x;
        delete[] y;
    }
};

class GLIS_NDCGridPixelCentered {
public:
    int width;
    int height;
    float * x = nullptr;
    float * y = nullptr;

    GLIS_NDCGridPixelCentered() {};
    
    GLIS_NDCGridPixelCentered(int width, int height) {
        this->width = width;
        this->height = height;
        x = new float[width+1];
        y = new float[height+1];
        float widthdiv2 = static_cast<float>((width)/2);
        float heightdiv2 = static_cast<float>((height)/2);
        for (int i = 0; i <= width; i++) {
            float pixel = static_cast<float>(i);
            float step_1 = pixel - widthdiv2 - 0.0f;
            float step_2 = step_1 / widthdiv2;
            x[i] = step_2 > 0.0f
            ? step_2 - (1.0f/(2.0f*static_cast<float>(width))) 
            : step_2 + (1.0f/(2.0f*static_cast<float>(width)));
        }
        for (int i = 0; i <= height; i++) {
            float pixel = static_cast<float>(i);
            float step_1 = pixel - heightdiv2 - 0.0f;
            float step_2 = step_1 / heightdiv2;
            y[i] = step_2 > 0.0f
            ? step_2 - (1.0f/(2.0f*static_cast<float>(height))) 
            : step_2 + (1.0f/(2.0f*static_cast<float>(height)));
        }
    }
    
    ~GLIS_NDCGridPixelCentered() {
        delete[] x;
        delete[] y;
    }
    
    void test() {
        GLIS_NDCGrid grid(width, height);
        LOG_MAGNUM_INFO << "testing width";
        for (int i = 0; i <= width; i++) {
            float val = grid.x[i] > 0.0f 
            ? grid.x[i] - (1.0f/(2.0f*static_cast<float>(width))) 
            : grid.x[i] + (1.0f/(2.0f*static_cast<float>(width)));
            auto a = std::to_string(grid.x[i]);
            auto b = std::to_string(val);
            auto c = std::to_string(x[i]);
            LOG_MAGNUM_INFO
            << "testing grid.x[i] ("
            << a.c_str()
            << ") centered ("
            << b.c_str()
            << ") matches x[i] ("
            << c.c_str()
            << ") with i = " << i;
            assert(val == x[i]);
        }
        LOG_MAGNUM_INFO << "testing height";
        for (int i = 0; i <= height; i++) {
            float val = grid.y[i] > 0.0f 
            ? grid.y[i] - (1.0f/(2.0f*static_cast<float>(height))) 
            : grid.y[i] + (1.0f/(2.0f*static_cast<float>(height)));
            auto a = std::to_string(grid.y[i]);
            auto b = std::to_string(val);
            auto c = std::to_string(y[i]);
            LOG_MAGNUM_INFO
            << "testing grid.y[i] ("
            << a.c_str()
            << ") centered ("
            << b.c_str()
            << ") matches y[i] ("
            << c.c_str()
            << ") with i = " << i;
            assert(val == y[i]);
        }
    }
};

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
    GLIS_NDCGridPixelCentered * grid = nullptr;

public:
    // Method to compare which one is the more close. 
    // We find the closest by taking the difference 
    // between the target and both values. It assumes 
    // that val2 is greater than val1 and target lies 
    // between these two. 
    float getClosest(float arr[], int val1, int val2, float target) 
    { 
        if (target - arr[val1] >= arr[val2] - target) 
            return val2;
        else
            return val1; 
    }
    
    // Returns element closest to target in arr[]
    int findClosest(float arr[], int n, float target) {
        // Corner cases
        if (target <= arr[0]) 
            return 0; 
        if (target >= arr[n - 1]) 
            return n - 1;
    
        // Doing binary search 
        int i = 0, j = n, mid = 0;
        while (i < j) { 
            mid = (i + j) / 2; 
    
            if (arr[mid] == target) 
                return mid;
    
            /* If target is less than array element, 
                then search in left */
            if (target < arr[mid]) { 
    
                // If target is greater than previous 
                // to mid, return closest of two 
                if (mid > 0 && target > arr[mid - 1]) 
                    return getClosest(arr, mid - 1, mid, target); 
    
                /* Repeat for left half */
                j = mid; 
            } 
    
            // If target is greater than mid 
            else { 
                if (mid < n - 1 && target < arr[mid + 1]) 
                    return getClosest(arr, mid, 
                                    mid + 1, target); 
                // update i 
                i = mid + 1;
            } 
        } 
    
        // Only single element left after search 
        return mid; 
    }

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
        delete grid;
        grid = nullptr;
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
        delete grid;
        grid = new GLIS_NDCGridPixelCentered(size[0], size[1]);
//         grid->test();
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
//             shaderRead->setTransformationProjectionMatrix(Matrix3::projection({2.005f, 2.005f}));
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
    
    // thanks to imirkin - #xorg-devel
    Vector2 move_NDC_to_pixel_center(Vector2 points) {
        // create a pixel-centered NCD grid to round to nearest DNC center
        // create a pixel grid
        // convert this pixel grid into an NDC grid
        // shift each value in the grid to pixel center
        // round given NDC to nearest NDC center
        
        // assume vector of w,h
        return {
            // shift x
            points[0] > 0.0f 
            ? points[0] - (1.0f/(2.0f*static_cast<float>(width))) 
            : points[0] + (1.0f/(2.0f*static_cast<float>(width))),
            
            // shift y
            points[1] > 0.0f 
            ? points[1] - (1.0f/(2.0f*static_cast<float>(height))) 
            : points[1] + (1.0f/(2.0f*static_cast<float>(height)))
        };
    }
    
    Vector2 NDC_to_WindowSpacef(const Vector2 & xy) {
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
        
        
        // shift NDC values by half a pixel so NDC is on pixel center instead of pixel edge
        // [13:04] <imirkin> ndc value +/- 1/(2*width)
        // [13:04] <imirkin> + on the left, - on the right
        // [13:05] <imirkin> you can't hard-code it as the width/height may change, but you can take them in as uniforms
        
        float widthdiv2 = static_cast<float>(width/2);
        float heightdiv2 = static_cast<float>(height/2);
        float xf = (widthdiv2 * xy[0]) + 0.0f + widthdiv2;
        float yf = (heightdiv2 * xy[1]) + 0.0f + heightdiv2;
        return {xf, yf};
    }
    
    Vector2i NDC_to_WindowSpace(const Vector2 & xy) {
        Vector2 v = NDC_to_WindowSpacef(xy);
        return {static_cast<int>(v[0]), static_cast<int>(v[1])};
    }
    
    Vector2 Correct_NDC(const Vector2 & xy) {
        return {grid->x[findClosest(grid->x, width+1, xy[0])], grid->y[findClosest(grid->y, height+1, xy[1])]};
    }
    
    void drawPlaneWireframe(
        const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
        const Vector2 & topLeft =     {-1.0f,  1.0f},
        const Vector2 & topRight =    { 1.0f,  1.0f},
        const Vector2 & bottomRight = { 1.0f, -1.0f},
        const Vector2 & bottomLeft =  {-1.0f, -1.0f}
    ) {
        LOG_MAGNUM_INFO_FUNCTION(topLeft);
        LOG_MAGNUM_INFO_FUNCTION(Correct_NDC(topLeft));
        LOG_MAGNUM_INFO_FUNCTION(move_NDC_to_pixel_center(topLeft));
        LOG_MAGNUM_INFO_FUNCTION(NDC_to_WindowSpacef(topLeft));
        LOG_MAGNUM_INFO_FUNCTION(NDC_to_WindowSpacef(move_NDC_to_pixel_center(topLeft)));
        draw(
            newShaderRead(),
            color,
            buildPlaneWireframeMesh(
                move_NDC_to_pixel_center(topLeft),
                move_NDC_to_pixel_center(topRight),
                move_NDC_to_pixel_center(bottomRight),
                move_NDC_to_pixel_center(bottomLeft)
            )
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
