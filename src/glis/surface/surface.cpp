//
// Created by smallville7123 on 20/08/20.
//

#include <glis/surface/surface.hpp>

void GLIS_Surface::release() {
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

void GLIS_Surface::newTexture2D(const Magnum::VectorTypeFor<2, int> &size) {
    delete texture2DDraw;
    texture2DDraw = new GLIS_SurfaceTexture2D;
    texture2DDraw->setStorage(1, Magnum::GL::TextureFormat::RGBA8, size);
}

void GLIS_Surface::newFramebuffer(const Magnum::VectorTypeFor<2, int> &size) {
    width = size[0];
    height = size[1];
    if (framebuffer_ == nullptr) framebuffer_ = new GLIS_SurfaceFramebuffer {{{}, size}};
    framebuffer_->setViewport({{}, size});
    newTexture2D(size);
    framebuffer_->attachTexture(Magnum::GL::Framebuffer::ColorAttachment{0}, *texture2DDraw, 0);
    framebuffer_->mapForDraw({{0, {Magnum::GL::Framebuffer::ColorAttachment{0}}}});
}

void GLIS_Surface::resize(const Magnum::VectorTypeFor<2, int> &size) {
    if (framebuffer_ != nullptr) {
        newFramebuffer(size);
    } else {
        width = size[0];
        height = size[1];
        Magnum::GL::defaultFramebuffer.setViewport({{}, size});
    }
    delete grid;
    grid = new GLIS_NDC_Tools::GridPixelCentered(size[0], size[1]);
    grid->test();
}

void GLIS_Surface::resize(const Magnum::VectorTypeFor<2, int> &size1,
                          const Magnum::VectorTypeFor<2, int> &size2) {
    width = size2[0];
    height = size2[1];
    Magnum::GL::defaultFramebuffer.setViewport({size1, size2});
}

void GLIS_Surface::genTextureFromGLTexture(const GLuint &id) {
    if (texture2DDraw != nullptr) delete texture2DDraw;
    texture2DDraw = new Magnum::GL::Texture2D;
    *texture2DDraw = Magnum::GL::Texture2D::wrap(id);
}

void GLIS_Surface::bind() {
    framebuffer_ != nullptr ? framebuffer_->bind() : Magnum::GL::defaultFramebuffer.bind();
}

void GLIS_Surface::clear() {
    if (framebuffer_ != nullptr) framebuffer_->clear(Magnum::GL::FramebufferClear::Color);
    else Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);
}

void GLIS_Surface::bindAnyClear() {
    bind();
    clear();
}

Magnum::Range2Di GLIS_Surface::getViewport() {
    return framebuffer_ != nullptr ? framebuffer_->viewport() : Magnum::GL::defaultFramebuffer.viewport();
}

GLIS_SurfaceShader *GLIS_Surface::newShaderReadTexture() {
    if (shaderReadTexture == nullptr) shaderReadTexture = new GLIS_SurfaceShader(Magnum::Shaders::Flat2D::Flag::Textured);
    return shaderReadTexture;
}

GLIS_SurfaceShader *GLIS_Surface::newShaderRead() {
    if (shaderRead == nullptr) shaderRead = new GLIS_SurfaceShader;
    return shaderRead;
}

void GLIS_Surface::draw(GLIS_SurfaceShader *shader, const GLIS_SurfaceColor &color,
                        Magnum::GL::Mesh &&mesh) {
    if (texture2DRead != nullptr) shader->bindTexture(*texture2DRead);
    shader
            ->setColor(color)
            .draw(mesh);
}

Magnum::GL::Mesh
GLIS_Surface::buildTriangleMesh(const Magnum::Vector2 &Left, const Magnum::Vector2 &Right,
                                const Magnum::Vector2 &Top) {
    struct TriangleVertex {
        Magnum::Vector2 position;
        Magnum::Vector2 textureCoordinates;
    };

    const TriangleVertex data[]{
            {Left , {0.0f, 0.0f}}, /* Left position and texture coordinate for mapping textures */
            {Right, {1.0f, 0.0f}}, /* Right position and texture coordinate for mapping textures */
            {Top  , {0.5f, 1.0f}}  /* Top position and texture coordinate for mapping textures */
    };

    Magnum::GL::Buffer buffer(data);

    Magnum::GL::Mesh mesh;
    mesh
            .setCount(3)
            .addVertexBuffer(
                    std::move(buffer), 0,
                    Magnum::GL::Attribute<0, Magnum::Vector2> {},
                    Magnum::GL::Attribute<1, Magnum::Vector2> {}
            );
    return mesh;
}

void GLIS_Surface::drawTriangle(const GLIS_SurfaceColor &color, const Magnum::Vector2 &Left,
                                const Magnum::Vector2 &Right, const Magnum::Vector2 &Top) {
    if (texture2DRead != nullptr) {
        draw(newShaderReadTexture(), surfaceTextureColor, buildTriangleMesh(Left, Right, Top));
    } else {
        draw(newShaderRead(), color, buildTriangleMesh(Left, Right, Top));
    }
}

void GLIS_Surface::drawTriangle(const GLIS_Surface &surface, const GLIS_SurfaceColor &color,
                                const Magnum::Vector2 &Left, const Magnum::Vector2 &Right,
                                const Magnum::Vector2 &Top) {
    GLIS_SurfaceTexture2D * tmp = texture2DRead;
    texture2DRead = surface.texture2DDraw;
    drawTriangle(color, Left, Right, Top);
    texture2DRead = tmp;
}

Magnum::GL::Mesh GLIS_Surface::buildPlaneWireframeMesh(const Magnum::Vector2 &topLeft,
                                                       const Magnum::Vector2 &topRight,
                                                       const Magnum::Vector2 &bottomRight,
                                                       const Magnum::Vector2 &bottomLeft) {
    const struct Vertex {
        Magnum::Vector2 position;
    } vertex[] {
            {grid->Correct_NDC(topRight)},    {grid->Correct_NDC(bottomRight)}, // right
            {grid->Correct_NDC(bottomRight)}, {grid->Correct_NDC(bottomLeft)},  // bottom
            {grid->Correct_NDC(bottomLeft)},  {grid->Correct_NDC(topLeft)},     // left
            {grid->Correct_NDC(topLeft)},     {grid->Correct_NDC(topRight)}     // top
    };

    Magnum::GL::Buffer vertices(vertex);

//         LOG_MAGNUM_INFO_FUNCTION(topLeft);
//         LOG_MAGNUM_INFO_FUNCTION(NDC_to_WindowSpace(topLeft));
//         LOG_MAGNUM_INFO_FUNCTION(NDC_to_WindowSpacei(topLeft));
//         auto tl = grid->Correct_NDC(topLeft);
//         LOG_MAGNUM_INFO_FUNCTION(tl);
//         LOG_MAGNUM_INFO_FUNCTION(NDC_to_WindowSpace(tl));
//         LOG_MAGNUM_INFO_FUNCTION(NDC_to_WindowSpacei(tl));

    Magnum::GL::Mesh mesh;
    mesh
            .setPrimitive(Magnum::MeshPrimitive::Lines)
            .addVertexBuffer(std::move(vertices), 0, Magnum::GL::Attribute<0, Magnum::Vector2> {})
            .setCount(sizeof(vertex)/sizeof(Vertex));
    return mesh;
}

void
GLIS_Surface::drawPlaneWireframe(const GLIS_SurfaceColor &color, const Magnum::Vector2 &topLeft,
                                 const Magnum::Vector2 &topRight,
                                 const Magnum::Vector2 &bottomRight,
                                 const Magnum::Vector2 &bottomLeft) {
    draw(
            newShaderRead(),
            color,
            buildPlaneWireframeMesh(topLeft, topRight, bottomRight, bottomLeft)
    );
}

Magnum::GL::Mesh
GLIS_Surface::buildPlaneMesh(const Magnum::Vector2 &topLeft, const Magnum::Vector2 &topRight,
                             const Magnum::Vector2 &bottomRight, const Magnum::Vector2 &bottomLeft) {
    const struct Vertex {
        Magnum::Vector2 position;
        Magnum::Vector2 textureCoordinates;
    } vertex[] {
            {topRight,    {1.0f, 1.0f}},
            {bottomRight, {1.0f, 0.0f}},
            {bottomLeft,  {0.0f, 0.0f}},
            {topLeft,     {0.0f, 1.0f}}
    };

    Magnum::UnsignedInt indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

    Magnum::GL::Buffer vertexBuffer(vertex);
    Magnum::GL::Buffer indexBuffer(indices);

    Magnum::GL::Mesh mesh;
    mesh
            .setPrimitive(Magnum::MeshPrimitive::TriangleStrip)
            .addVertexBuffer(
                    std::move(vertexBuffer), 0,
                    Magnum::GL::Attribute<0, Magnum::Vector2> {}, Magnum::GL::Attribute<1, Magnum::Vector2> {}
            )
            .setCount(sizeof(vertex)/sizeof(Vertex))
            .setIndexBuffer(
                    std::move(indexBuffer), 0, Magnum::GL::MeshIndexType::UnsignedInt
            )
            .setCount(sizeof(indices)/sizeof(Magnum::UnsignedInt))
            ;
    return mesh;
}

void GLIS_Surface::drawPlane(const GLIS_SurfaceColor &color, const Magnum::Vector2 &topLeft,
                             const Magnum::Vector2 &topRight, const Magnum::Vector2 &bottomRight,
                             const Magnum::Vector2 &bottomLeft) {
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

void GLIS_Surface::drawPlane(const GLIS_Surface &surface, const GLIS_SurfaceColor &color,
                             const Magnum::Vector2 &topLeft, const Magnum::Vector2 &topRight,
                             const Magnum::Vector2 &bottomRight, const Magnum::Vector2 &bottomLeft) {
    GLIS_SurfaceTexture2D * tmp = texture2DRead;
    texture2DRead = surface.texture2DDraw;
    drawPlane(color, topLeft, topRight, bottomRight, bottomLeft);
    texture2DRead = tmp;
}
