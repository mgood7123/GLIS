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

namespace Magnum {
    namespace Font {
        class BasicFont {
        public:
            PluginManager::Manager<Text::AbstractFont> *manager = nullptr;
            Utility::Resource * resource = nullptr;

            class Instance {
            public:
                Shaders::Vector2D *shader = nullptr;
                Text::Renderer2D *text = nullptr;
                Text::GlyphCache *cache = nullptr;
                Containers::Pointer <Text::AbstractFont> * font;
                float openData_size = 0;

                void create();

                void draw(std::string str, float size, float x, float y,
                          Text::Alignment alignment);

                void draw(const char * str, float size, float x, float y,
                          Text::Alignment alignment);

                void draw(const Containers::ArrayView<const char> str, float size, float x, float y,
                          Text::Alignment alignment);

                void release();
            };

            void create();

            void load(
                    const Containers::ArrayView<const char> resource,
                    const Containers::ArrayView<const char> fontPlugin
            );

            void unload(const Containers::ArrayView<const char> fontPlugin);

            Instance newInstance(const Containers::ArrayView<const char> fontPlugin,
                                 const Containers::ArrayView<const char> fontFile,
                                 const float dpi
            );

            void release();
        };

        void BasicFont::create() {
            manager = new PluginManager::Manager<Text::AbstractFont>;
        }

        void BasicFont::load(
                const Containers::ArrayView<const char> resource,
                const Containers::ArrayView<const char> fontPlugin
        ) {
            this->resource = new Utility::Resource(resource.data());
            // for some reason, CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT is set to 1 on
            // android, even though android supports shared object files (.so)
            manager->load(fontPlugin.data());
        }

        void BasicFont::unload(const Containers::ArrayView<const char> fontPlugin) {
            manager->unload(fontPlugin.data());
        }

        void BasicFont::release() {
            delete manager;
            delete resource;
        }

        BasicFont::Instance BasicFont::newInstance(
                const Containers::ArrayView<const char> fontPlugin,
                const Containers::ArrayView<const char> fontFile,
                const float dpi
        ) {
            Instance x;
            x.create();
            x.font = new Containers::Pointer<Text::AbstractFont>;
            x.font[0] = manager->instantiate(fontPlugin.data());
            x.openData_size = dpi * 2;
            if (!x.font[0] || !x.font[0]->openData(resource->getRaw(fontFile.data()), x.openData_size))
                LOG_MAGNUM_FATAL << "Cannot open font file";

            x.font[0]->fillGlyphCache(*x.cache,
                                      "abcdefghijklmnopqrstuvwxyz"
                                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "0123456789 _.,-+=*:;?!@$&#/"
                                      "\\|`\"'<>()[]{}%…"
            );
            return x;
        }

        void BasicFont::Instance::create() {
            shader = new Shaders::Vector2D;
            cache = new Text::GlyphCache{Vector2i{4096}};
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
        BasicFont::Instance::draw(std::string str, float size, float x, float y,
                        Text::Alignment alignment) {
            draw(str.c_str(), size, x, y, alignment);
        }

        void
        BasicFont::Instance::draw(const char * str, float size, float x, float y,
                        Text::Alignment alignment) {
            Containers::ArrayView<const char> s(str, strlen(str));
            draw(s, size, x, y, alignment);
        }

        void
        BasicFont::Instance::draw(const Containers::ArrayView<const char> str, float size, float x, float y,
                        Text::Alignment alignment) {
            float pt = size;
            float fontSize = pt * 1.3333f;
            float render2D_size = fontSize / openData_size;
            if (text != nullptr) delete text;
            text = new Text::Renderer2D(**font, *cache, render2D_size, alignment);
            text->reserve(str.size(), GL::BufferUsage::DynamicDraw,
                          GL::BufferUsage::StaticDraw);

            /* Draw the text on the screen */
            shader->setColor({1.0f, 1.0f, 1.0f, 1.0f});
            shader->bindVectorTexture(cache->texture());
            text->render(str.data());

            GL::DefaultFramebuffer &fb = GL::defaultFramebuffer;
            auto viewport = fb.viewport();
            auto viewportSize = viewport.size();
            auto vector2 = Vector2(viewportSize);
            auto aspectRatio = vector2.aspectRatio();
            auto yScale = Vector2::yScale(aspectRatio);
            auto viewportScaling = Matrix3::scaling(yScale);
            auto translation = Matrix3::translation({x, y});
            auto matrix = translation * viewportScaling;
            shader->setTransformationProjectionMatrix(matrix);
            shader->draw(text->mesh());
        }

        void BasicFont::Instance::release() {
            delete shader;
            if (font != nullptr)
                if (font->get() != nullptr)
                    font->get()->close();
            deleteContainer(this->font);
            delete cache;
            delete text;
        }
    }
    namespace Shapes {
        void drawGrid(const Color4 &color, int columns, int rows) {
            Shaders::Flat2D shader;
            shader.setColor(color);
            GL::Mesh mesh;
            Vector2i subDivisions = {columns-1, rows-1};
            mesh = MeshTools::compile(Primitives::grid3DWireframe(subDivisions));
            shader.draw(mesh);
        }

        void drawLine(const Color4 &color, float startX, float startY, float endX, float endY) {
            GL::Mesh mesh;
            Shaders::Flat2D shader;
            shader.setColor(color);
            mesh = MeshTools::compile(Primitives::line2D({startX, startY}, {endX, endY}));
            shader.draw(mesh);
        }
        void drawLineX(const Color4 &color, float y, float startX, float endX) {
            drawLine(color, startX, y, endX, y);
        }

        void drawLineY(const Color4 &color, float x, float startY, float endY) {
            drawLine(color, x, startY, x, endY);
        }

        void drawDNC(Color4 color, float ratio) {
            float s;
            s= -1.0f;
            while(true) {
                drawLineX(color, s, 1.0f, -1.0f);
                if (s >= 0.0f) s += ratio;
                else s -= -ratio;
                if (s >= 1.0f) break;
            }

            s = -1.0f;
            while(true) {
                drawLineY(color, s, 1.0f, -1.0f);
                if (s >= 0.0f) s += ratio;
                else s -= -ratio;
                if (s >= 1.0f) break;
            }
        }

        void drawDeviceNormalizedCoordinateGrid_Ratio_ZeroPointOne(Font::BasicFont::Instance & font) {
            drawDNC({0.0f, 1.0f, 0.0f, 1.0f}, 0.1f);

            // draw co-ordinates

#define draw_coordinateX(x, alignment) font.draw(#x, 5, x, 1.0f, alignment)
#define draw_coordinateY(y, alignment) font.draw(#y, 5, 1.0f, y+0.01f, alignment)

            draw_coordinateX(-1.0F, Text::Alignment::TopLeft);
            draw_coordinateX(-0.9F, Text::Alignment::TopLeft);
            draw_coordinateX(-0.8F, Text::Alignment::TopLeft);
            draw_coordinateX(-0.7F, Text::Alignment::TopLeft);
            draw_coordinateX(-0.6F, Text::Alignment::TopLeft);
            draw_coordinateX(-0.5F, Text::Alignment::TopLeft);
            draw_coordinateX(-0.4F, Text::Alignment::TopLeft);
            draw_coordinateX(-0.3F, Text::Alignment::TopLeft);
            draw_coordinateX(-0.2F, Text::Alignment::TopLeft);
            draw_coordinateX(-0.1F, Text::Alignment::TopLeft);
            draw_coordinateX( 0.1F, Text::Alignment::TopRight);
            draw_coordinateX( 0.2F, Text::Alignment::TopRight);
            draw_coordinateX( 0.3F, Text::Alignment::TopRight);
            draw_coordinateX( 0.4F, Text::Alignment::TopRight);
            draw_coordinateX( 0.5F, Text::Alignment::TopRight);
            draw_coordinateX( 0.6F, Text::Alignment::TopRight);
            draw_coordinateX( 0.7F, Text::Alignment::TopRight);
            draw_coordinateX( 0.8F, Text::Alignment::TopRight);
            draw_coordinateX( 0.9F, Text::Alignment::TopRight);
            draw_coordinateX( 1.0F, Text::Alignment::TopRight);
            draw_coordinateY( 0.9F, Text::Alignment::LineRight);
            draw_coordinateY( 0.8F, Text::Alignment::LineRight);
            draw_coordinateY( 0.7F, Text::Alignment::LineRight);
            draw_coordinateY( 0.6F, Text::Alignment::LineRight);
            draw_coordinateY( 0.5F, Text::Alignment::LineRight);
            draw_coordinateY( 0.4F, Text::Alignment::LineRight);
            draw_coordinateY( 0.3F, Text::Alignment::LineRight);
            draw_coordinateY( 0.2F, Text::Alignment::LineRight);
            draw_coordinateY( 0.1F, Text::Alignment::LineRight);
            draw_coordinateY( 0.0F, Text::Alignment::LineRight);
            draw_coordinateY(-0.1F, Text::Alignment::LineRight);
            draw_coordinateY(-0.2F, Text::Alignment::LineRight);
            draw_coordinateY(-0.3F, Text::Alignment::LineRight);
            draw_coordinateY(-0.4F, Text::Alignment::LineRight);
            draw_coordinateY(-0.5F, Text::Alignment::LineRight);
            draw_coordinateY(-0.6F, Text::Alignment::LineRight);
            draw_coordinateY(-0.7F, Text::Alignment::LineRight);
            draw_coordinateY(-0.8F, Text::Alignment::LineRight);
            draw_coordinateY(-0.9F, Text::Alignment::LineRight);
            draw_coordinateY(-1.0F, Text::Alignment::LineRight);
        }
        void drawDeviceNormalizedCoordinateGrid_Ratio_ZeroPointZeroOne(Font::BasicFont::Instance & font) {
            drawDNC({1.0f, 0.0f, 0.0f, 1.0f}, 0.01f);

        }
    }
}

Magnum::Font::BasicFont basicFont;
Magnum::Font::BasicFont::Instance fontA, fontB;

GLIS_CALLBACKS_DRAW(draw, glis, renderer, font, fps) {
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color|Magnum::GL::FramebufferClear::Depth);
    Magnum::Shapes::drawDeviceNormalizedCoordinateGrid_Ratio_ZeroPointZeroOne(fontA);
    Magnum::Shapes::drawDeviceNormalizedCoordinateGrid_Ratio_ZeroPointOne(fontA);
    fontB.draw("A", 1, 0.0f, 0.0f, Magnum::Text::Alignment::TopLeft);
    {
        auto rect = fontB.text->rectangle();
        int rangeDimensions = 2;
        std::string r;
        r = "Range({";;
        auto min = rect.min();
        r += std::to_string(min[0]);
        for (Magnum::UnsignedInt i = 1; i != rangeDimensions; ++i) {
            r += ",";
            r += std::to_string(min[i]);
        }
        r += "}, {";;
        auto max = rect.max();
        r += std::to_string(max[0]);
        for (Magnum::UnsignedInt i = 1; i != rangeDimensions; ++i) {
            r += ",";
            r += std::to_string(max[i]);
        }
        r += "})";
        fontA.draw(r, 10, -1.0f, -0.4f, Magnum::Text::Alignment::TopLeft);
    }
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_RESIZE(resize, glis, renderer, font, fps, width, height) {
    glViewport(0, 0, width, height);
}

GLIS_CALLBACKS_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(screen);
    fontB.release();
    fontA.release();
    basicFont.unload("FreeTypeFont");
    basicFont.release();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 800, 600);
    glis.GLIS_setupOnScreenRendering(screen);
    screen.contextMagnum.create();
//    GL::DebugOutput::Callback on_gl_errorMagnum = NULL;
//    GL::DebugOutput::setCallback(on_gl_errorMagnum, nullptr);

    basicFont.create();
    basicFont.load("fonts", "FreeTypeFont");
    fontA = basicFont.newInstance("FreeTypeFont", "Vera.ttf", 96.0f);
    fontB = basicFont.newInstance("FreeTypeFont", "Vera.ttf", 10.0f);
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}