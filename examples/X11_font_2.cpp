//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    /* White background */
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    font.render_text("This is Vera font at size 24", "Vera", 24, 0, 24);
    font.render_text("This is Fireflysung font at size 44", "Fireflysung", 44, 0, 24+44);

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
    glis.getX11Window(screen, 680, 480);
    glis.GLIS_setupOnScreenRendering(screen);
    glis.GLIS_Viewport(screen);
    font.set_max_width_height(screen);

    font.add_font("Vera", "/home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/Vera.ttf");
    font.add_font_size("Vera", 24);
    font.add_font("Fireflysung", "/home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/fireflysung.ttf");
    font.add_font_size("Fireflysung", 44);

    font.set_color(font.colors.white);

    glis.GLIS_SwapBuffers(screen);
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
