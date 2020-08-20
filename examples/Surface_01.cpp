//
// Created by smallville7123 on 19/07/20.
//

#include <glis/glis.hpp>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

GLIS_Surface surfaceMain;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, screen, font, fps) {
    surfaceMain.bindAndClear();
    surfaceMain.drawTriangleWireframe({1.0f, 0.0f,  0.0f,  1.0f});
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, screen, font, fps) {
    surfaceMain.resize({screen.width, screen.height});
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, screen, font, fps) {
    glis.destroyX11Window(screen);
    surfaceMain.release();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    if (glis.getX11Window(screen, 400, 400)) {
        glis.GLIS_setupOnScreenRendering(screen);
        screen.contextMagnum.create();
        // improves screen resizing
        glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
    }
}
