//
// Created by smallville7123 on 19/07/20.
//

#include <glis/glis.hpp>
#include <Magnum/Mesh.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Primitives/Grid.h>
#include <Magnum/Primitives/Line.h>
#include <magnum/src/Magnum/Text/AbstractFont.h>
#include <magnum/src/Magnum/Text/GlyphCache.h>
#include <magnum/src/Magnum/Shaders/Vector.h>
#include <magnum/src/Magnum/Text/Alignment.h>
#include <Magnum/Text/Renderer.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <corrade/src/Corrade/PluginManager/Manager.h>
#include <MagnumPlugins/MagnumFont/MagnumFont.h>
#include <corrade/src/Corrade/Utility/Resource.h>

GLIS_CLASS CompositorMain;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

using namespace Magnum;

class grid {
    GL::Mesh * _mesh = nullptr;
    Shaders::Flat2D * _shader = nullptr;
    int grid_rows = 0;
    int grid_columns = 0;

public:
    bool drawLabels = false;
    void create();
    void setSize(int rows, int columns);
    void setColor(const Color4 &color);
    void createMesh();
    void draw();
    void release();
};

void grid::create() {
    _shader = new Shaders::Flat2D;
    _mesh = new GL::Mesh;
}

void grid::setSize(int rows, int columns) {
    grid_rows = rows;
    grid_columns = columns;
}

void grid::setColor(const Color4 &color) {
    _shader->setColor(color);
}

void grid::createMesh(){
    Vector2i subDivisions = {grid_columns-1, grid_rows-1};
    *_mesh = MeshTools::compile(Primitives::grid3DWireframe(subDivisions));
}

void grid::draw() {
    _shader->draw(*_mesh);
}

void grid::release() {
    delete _shader;
    delete _mesh;
}

class line {
    GL::Mesh * _mesh = nullptr;
    Shaders::Flat2D * _shader = nullptr;
    float x1 = 0;
    float y1 = 0;
    float x2 = 0;
    float y2 = 0;

public:
    void create();
    void setStart(float x, float y);
    void setEnd(float x, float y);
    void setColor(const Color4 &color);
    void createMesh();
    void draw();
    void release();
};

void line::create() {
    _shader = new Shaders::Flat2D;
    _mesh = new GL::Mesh;
}

void line::setStart(float x, float y) {
    x1 = x;
    y2 = y;
}

void line::setEnd(float x, float y) {
    x = x;
    y2 = y;
}

void line::setColor(const Color4 &color) {
    _shader->setColor(color);
}

void line::createMesh(){
    Vector2 from = {x1, y1};
    Vector2 to = {x2, y2};

    *_mesh = MeshTools::compile(Primitives::line2D(from, to));
}

void line::draw() {
    _shader->draw(*_mesh);
}

void line::release() {
    delete _shader;
    delete _mesh;
}

grid dnc_x1, dnc_x2, dnc_x3;

GL::Mesh * mesh = nullptr;
Shaders::Vector2D * shader = nullptr;
/* Font instance, received from a plugin manager */
Containers::Pointer<Text::AbstractFont> * _font = nullptr;
Text::GlyphCache * cache = nullptr;
GL::Buffer * vertexBuffer = nullptr, *indexBuffer = nullptr;

GLIS_CALLBACKS_DRAW(draw, glis, renderer, font, fps) {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);
    dnc_x1.draw();
    shader->draw(*mesh);
    glis.GLIS_SwapBuffers(CompositorMain);
}

GLIS_CALLBACKS_RESIZE(resize, glis, renderer, font, fps, width, height) {
    glViewport(0, 0, width, height);
}

GLIS_CALLBACKS_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(CompositorMain);
    // order does not matter
    dnc_x1.release();
    delete vertexBuffer;
    delete indexBuffer;
    delete cache;
    delete _font;
    delete shader;
    delete mesh;
    glis.GLIS_destroy_GLIS(CompositorMain);
}

int main() {
    glis.getX11Window(CompositorMain, 400, 400);
    glis.GLIS_setupOnScreenRendering(CompositorMain);
    CompositorMain.contextMagnum.create();

    /* Load a TrueTypeFont plugin and open the font */
    PluginManager::Manager<Text::AbstractFont> _manager;
    _font = new Containers::Pointer<Text::AbstractFont>;

    Utility::Resource rs("fonts");
    // better performance can be obtained by using FreeTypeFont
    *_font = _manager.loadAndInstantiate("TrueTypeFont");
    if(!_font || !_font[0]->openData(rs.getRaw("Vera.ttf"), 180.0f))
        LOG_MAGNUM_FATAL << "Cannot open font file";

    cache = new Text::GlyphCache{Vector2i{512*2}};
    LOG_MAGNUM_DEBUG << "filling glyph cache";
    auto s = now_ms();
    _font[0]->fillGlyphCache(*cache, "Hello World!");
    auto e = now_ms();
    LOG_MAGNUM_DEBUG << "filled glyph cache in" << e - s << "milliseconds";

    mesh = new GL::Mesh;
    shader = new Shaders::Vector2D;
    vertexBuffer = new GL::Buffer;
    indexBuffer = new GL::Buffer;

    /* Render the text, centered */
    std::tie(*mesh, std::ignore) = Text::Renderer2D::render(
            **_font,
            *cache,
            0.15f,
            "Hello World!",
            *vertexBuffer,
            *indexBuffer,
            GL::BufferUsage::StaticDraw,
            Text::Alignment::LineCenter
    );

    /* Draw the text on the screen */
    shader->setColor({1.0f, 1.0f, 1.0f, 1.0f});
    shader->bindVectorTexture(cache->texture());


    dnc_x1.create();
    dnc_x1.setSize(10, 10);
    dnc_x1.setColor({1.0f, 0.0f, 0.0f, 1.0f});
    dnc_x1.createMesh();
    dnc_x1.drawLabels = true;

//    dnc_x2.create();
//    dnc_x2.setSize(20, 20);
//    dnc_x2.setColor({0.0f, 1.0f, 0.0f, 1.0f});
//    dnc_x2.createMesh();
//
//    dnc_x3.create();
//    dnc_x3.setSize(40, 40);
//    dnc_x3.setColor({0.0f, 0.0f, 1.0f, 1.0f});
//    dnc_x3.createMesh();

    glis.runUntilX11WindowClose(glis, CompositorMain, font, fps, draw, resize, close);
}