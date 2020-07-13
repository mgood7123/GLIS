// LINUX

#include <glis/glis.hpp>
#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

int main() {
    GLIS glis;
    GLIS_CLASS glis_class;
    // we need to obtain a native window

    // open the standard display (the primary screen)
    Display    *x_display;
    Window      win;

    x_display = XOpenDisplay ( NULL );   // open the standard display (the primary screen)
    if ( x_display == NULL ) {
        LOG_ERROR("error, cannot connect to X server");
        return 1;
    }

    Window root  =  DefaultRootWindow( x_display );   // get the root window (usually the whole screen)

    XSetWindowAttributes  swa;
    swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;

    win  =  XCreateWindow (   // create a window with the provided parameters
            x_display, root,
            0, 0, 800, 480,   0,
            CopyFromParent, InputOutput,
            CopyFromParent, CWEventMask,
            &swa );

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    XMapWindow ( x_display , win );             // make the window visible on the screen
    XStoreName ( x_display , win , "GL test" ); // give the window a name

    glis_class.native_window = win;
    glis_class.display_id = (EGLNativeDisplayType) x_display;
    glis.GLIS_setupOnScreenRendering(glis_class);

    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glis.GLIS_SwapBuffers(glis_class);

    sleep(2);
    LOG_INFO("destroying glis");
    glis.GLIS_destroy_GLIS(glis_class);
    LOG_INFO("destroyed glis");
    XDestroyWindow    ( x_display, win );
    XCloseDisplay     ( x_display );
    return 0;
}