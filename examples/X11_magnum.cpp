//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>

GLIS_CLASS CompositorMain;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

GLIS_CALLBACKS_DRAW(draw, glis, renderer, font, fps) {
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glis.GLIS_SwapBuffers(CompositorMain);
}

GLIS_CALLBACKS_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(CompositorMain);
    glis.GLIS_destroy_GLIS(CompositorMain);
}

int main() {
    glis.getX11Window(CompositorMain, 400, 400);
    glis.GLIS_setupOnScreenRendering(CompositorMain);
    eglSwapInterval(CompositorMain.display, 0);
    glis.runUntilX11WindowClose(glis, CompositorMain, font, fps, draw, nullptr, close);
}