//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>

GLIS_CLASS CompositorMain;
GLIS glis;

void draw() {
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glis.GLIS_SwapBuffers(CompositorMain);
}

void close() {
    glis.destroyX11Window(CompositorMain);
    glis.GLIS_destroy_GLIS(CompositorMain);
}

int main() {
    glis.getX11Window(CompositorMain, 400, 400);
    glis.GLIS_setupOnScreenRendering(CompositorMain);
    glis.runUntilX11WindowClose(CompositorMain, draw, nullptr, close);
}