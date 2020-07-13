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

    XSetWindowAttributes  xattr;
    Atom  atom;
    int   one = 1;

    xattr.override_redirect = False;
    XChangeWindowAttributes ( x_display, win, CWOverrideRedirect, &xattr );

    atom = XInternAtom ( x_display, "_NET_WM_STATE_FULLSCREEN", True );
    XChangeProperty (
            x_display, win,
            XInternAtom ( x_display, "_NET_WM_STATE", True ),
            XA_ATOM,  32,  PropModeReplace,
            (unsigned char*) &atom,  1 );

    XChangeProperty (
            x_display, win,
            XInternAtom ( x_display, "_HILDON_NON_COMPOSITED_WINDOW", False ),
            XA_INTEGER,  32,  PropModeReplace,
            (unsigned char*) &one,  1);

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    XMapWindow ( x_display , win );             // make the window visible on the screen
    XStoreName ( x_display , win , "GL test" ); // give the window a name

    //// get identifiers for the provided atom name strings
    Atom wm_state   = XInternAtom ( x_display, "_NET_WM_STATE", False );
    Atom fullscreen = XInternAtom ( x_display, "_NET_WM_STATE_FULLSCREEN", False );

    XEvent xev;
    memset ( &xev, 0, sizeof(xev) );

    xev.type                 = ClientMessage;
    xev.xclient.window       = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 1;
    xev.xclient.data.l[1]    = fullscreen;
    XSendEvent (                // send an event mask to the X-server
            x_display,
            DefaultRootWindow ( x_display ),
            False,
            SubstructureNotifyMask,
            &xev );

    glis_class.native_window = win;
    glis_class.display_id = (EGLNativeDisplayType) x_display;
    glis.GLIS_setupOnScreenRendering(glis_class);
    sleep(2);
    LOG_INFO("destroying glis");
    glis.GLIS_destroy_GLIS(glis_class);
    LOG_INFO("destroyed glis");
    XDestroyWindow    ( x_display, win );
    XCloseDisplay     ( x_display );
    return 0;
}