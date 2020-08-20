//
// Created by smallville7123 on 20/08/20.
//

#ifndef ANDROIDCOMPOSITOR_SURFACE_HPP
#define ANDROIDCOMPOSITOR_SURFACE_HPP

#include <glis/surface/ndc_grid.hpp>

typedef Magnum::GL::Framebuffer GLIS_SurfaceFramebuffer;
typedef Magnum::ImageView2D GLIS_SurfaceImageView2D;
typedef Corrade::Containers::ArrayView<const void> GLIS_SurfaceImageData;
typedef Magnum::GL::Texture2D GLIS_SurfaceTexture2D;
typedef Magnum::Shaders::Flat2D GLIS_SurfaceShader;
typedef Magnum::Color4 GLIS_SurfaceColor;

constexpr GLIS_SurfaceColor surfaceTextureColor = {1.0f,  1.0f,  1.0f,  1.0f};

// https://gamedev.stackexchange.com/a/110358 software rendering and opengl rendering details

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
    GLIS_NDC_Tools::GridPixelCentered * grid = nullptr;

public:

    int width = 0;
    int height = 0;

    static constexpr float globalScale = 1.0f;

    void release();

    void newTexture2D(const Magnum::VectorTypeFor<2, int> & size);

    void newFramebuffer(const Magnum::VectorTypeFor<2, int> & size);

    void resize(const Magnum::VectorTypeFor<2, int> & size);

    void resize(const Magnum::VectorTypeFor<2, int> & size1, const Magnum::VectorTypeFor<2, int> & size2);

    template <typename T> void setTextureData(const T * data, int texture_width, int texture_height) {

        Corrade::Containers::ArrayView<const T> data_ (data, texture_width*texture_height);
        Magnum::ImageView2D image_ (Magnum::PixelFormat::RGBA8Unorm, {texture_width, texture_height}, std::move(data_));

        newTexture2D({texture_width, texture_height});

        texture2DDraw
                ->setSubImage(0, {}, std::move(image_))
                .generateMipmap();
    }

    void genTextureFromGLTexture(const GLuint & id);

    void bind();

    void clear();

    void bindAnyClear();

    Magnum::Range2Di getViewport();

    GLIS_SurfaceShader * newShaderReadTexture();

    GLIS_SurfaceShader * newShaderRead();

    void draw(GLIS_SurfaceShader * shader, const GLIS_SurfaceColor & color, Magnum::GL::Mesh && mesh);

    Magnum::GL::Mesh buildTriangleMesh(
            const Magnum::Vector2 & Left,
            const Magnum::Vector2 & Right,
            const Magnum::Vector2 & Top
    );

    void drawTriangle(
            const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & Left  = {-globalScale, -globalScale},
            const Magnum::Vector2 & Right = { globalScale, -globalScale},
            const Magnum::Vector2 & Top   = { 0.0f,  globalScale}
    );

    void drawTriangle(
            const GLIS_Surface & surface,
            const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & Left  = {-globalScale, -globalScale},
            const Magnum::Vector2 & Right = { globalScale, -globalScale},
            const Magnum::Vector2 & Top   = { 0.0f,  globalScale}
    );

    Magnum::GL::Mesh buildPlaneWireframeMesh(
            const Magnum::Vector2 & topLeft,
            const Magnum::Vector2 & topRight,
            const Magnum::Vector2 & bottomRight,
            const Magnum::Vector2 & bottomLeft
    );

    void drawPlaneWireframe(
            const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & topLeft =     {-1.0f,  1.0f},
            const Magnum::Vector2 & topRight =    { 1.0f,  1.0f},
            const Magnum::Vector2 & bottomRight = { 1.0f, -1.0f},
            const Magnum::Vector2 & bottomLeft =  {-1.0f, -1.0f}
    );

    Magnum::GL::Mesh buildPlaneMesh(
            const Magnum::Vector2 & topLeft,
            const Magnum::Vector2 & topRight,
            const Magnum::Vector2 & bottomRight,
            const Magnum::Vector2 & bottomLeft
    );

    void drawPlane(
            const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & topLeft =     {-globalScale,  globalScale},
            const Magnum::Vector2 & topRight =    { globalScale,  globalScale},
            const Magnum::Vector2 & bottomRight = { globalScale, -globalScale},
            const Magnum::Vector2 & bottomLeft =  {-globalScale, -globalScale}
    );

    void drawPlane(
            const GLIS_Surface & surface,
            const GLIS_SurfaceColor & color = {0.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & topLeft =     {-globalScale,  globalScale},
            const Magnum::Vector2 & topRight =    { globalScale,  globalScale},
            const Magnum::Vector2 & bottomRight = { globalScale, -globalScale},
            const Magnum::Vector2 & bottomLeft =  {-globalScale, -globalScale}
    );



/*
    // texture manipulation

    // https://en.wikibooks.org/wiki/GLSL_Programming/Rasterization
    //     explains why a NDC point of -1 needs to be centered for it to be lit up
    //     as it seems like, to correctly do software drawing i will need to implement/emulate rasterization

    struct BITMAP_FORMAT_RGBA8 {
        uint8_t R = 0;
        uint8_t G = 0;
        uint8_t B = 0;
        uint8_t A = 255;
    };

    void drawTextureRectangle() {
        BITMAP_FORMAT_RGBA8 data[width*height];

        setTextureData(reinterpret_cast<uint32_t*>(data), width, height);

        GLIS_SurfaceTexture2D * tmp = texture2DRead;
        texture2DRead = texture2DDraw;
        drawPlane();
        texture2DRead = tmp;
    }
*/
};

#endif //ANDROIDCOMPOSITOR_SURFACE_HPP
