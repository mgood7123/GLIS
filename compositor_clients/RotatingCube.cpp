//
// Created by konek on 8/18/2019.
//

#include <glis/glis.hpp>

GLIS_CLASS G;

GLIS glis;

using namespace Magnum;
using namespace Magnum::Math::Literals;

GL::Mesh * _mesh = nullptr;
Shaders::Phong * _shader = nullptr;
GL::Framebuffer * framebuffer = nullptr;
GL::Renderbuffer * depthStencil = nullptr;
GL::Texture2D * color = nullptr;
Matrix4 _transformation, _projection;
Color3 _color;

int main() {
    int W = GLIS_COMMON_WIDTH;
    int H = GLIS_COMMON_HEIGHT;
    if (glis.GLIS_setupOffScreenRendering(G, W, H)) {

        G.contextMagnum.create();

        Vector2i size = {W, H};

        framebuffer = new GL::Framebuffer {{{}, size}};
        depthStencil = new GL::Renderbuffer;
        color = new GL::Texture2D;
        _shader = new Shaders::Phong;
        _mesh = new GL::Mesh;

        color->setStorage(1, GL::TextureFormat::RGBA8, size);
        depthStencil->setStorage(GL::RenderbufferFormat::Depth24Stencil8, size);

        framebuffer->attachTexture(GL::Framebuffer::ColorAttachment{0}, *color, 0);
        framebuffer->attachRenderbuffer(GL::Framebuffer::BufferAttachment::DepthStencil, *depthStencil);

        framebuffer->bind();

        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

        *_mesh = MeshTools::compile(Primitives::cubeSolid() );
        _transformation = Matrix4::rotationX(0.0_degf)*Matrix4::rotationY(0.0_degf);
        _projection = Matrix4::perspectiveProjection(35.0_degf, Vector2{static_cast<float>(W*H)}.aspectRatio(), 0.01f, 100.0f)*Matrix4::translation(Vector3::zAxis(-10.0f));
        _color = Color3::fromHsv({35.0_degf, 1.0f, 1.0f});

        LOG_INFO("creating window %d", 0);
        size_t win_id1 = glis.GLIS_new_window(0, 0, W, H);
        LOG_INFO("window id: %zu", win_id1);
        LOG_INFO("created window %d", 0);
        SERVER_LOG_TRANSFER_INFO = true;

        while(true) {
//            GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);
            framebuffer->clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);
            _transformation =
                    _transformation * Matrix4::rotationX(1.0_degf) * Matrix4::rotationY(1.0_degf);
            _shader->setLightPosition({7.0f, 5.0f, 2.5f})
                    .setLightColor(Color3{1.0f})
                    .setDiffuseColor(_color)
                    .setAmbientColor(Color3::fromHsv({_color.hue(), 1.0f, 0.3f}))
                    .setTransformationMatrix(_transformation)
                    .setNormalMatrix(_transformation.normalMatrix())
                    .setProjectionMatrix(_projection)
                    .draw(*_mesh);
            glis.GLIS_upload_texture(G, win_id1, W, H);
        }

        LOG_INFO("Cleaning up");
        delete _shader;
        delete _mesh;
        delete color;
        delete depthStencil;
        delete framebuffer;
        glis.GLIS_destroy_GLIS(G);
        LOG_INFO("Destroyed sub Compositor GLIS");
        LOG_INFO("Cleaned up");
    }
    return 0;
}