// LINUX

#include <glis/glis.hpp>

int main() {
    // print non errors
    GLIS_LOG_PRINT_NON_ERRORS = true;
    GLIS glis;
    GLIS_CLASS glis_class;

//    // create a new X11 window
//    if (!glis.getX11Window(glis_class, 400, 400)) return -1;
    // create a new Wayland window
    if (!glis.getWaylandWindow(glis_class, 400, 400)) return -1;

    // initiate opengl
    glis.GLIS_setupOnScreenRendering(glis_class);
    glis.GLIS_error_to_string_GL();

    // draw a pink background
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glis.GLIS_error_to_string_GL("glClearColor");
    glClear(GL_COLOR_BUFFER_BIT);
    glis.GLIS_error_to_string_GL("glClear");

    // swap buffers
    glis.GLIS_SwapBuffers(glis_class);
    glis.GLIS_error_to_string_EGL("SwapBuffers");

    while (glis.waylandDispatch(glis_class) != -1 && glis.waylandIsRunning());

    // sleep to give time for window to appear
//    sleep(2);

    // clean up everything
    glis.GLIS_destroy_GLIS(glis_class);
//    glis.destroyX11Window(glis_class);
    glis.destroyWaylandWindow(glis_class);
    return 0;
}