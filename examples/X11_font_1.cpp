//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

GLIS_FONT::ATLAS_TYPE * quad_cache = nullptr;
GLIS_FONT::font_init * fontInit_cache = nullptr;
GLIS_FONT::font_data * fontData_cache = nullptr;
GLIS_FONT::atlas * a12_cache = nullptr;
GLIS_FONT::atlas * a24_cache = nullptr;
GLIS_FONT::atlas * a48_cache = nullptr;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    // initialize cache
    if (quad_cache == nullptr) quad_cache = font.get_atlas("id");
    if (fontInit_cache == nullptr) fontInit_cache = quad_cache->third.get<GLIS_FONT::font_init*>();
    if (fontData_cache == nullptr) fontData_cache = quad_cache->fourth.get<GLIS_FONT::font_data*>();
    if (a12_cache == nullptr) a12_cache = font.find_size("id", fontData_cache, 12);
    if (a24_cache == nullptr) a24_cache = font.find_size("id", fontData_cache, 24);
    if (a48_cache == nullptr) a48_cache = font.find_size("id", fontData_cache, 48);
    // cache initialized

    /* White background */
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Set color to black */
    font.set_color(font.colors.black);

    /* Effects of alignment */
    font.render_text("The Quick Brown Fox Jumps Over The Lazy Dog", a48_cache, 8, 50);
    font.render_text("The Misaligned Fox Jumps Over The Lazy Dog", a48_cache, 8.5, 100.5);

    /* Scaling the texture versus changing the font size */
    font.render_text("The Small Texture Scaled Fox Jumps Over The Lazy Dog", a48_cache, 8, 175, 0.5f, 0.5f);
    font.render_text("The Small Font Sized Fox Jumps Over The Lazy Dog", a24_cache, 8, 200);
    font.render_text("The Tiny Texture Scaled Fox Jumps Over The Lazy Dog", a48_cache, 8, 235, 0.25, 0.25);
    font.render_text("The Tiny Font Sized Fox Jumps Over The Lazy Dog", a12_cache, 8, 250);

    /* Colors and transparency */
    font.render_text("The Solid Black Fox Jumps Over The Lazy Dog", a48_cache, 8, 430);

    font.set_color(font.colors.red);
    font.render_text("The Solid Red Fox Jumps Over The Lazy Dog", a48_cache, 8, 330);
    font.render_text("The Solid Red Fox Jumps Over The Lazy Dog", a48_cache, 28, 450);

    font.set_color(font.colors.transparent_green);
    font.render_text("The Transparent Green Fox Jumps Over The Lazy Dog", a48_cache, 0, 380);
    font.render_text("The Transparent Green Fox Jumps Over The Lazy Dog", a48_cache, 18, 440);
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glis.GLIS_Viewport(renderer);
    font.set_max_width_height(renderer);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(screen);
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 640, 480);
    glis.GLIS_setupOnScreenRendering(screen);
    glis.GLIS_Viewport(screen);
    const char * f =
            "/home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/Vera.ttf";
    font.set_max_width_height(screen);

    // take advantage of return values to cache results

    quad_cache = font.add_font("id", f);
    a12_cache = font.add_font_size("id", 12);
    a24_cache = font.add_font_size("id", 24);
    a48_cache = font.add_font_size("id", 48);

    glis.GLIS_SwapBuffers(screen);
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}