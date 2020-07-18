// LINUX

#include <glis/glis.hpp>

GLIS_CLASS CompositorMain;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

GLIS_COMPOSITOR_LOOP_FUNCTION(loop, glis, CompositorMain, font, fps, render, resize, close_) {
    glis.runUntilX11WindowClose(glis, CompositorMain, font, fps, render, resize, close_);
}

int main() {
    // print non errors
    if (!glis.getX11Window(CompositorMain, GLIS_COMMON_WIDTH/4, GLIS_COMMON_HEIGHT/4)) return -1;
    CompositorMain.width = GLIS_COMMON_WIDTH;
    CompositorMain.height = GLIS_COMMON_HEIGHT;

    // thread 1
    GLIS_COMPOSITOR_BEFORE_REQUEST_STARTUP(glis);

    // main
    GLIS_COMPOSITOR_REQUEST_STARTUP(glis);

    // thread 1
    GLIS_COMPOSITOR_HANDLE_STARTUP_REQUEST(glis);
    GLIS_COMPOSITOR_DO_MAIN(
            glis,
            CompositorMain,
            fps,
            font,
            "/home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/Vera.ttf",
            12,
            loop,
            GLIS_COMPOSITOR_DEFAULT_DRAW_FUNCTION,
            GLIS_COMPOSITOR_DEFAULT_RESIZE_FUNCTION,
            GLIS_COMPOSITOR_DEFAULT_CLOSE_FUNCTION
    );
    return 0;
}