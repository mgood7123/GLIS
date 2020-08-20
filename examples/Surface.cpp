//
// Created by smallville7123 on 19/07/20.
//

#include <glis/glis.hpp>
#include "../src/glis/surface/surface.cpp"
#include <Magnum/Primitives/Plane.h>
#include <Magnum/Shaders/MeshVisualizer.h>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

GLIS_Surface surfaceMain;
GLIS_Surface surfaceTemporary;
GLIS_Surface surfaceTemporary2;
GLIS_Surface surfaceTemporary3;
GLIS_Surface surfaceTemporary4;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, screen, font, fps) {
    surfaceTemporary3.clear();
    surfaceTemporary3.bind();
    surfaceTemporary3.drawTriangle();
    surfaceTemporary3.drawPlaneWireframe({1.0f, 0.0f,  0.0f,  1.0f});
    
    surfaceTemporary2.clear();
    surfaceTemporary3.bind();
    surfaceTemporary2.drawPlane(surfaceTemporary3);
    surfaceTemporary2.drawPlaneWireframe({1.0f, 0.0f,  0.0f,  1.0f});
    
    surfaceTemporary.clear();
    surfaceTemporary.bind();
    surfaceTemporary.drawTriangle(surfaceTemporary2);
    surfaceTemporary.drawPlaneWireframe({1.0f, 0.0f,  0.0f,  1.0f});
    
    surfaceMain.clear();
    surfaceMain.bind();
    surfaceMain.drawPlane(surfaceTemporary);
    surfaceMain.drawPlaneWireframe({1.0f, 0.0f,  0.0f,  1.0f});
    
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, screen, font, fps) {
    surfaceTemporary4.resize({screen.width, screen.height});
    surfaceTemporary3.resize({screen.width, screen.height});
    surfaceTemporary2.resize({screen.width, screen.height});
    surfaceTemporary.resize({screen.width, screen.height});
    surfaceMain.resize({screen.width, screen.height});
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, screen, font, fps) {
    glis.destroyX11Window(screen);
    surfaceTemporary4.release();
    surfaceTemporary3.release();
    surfaceTemporary2.release();
    surfaceTemporary.release();
    surfaceMain.release();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    if (glis.getX11Window(screen, 400, 400)) {
        glis.GLIS_setupOnScreenRendering(screen);
        screen.contextMagnum.create();
        // improves screen resizing
        glis.vsync(screen, 0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        surfaceTemporary.newFramebuffer({screen.width, screen.height});
        surfaceTemporary2.newFramebuffer({screen.width, screen.height});
        surfaceTemporary3.newFramebuffer({screen.width, screen.height});
        surfaceTemporary4.newFramebuffer({screen.width, screen.height});
        glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
    }
}
