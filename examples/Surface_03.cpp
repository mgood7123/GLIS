//
// Created by smallville7123 on 19/07/20.
//

#include <glis/glis.hpp>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

GLIS_Surface surfaceMain;
GLIS_Surface surfaceTemporary;
GLIS_Surface surfaceTemporary1;

GLIS_SurfaceColor TORQUISE {0.0f, 1.0f,  1.0f,  1.0f};
GLIS_SurfaceColor PURPLE {0.4f, 0.0f,  0.4f,  1.0f};
GLIS_SurfaceColor BLUE {0.0f, 0.0f,  0.4f,  1.0f};

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, screen, font, fps) {
    surfaceTemporary1.bindAndClear();
    surfaceTemporary1.drawPlaneCorners(BLUE, {0,0}, {-1,-1});
    surfaceTemporary1.drawPlaneCorners(PURPLE, {0,0}, {-1,1});
    surfaceTemporary1.drawPlaneCorners(PURPLE, {0,0}, {1,-1});
    surfaceTemporary1.drawPlaneCorners(BLUE, {0,0}, {1,1});

    surfaceTemporary1.drawPlaneWireframeCorners(TORQUISE, {0,0}, {-1,-1});
    surfaceTemporary1.drawPlaneWireframeCorners(TORQUISE, {0,0}, {-1,1});
    surfaceTemporary1.drawPlaneWireframeCorners(TORQUISE, {0,0}, {1,-1});
    surfaceTemporary1.drawPlaneWireframeCorners(TORQUISE, {0,0}, {1,1});

    surfaceTemporary.bindAndClear();
    surfaceTemporary.drawPlaneCorners(surfaceTemporary1, {0,0}, {-1,-1});
    surfaceTemporary.drawPlaneCorners(surfaceTemporary1, {0,0}, {-1,1});
    surfaceTemporary.drawPlaneCorners(surfaceTemporary1, {0,0}, {1,-1});
    surfaceTemporary.drawPlaneCorners(surfaceTemporary1, {0,0}, {1,1});

    surfaceMain.bindAndClear();
    surfaceMain.drawTriangle(surfaceTemporary);

    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, screen, font, fps) {
    surfaceTemporary1.resize({screen.width, screen.height});
    surfaceTemporary.resize({screen.width, screen.height});
    surfaceMain.resize({screen.width, screen.height});
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, screen, font, fps) {
    glis.destroyX11Window(screen);
    surfaceTemporary1.release();
    surfaceTemporary.release();
    surfaceMain.release();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    if (glis.getX11Window(screen, 400, 400)) {
        glis.GLIS_setupOnScreenRendering(screen);
        screen.contextMagnum.create();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        surfaceTemporary.newFramebuffer({screen.width, screen.height});
        surfaceTemporary1.newFramebuffer({screen.width, screen.height});
        glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
    }
}