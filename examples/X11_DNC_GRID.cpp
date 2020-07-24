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
#include <Corrade/PluginManager/Manager.h>
#include <MagnumPlugins/MagnumFont/MagnumFont.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/Math/Matrix3.h>
#include <magnum/src/Magnum/GL/DebugOutput.h>

GLIS_CLASS screen;
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

namespace Magnum {
    namespace Font {
        class BasicFont {
            Shaders::Vector2D *shader = nullptr;
            Containers::Pointer<Text::AbstractFont> *font = nullptr;
            Containers::Pointer<PluginManager::Manager<Text::AbstractFont>> *manager = nullptr;
            Containers::Pointer<Text::Renderer2D> *text = nullptr;
            Containers::Pointer<Text::GlyphCache> *cache = nullptr;
            float openData_size = 0;

        public:
            void create();

            void load(
                    const Containers::ArrayView<const char> resource,
                    const Containers::ArrayView<const char> fontPlugin,
                    const Containers::ArrayView<const char> font,
                    const float dpi
            );

            void draw(const Containers::ArrayView<const char> str, float size, float x, float y,
                      Text::Alignment alignment);

            void release();
        };

        void BasicFont::create() {
            shader = new Shaders::Vector2D;
            font = new Containers::Pointer<Text::AbstractFont>;
            manager = new Containers::Pointer<PluginManager::Manager<Text::AbstractFont>>(
                    new PluginManager::Manager<Text::AbstractFont>
            );
            cache = new Containers::Pointer<Text::GlyphCache>(new Text::GlyphCache{Vector2i{4096}});
        }

        void BasicFont::load(
                const Containers::ArrayView<const char> resource,
                const Containers::ArrayView<const char> fontPlugin,
                const Containers::ArrayView<const char> fontFile,
                const float dpi
        ) {
            /* Load a TrueTypeFont plugin and open the font */

            Utility::Resource rs(resource.data());
            font[0] = manager->get()->loadAndInstantiate(fontPlugin.data());
            openData_size = dpi * 2;
            if (!font[0] || !font[0]->openData(rs.getRaw(fontFile.data()), openData_size))
                LOG_MAGNUM_FATAL << "Cannot open font file";

            /* Prepare glyph cache */
            font[0]->fillGlyphCache(**cache,
                                    "abcdefghijklmnopqrstuvwxyz"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "0123456789 _.,-+=*:;?!@$&#/"
                                    "\\|`\"'<>()[]{}%…"
            );
        }

        template<class T> void deleteContainer(T & container) {
            if (container != nullptr) {
                if (container->get() != nullptr)
                    delete container->get();
                container->release();
                delete container;
            }
        }

        void
        BasicFont::draw(const Containers::ArrayView<const char> str, float size, float x, float y,
                        Text::Alignment alignment) {
            float pt = size;
            float fontSize = pt * 1.3333f;
            float render2D_size = fontSize / openData_size;
            deleteContainer(text);
            text = new Containers::Pointer<Text::Renderer2D>{
                    new Text::Renderer2D(**font, **cache, render2D_size, alignment)
            };
            text->get()->reserve(str.size(), GL::BufferUsage::DynamicDraw,
                                 GL::BufferUsage::StaticDraw);

            /* Draw the text on the screen */
            shader->setColor({1.0f, 1.0f, 1.0f, 1.0f});
            shader->bindVectorTexture(cache->get()->texture());
            text->get()->render(str.data());

            GL::DefaultFramebuffer &fb = GL::defaultFramebuffer;
            auto viewport = fb.viewport();
            auto viewportSize = viewport.size();
            auto vector2 = Vector2(viewportSize);
            auto aspectRatio = vector2.aspectRatio();
            auto yScale = Vector2::yScale(aspectRatio);
            auto viewportScaling = Matrix3::scaling(yScale);
            // set text position to -1.0f, 1.0f, top left
            auto translation = Matrix3::translation({x, y});
            auto matrix = translation * viewportScaling;
            shader->setTransformationProjectionMatrix(matrix);
            shader->draw(text->get()->mesh());
        }

        void BasicFont::release() {
            delete shader;
            deleteContainer(this->manager);
            deleteContainer(this->cache);
            deleteContainer(this->font);
            deleteContainer(this->text);
        }
    }
}

grid grid_10x10, dnc_x2, dnc_x3;
Font::BasicFont _font;

GLIS_CALLBACKS_DRAW(draw, glis, renderer, font, fps) {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);
    grid_10x10.draw();
    #define draw_coordinateX(x, alignment) _font.draw(#x, 5, x, 1.0f, alignment)
    draw_coordinateX(-1.0f, Text::Alignment::TopLeft);
    draw_coordinateX(-0.9f, Text::Alignment::TopLeft);
    draw_coordinateX(-0.8f, Text::Alignment::TopLeft);
    draw_coordinateX(-0.7f, Text::Alignment::TopLeft);
    draw_coordinateX(-0.6f, Text::Alignment::TopLeft);
    draw_coordinateX(-0.5f, Text::Alignment::TopLeft);
    draw_coordinateX(-0.4f, Text::Alignment::TopLeft);
    draw_coordinateX(-0.3f, Text::Alignment::TopLeft);
    draw_coordinateX(-0.2f, Text::Alignment::TopLeft);
    draw_coordinateX(-0.1f, Text::Alignment::TopLeft);
    draw_coordinateX(0.1f, Text::Alignment::TopRight);
    draw_coordinateX(0.2f, Text::Alignment::TopRight);
    draw_coordinateX(0.3f, Text::Alignment::TopRight);
    draw_coordinateX(0.4f, Text::Alignment::TopRight);
    draw_coordinateX(0.5f, Text::Alignment::TopRight);
    draw_coordinateX(0.6f, Text::Alignment::TopRight);
    draw_coordinateX(0.7f, Text::Alignment::TopRight);
    draw_coordinateX(0.8f, Text::Alignment::TopRight);
    draw_coordinateX(0.9f, Text::Alignment::TopRight);
    draw_coordinateX(1.0f, Text::Alignment::TopRight);
    #define draw_coordinateY(y, alignment) _font.draw(#y, 5, 1.0f, y+0.01f, alignment)
    draw_coordinateY(0.9f, Text::Alignment::LineRight);
    draw_coordinateY(0.8f, Text::Alignment::LineRight);
    draw_coordinateY(0.7f, Text::Alignment::LineRight);
    draw_coordinateY(0.6f, Text::Alignment::LineRight);
    draw_coordinateY(0.5f, Text::Alignment::LineRight);
    draw_coordinateY(0.4f, Text::Alignment::LineRight);
    draw_coordinateY(0.3f, Text::Alignment::LineRight);
    draw_coordinateY(0.2f, Text::Alignment::LineRight);
    draw_coordinateY(0.1f, Text::Alignment::LineRight);
    draw_coordinateY(0.0f, Text::Alignment::LineRight);
    draw_coordinateY(-0.1f, Text::Alignment::LineRight);
    draw_coordinateY(-0.2f, Text::Alignment::LineRight);
    draw_coordinateY(-0.3f, Text::Alignment::LineRight);
    draw_coordinateY(-0.4f, Text::Alignment::LineRight);
    draw_coordinateY(-0.5f, Text::Alignment::LineRight);
    draw_coordinateY(-0.6f, Text::Alignment::LineRight);
    draw_coordinateY(-0.7f, Text::Alignment::LineRight);
    draw_coordinateY(-0.8f, Text::Alignment::LineRight);
    draw_coordinateY(-0.9f, Text::Alignment::LineRight);
    draw_coordinateY(-1.0f, Text::Alignment::LineRight);
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_RESIZE(resize, glis, renderer, font, fps, width, height) {
    glViewport(0, 0, width, height);
}

GLIS_CALLBACKS_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(screen);
    // order does not matter
    grid_10x10.release();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 800, 800);
    glis.GLIS_setupOnScreenRendering(screen);
    screen.contextMagnum.create();
//    GL::DebugOutput::Callback on_gl_errorMagnum = NULL;
//    GL::DebugOutput::setCallback(on_gl_errorMagnum, nullptr);

    _font.create();
    _font.load("fonts", "FreeTypeFont", "Vera.ttf", 96.0f);
    grid_10x10.create();
    grid_10x10.setSize(20, 20);
    grid_10x10.setColor({1.0f, 0.0f, 0.0f, 1.0f});
    grid_10x10.createMesh();
    grid_10x10.drawLabels = true;

//    dnc_x2.create();
//    dnc_x2.setSize(20, 20);
//    dnc_x2.setColor({0.0f, 1.0f, 0.0f, 1.0f});
//    dnc_x2.createMesh();
//
//    dnc_x3.create();
//    dnc_x3.setSize(40, 40);
//    dnc_x3.setColor({0.0f, 0.0f, 1.0f, 1.0f});
//    dnc_x3.createMesh();

    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}