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

class GLIS_Mesh {
public:
    GLIS_NDC_Tools::GridPixelCentered * grid = nullptr;

    Magnum::GL::Mesh buildTriangleWireframeMesh(
            const Magnum::Vector2 & left,
            const Magnum::Vector2 & right,
            const Magnum::Vector2 & top
    );

    Magnum::GL::Mesh buildTriangleMesh(
            const Magnum::Vector2 & left,
            const Magnum::Vector2 & right,
            const Magnum::Vector2 & top
    );

    Magnum::GL::Mesh buildPlaneWireframeMesh(
            const Magnum::Vector2 & topLeft,
            const Magnum::Vector2 & topRight,
            const Magnum::Vector2 & bottomRight,
            const Magnum::Vector2 & bottomLeft
    );

    Magnum::GL::Mesh buildPlaneMesh(
            const Magnum::Vector2 & topLeft,
            const Magnum::Vector2 & topRight,
            const Magnum::Vector2 & bottomRight,
            const Magnum::Vector2 & bottomLeft
    );
};

class GLIS_Surface {
public:
    GLIS_SurfaceFramebuffer * framebuffer_ = nullptr;
    GLIS_SurfaceImageView2D * image_ = nullptr;
    GLIS_SurfaceImageData * data_ = nullptr;
    GLIS_SurfaceTexture2D * texture2DRead = nullptr;
    GLIS_SurfaceTexture2D * texture2DDraw = nullptr;
    GLIS_SurfaceShader * shaderRead = nullptr;
    GLIS_SurfaceShader * shaderReadTexture = nullptr;
    GLIS_SurfaceShader * shaderDraw = nullptr;

    GLIS_SurfaceShader * newShaderReadTexture();

    GLIS_SurfaceShader * newShaderRead();

    GLIS_Mesh mesh;

    int width = 0;
    int height = 0;

    static constexpr float globalScale = 1.0f;

    void release();

    void newTexture2D(const Magnum::VectorTypeFor<2, int> & size);

    void newFramebuffer(const Magnum::VectorTypeFor<2, int> & size);

    void resize(const Magnum::VectorTypeFor<2, int> & size);

    void resize(const Magnum::VectorTypeFor<2, int> & size1, const Magnum::VectorTypeFor<2, int> & size2);

    template <typename T> void setTextureData(const T * data, int texture_width, int texture_height) {

        newTexture2D({texture_width, texture_height});

        texture2DDraw->setSubImage(
            0,
            {},
            std::move(
                    Magnum::ImageView2D(
                        Magnum::PixelFormat::RGBA8Unorm,
                        {texture_width, texture_height},
                        std::move(
                                Corrade::Containers::ArrayView<const T> (
                                        data,
                                        texture_width*texture_height
                                )
                        )
                    )

            )
        ).generateMipmap();
    }

    void genTextureFromGLTexture(const GLuint & id);

    void bind();

    void clear();

    void bindAndClear();

    Magnum::Range2Di getViewport();

    void draw(GLIS_SurfaceShader * shader, const GLIS_SurfaceColor & color, Magnum::GL::Mesh && mesh);

    void drawTriangleWireframe(
            const GLIS_SurfaceColor & color = {1.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & left  = {-globalScale, -globalScale},
            const Magnum::Vector2 & right = { globalScale, -globalScale},
            const Magnum::Vector2 & top   = { 0.0f,  globalScale}
    );

    void drawTriangle(
            const GLIS_SurfaceColor & color = {1.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & left  = {-globalScale, -globalScale},
            const Magnum::Vector2 & right = { globalScale, -globalScale},
            const Magnum::Vector2 & top   = { 0.0f,  globalScale}
    );

    void drawTriangle(
            const GLIS_SurfaceTexture2D * texture,
            const Magnum::Vector2 & left  = {-globalScale, -globalScale},
            const Magnum::Vector2 & right = { globalScale, -globalScale},
            const Magnum::Vector2 & top   = { 0.0f,  globalScale}
    );

    void drawTriangle(
            const GLIS_Surface & surface,
            const Magnum::Vector2 & left  = {-globalScale, -globalScale},
            const Magnum::Vector2 & right = { globalScale, -globalScale},
            const Magnum::Vector2 & top   = { 0.0f,  globalScale}
    );

    void drawPlaneWireframe(
            const GLIS_SurfaceColor & color = {1.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & topLeft =     {-1.0f,  1.0f},
            const Magnum::Vector2 & topRight =    { 1.0f,  1.0f},
            const Magnum::Vector2 & bottomRight = { 1.0f, -1.0f},
            const Magnum::Vector2 & bottomLeft =  {-1.0f, -1.0f}
    );

    void drawPlane(
            const GLIS_SurfaceColor & color = {1.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & topLeft =     {-globalScale,  globalScale},
            const Magnum::Vector2 & topRight =    { globalScale,  globalScale},
            const Magnum::Vector2 & bottomRight = { globalScale, -globalScale},
            const Magnum::Vector2 & bottomLeft =  {-globalScale, -globalScale}
    );

    void drawPlane(
            const GLIS_SurfaceTexture2D * texture,
            const Magnum::Vector2 & topLeft =     {-globalScale,  globalScale},
            const Magnum::Vector2 & topRight =    { globalScale,  globalScale},
            const Magnum::Vector2 & bottomRight = { globalScale, -globalScale},
            const Magnum::Vector2 & bottomLeft =  {-globalScale, -globalScale}
    );

    void drawPlane(
            const GLIS_Surface & surface,
            const Magnum::Vector2 & topLeft =     {-globalScale,  globalScale},
            const Magnum::Vector2 & topRight =    { globalScale,  globalScale},
            const Magnum::Vector2 & bottomRight = { globalScale, -globalScale},
            const Magnum::Vector2 & bottomLeft =  {-globalScale, -globalScale}
    );

    void drawPlaneWireframeCorners(
            const GLIS_SurfaceColor & color = {1.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & topLeft =     {-globalScale,  globalScale},
            const Magnum::Vector2 & bottomRight = { globalScale, -globalScale}
    );

    void drawPlaneCorners(
            const GLIS_SurfaceColor & color = {1.0f,  1.0f,  1.0f,  1.0f},
            const Magnum::Vector2 & topLeft =     {-globalScale,  globalScale},
            const Magnum::Vector2 & bottomRight = { globalScale, -globalScale}
    );

    void drawPlaneCorners(
            const GLIS_SurfaceTexture2D * texture,
            const Magnum::Vector2 & topLeft =     {-globalScale,  globalScale},
            const Magnum::Vector2 & bottomRight = { globalScale, -globalScale}
    );

    void drawPlaneCorners(
            const GLIS_Surface & surface,
            const Magnum::Vector2 & topLeft =     {-globalScale,  globalScale},
            const Magnum::Vector2 & bottomRight = { globalScale, -globalScale}
    );
};

#endif //ANDROIDCOMPOSITOR_SURFACE_HPP
