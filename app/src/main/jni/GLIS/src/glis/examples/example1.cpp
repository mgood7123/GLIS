// LINUX

#include <glis/glis.hpp>
#include  <X11/Xlib.h>

int main() {
    GLIS glis;
    GLIS_CLASS glis_class;

    // create a new X11 window
    Display * x_display = XOpenDisplay(nullptr);
    if (x_display == nullptr) LOG_ALWAYS_FATAL("error, cannot connect to X server");
    Window win = XCreateSimpleWindow(x_display, DefaultRootWindow(x_display), 0, 0, 800, 480, 0, 0, 0);
    XMapWindow(x_display, win);
    XStoreName(x_display, win, "Compositor");

    // override default values
    glis_class.native_window = win;
    glis_class.display_id = x_display;

    // initiate opengl
    glis.GLIS_setupOnScreenRendering(glis_class);

    // draw a pink background
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // swap buffers
    glis.GLIS_SwapBuffers(glis_class);

    // sleep to give time for window to appear
    sleep(2);

    // clean up everything
    glis.GLIS_destroy_GLIS(glis_class);
    XDestroyWindow(x_display, win);
    XCloseDisplay(x_display);
    return 0;
}