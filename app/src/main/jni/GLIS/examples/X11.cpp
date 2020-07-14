//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>

void noop() {}

int main() {
    GLIS_CLASS CompositorMain;
    GLIS glis;
    glis.getX11Window(CompositorMain, 400, 400);
    glis.GLIS_setupOnScreenRendering(CompositorMain);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glis.GLIS_SwapBuffers(CompositorMain);
    glis.runUntilX11WindowClose(CompositorMain, noop);
    glis.destroyX11Window(CompositorMain);
    glis.GLIS_destroy_GLIS(CompositorMain);
}