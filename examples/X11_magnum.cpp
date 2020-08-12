//
// Created by smallville7123 on 19/07/20.
//

#include <glis/glis.hpp>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Primitives/Plane.h>

#include <Magnum/GL/Texture.h>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

using namespace Magnum;

void setTextureData(GL::Texture2D & texture, int texture_width, int texture_height, void * data) {

    // is this safe to be function scoped?
    ImageView2D image(PixelFormat::RGBA8Unorm, {texture_width, texture_height}, data);
    
    // do mipmaps need to be regenerated if the data changes?
    texture.setSubImage(0, {}, image).generateMipmap();
}

GL::Texture2D & genTextureFromGLTexture() {
    GL::Texture2D texture;
    texture.wrap(GLuint id);
    // would anything else need to be done?
    return texture;
}

GL::Texture2D & genTextureFromData(int texture_width, int texture_height, void * data) {

    // is this safe to be function scoped?
    ImageView2D image(PixelFormat::RGBA8Unorm, {texture_width, texture_height}, data);

    GL::Texture2D texture;
    texture.setMagnificationFilter(GL::SamplerFilter::Linear)
        .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
        .setStorage(Math::log2(4096)+1, GL::TextureFormat::RGBA8, {texture_width, texture_height})
        .setSubImage(0, {}, image)
        .generateMipmap();
    return texture;
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);
    Shaders::Flat2D shader;
    shader.setColor({1.0f, 0.0f, 0.0f, 1.0f});
    shader.draw(MeshTools::compile(Primitives::planeSolid()));
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glis.GLIS_Viewport(renderer);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(screen);
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 400, 400);
    glis.GLIS_setupOnScreenRendering(screen);
    screen.contextMagnum.create();
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
