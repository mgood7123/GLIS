//
// Created by smallville7123 on 20/08/20.
//

#include <glis/surface/surface.hpp>

// TODO: add an optimizing command queue, this will trade speed for speed
//  specifically, if run-time optimization takes longer than draw time
//  then optimization is generally not recommended
//  since drawing will be faster than optimizing
//  as an example, if it takes 2ms to draw an unoptimized command queue
//  but it takes 3ms to optimize, and 1ms to draw an optimized command queue
//  then it would be faster to draw the unoptimized command queue
//  as it takes 2ms to draw an unoptimized command queue
//  while it takes 4ms to draw an optimized command queue
//  with the bottleneck being the time taken to optimize the command queue
//  this is all at runtime however
//  if we can move optimization into compile-time
//  then drawing an optimized command queue will always be
//  at least as fast as drawing an unoptimized command queue
//  and thus would have no trade-off's
//  Template<> functions CANNOT be used as a compile-time construct since
//  they only only generate functions with substituted type info
//  and cannot generate actual new code

Magnum::GL::Mesh GLIS_Mesh::buildTriangleWireframeMesh(const Magnum::Vector2 &left,
                                                    const Magnum::Vector2 &right,
                                                    const Magnum::Vector2 &top) {
    const struct Vertex {
        Magnum::Vector2 position;
    } vertex[] {
            {grid->Correct_NDC(left)},    {grid->Correct_NDC(right)},
            {grid->Correct_NDC(right)}, {grid->Correct_NDC(top)},
            {grid->Correct_NDC(top)},  {grid->Correct_NDC(left)}
    };

    Magnum::GL::Buffer vertices(vertex);

    Magnum::GL::Mesh mesh;
    mesh
            .setPrimitive(Magnum::MeshPrimitive::Lines)
            .addVertexBuffer(std::move(vertices), 0, Magnum::GL::Attribute<0, Magnum::Vector2> {})
            .setCount(sizeof(vertex)/sizeof(Vertex));
    return mesh;
}

Magnum::GL::Mesh GLIS_Mesh::buildTriangleMesh(const Magnum::Vector2 &left,
                                              const Magnum::Vector2 &right,
                                              const Magnum::Vector2 &top) {
    struct TriangleVertex {
        Magnum::Vector2 position;
        Magnum::Vector2 textureCoordinates;
    };

    const TriangleVertex data[]{
            {left , {0.0f, 0.0f}}, /* Left position and texture coordinate for mapping textures */
            {right, {1.0f, 0.0f}}, /* Right position and texture coordinate for mapping textures */
            {top  , {0.5f, 1.0f}}  /* Top position and texture coordinate for mapping textures */
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

Magnum::GL::Mesh GLIS_Mesh::buildPlaneWireframeMesh(const Magnum::Vector2 &topLeft,
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

    Magnum::GL::Mesh mesh;
    mesh
            .setPrimitive(Magnum::MeshPrimitive::Lines)
            .addVertexBuffer(std::move(vertices), 0, Magnum::GL::Attribute<0, Magnum::Vector2> {})
            .setCount(sizeof(vertex)/sizeof(Vertex));
    return mesh;
}

Magnum::GL::Mesh
GLIS_Mesh::buildPlaneMesh(const Magnum::Vector2 &topLeft, const Magnum::Vector2 &topRight,
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
    delete mesh.grid;
    mesh.grid = nullptr;
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
    delete mesh.grid;
    mesh.grid = new GLIS_NDC_Tools::GridPixelCentered(size[0], size[1]);
    if (width > 1 && height > 1) mesh.grid->test();
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

void GLIS_Surface::bindAndClear() {
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

void GLIS_Surface::drawTriangleWireframe(const GLIS_SurfaceColor &color, const Magnum::Vector2 &left,
                                const Magnum::Vector2 &right, const Magnum::Vector2 &top) {
    if (texture2DRead != nullptr) {
        draw(newShaderReadTexture(), surfaceTextureColor, this->mesh.buildTriangleWireframeMesh(left, right, top));
    } else {
        draw(newShaderRead(), color, this->mesh.buildTriangleWireframeMesh(left, right, top));
    }
}

void GLIS_Surface::drawTriangle(const GLIS_SurfaceColor &color, const Magnum::Vector2 &left,
                                const Magnum::Vector2 &right, const Magnum::Vector2 &top) {
    if (texture2DRead != nullptr) {
        draw(newShaderReadTexture(), surfaceTextureColor, this->mesh.buildTriangleMesh(left, right, top));
    } else {
        draw(newShaderRead(), color, this->mesh.buildTriangleMesh(left, right, top));
    }
}

void GLIS_Surface::drawTriangle(const GLIS_SurfaceTexture2D * texture, const Magnum::Vector2 &left,
                                const Magnum::Vector2 &right, const Magnum::Vector2 &top) {
    GLIS_SurfaceTexture2D * tmp = texture2DRead;
    texture2DRead = const_cast<GLIS_SurfaceTexture2D *>(texture);
    drawTriangle(GLIS_SurfaceColor {0,0,0,0}, left, right, top);
    texture2DRead = tmp;
}

void GLIS_Surface::drawTriangle(const GLIS_Surface &surface, const Magnum::Vector2 &left,
                                const Magnum::Vector2 &right, const Magnum::Vector2 &top) {
    drawTriangle(surface.texture2DDraw, left, right, top);
}

void
GLIS_Surface::drawPlaneWireframe(const GLIS_SurfaceColor &color, const Magnum::Vector2 &topLeft,
                                 const Magnum::Vector2 &topRight,
                                 const Magnum::Vector2 &bottomRight,
                                 const Magnum::Vector2 &bottomLeft) {
    draw(
            newShaderRead(),
            color,
            this->mesh.buildPlaneWireframeMesh(topLeft, topRight, bottomRight, bottomLeft)
    );
}

void GLIS_Surface::drawPlane(const GLIS_SurfaceColor &color, const Magnum::Vector2 &topLeft,
                             const Magnum::Vector2 &topRight, const Magnum::Vector2 &bottomRight,
                             const Magnum::Vector2 &bottomLeft) {
    if (texture2DRead != nullptr) {
        draw(newShaderReadTexture(), surfaceTextureColor, this->mesh.buildPlaneMesh(topLeft, topRight, bottomRight, bottomLeft));
    } else {
        draw(newShaderRead(), color, this->mesh.buildPlaneMesh(topLeft, topRight, bottomRight, bottomLeft));
    }
}

void GLIS_Surface::drawPlane(const GLIS_SurfaceTexture2D * texture,
                             const Magnum::Vector2 &topLeft, const Magnum::Vector2 &topRight,
                             const Magnum::Vector2 &bottomRight, const Magnum::Vector2 &bottomLeft) {
    GLIS_SurfaceTexture2D * tmp = texture2DRead;
    texture2DRead = const_cast<GLIS_SurfaceTexture2D *>(texture);
    drawPlane(GLIS_SurfaceColor {0,0,0,0}, topLeft, topRight, bottomRight, bottomLeft);
    texture2DRead = tmp;
}

void GLIS_Surface::drawPlane(const GLIS_Surface &surface,
                             const Magnum::Vector2 &topLeft, const Magnum::Vector2 &topRight,
                             const Magnum::Vector2 &bottomRight, const Magnum::Vector2 &bottomLeft) {
    drawPlane(surface.texture2DDraw, topLeft, topRight, bottomRight, bottomLeft);
}

void
GLIS_Surface::drawPlaneWireframeCorners(const GLIS_SurfaceColor &color, const Magnum::Vector2 &topLeft,
                                 const Magnum::Vector2 &bottomRight) {
    // the X of top right is the X of bottom right
    // the Y of top right is the Y of top left
    const Magnum::Vector2 topRight {bottomRight[0], topLeft[1]};
    const Magnum::Vector2 bottomLeft {topLeft[0], bottomRight[1]};

    drawPlaneWireframe(
            color,
            topLeft,
            topRight,
            bottomRight,
            bottomLeft
    );
}

void GLIS_Surface::drawPlaneCorners(const GLIS_SurfaceColor &color, const Magnum::Vector2 &topLeft,
                             const Magnum::Vector2 &bottomRight) {
    // the X of top right is the X of bottom right
    // the Y of top right is the Y of top left
    const Magnum::Vector2 topRight {bottomRight[0], topLeft[1]};
    const Magnum::Vector2 bottomLeft {topLeft[0], bottomRight[1]};

    drawPlane(
            color,
            topLeft,
            topRight,
            bottomRight,
            bottomLeft
    );
}

void GLIS_Surface::drawPlaneCorners(const GLIS_SurfaceTexture2D * texture, const Magnum::Vector2 &topLeft,
                                    const Magnum::Vector2 &bottomRight) {
    // the X of top right is the X of bottom right
    // the Y of top right is the Y of top left
    const Magnum::Vector2 topRight {bottomRight[0], topLeft[1]};
    const Magnum::Vector2 bottomLeft {topLeft[0], bottomRight[1]};

    drawPlane(
            texture,
            topLeft,
            topRight,
            bottomRight,
            bottomLeft
    );
}

void GLIS_Surface::drawPlaneCorners(const GLIS_Surface &surface, const Magnum::Vector2 &topLeft,
                                    const Magnum::Vector2 &bottomRight) {
    drawPlaneCorners(surface.texture2DDraw, topLeft, bottomRight);
}
