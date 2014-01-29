
#include <QtGui/QGuiApplication>
#include <QDebug>
#include <QScreen>
#include "qtquick2applicationviewer.h"
#include <X11/Xlib.h>
#include <X11/X.h>

int main(int argc, char *argv[])
{
    Display* display = XOpenDisplay(NULL);

    XSynchronize(display, True);
    //    XSetErrorHandler(myErrorHandler);

    Window x11root = XDefaultRootWindow(display);

    int x = 500;
    int y = 500;
    unsigned int width = 640;
    unsigned int height = 480;
    unsigned int borderWidth = 0;
    long colorBlue = 0xff0000ff;

    Window x11w = XCreateSimpleWindow(display, x11root, x, y,
                                      width, height, borderWidth, 1 /*magic number*/, colorBlue);
    QGuiApplication app(argc, argv);

    QRect virtualGeometry = app.screens().at(0)->virtualGeometry();

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/fullscreentest/main.qml"));
    //    viewer.setGeometry(screenList.at(0)->virtualGeometry());
    qDebug() << viewer.isTopLevel();
    XReparentWindow(display, viewer.winId(), x11w, 0, 0);

    XMapWindow(display, x11w); // must be performed after XReparentWindow,
    // otherwise the window is not visible.
    viewer.setGeometry(virtualGeometry);
    viewer.show();
    //    XFlush(display);

    XEvent xev;
    Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = x11w;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = fullscreen;
    xev.xclient.data.l[2] = 0;
    XSendEvent(display, XDefaultRootWindow(display), False,
               SubstructureNotifyMask, &xev);

    Atom fullmons = XInternAtom(display, "_NET_WM_FULLSCREEN_MONITORS", False);
    XEvent xev2;
    memset(&xev2, 0, sizeof(xev2));
    xev2.type = ClientMessage;
    xev2.xclient.window = x11w;
    xev2.xclient.message_type = fullmons;
    xev2.xclient.format = 32;
    xev2.xclient.data.l[0] = 0; /* your topmost monitor number */
    xev2.xclient.data.l[1] = 1; /* bottommost */
    xev2.xclient.data.l[2] = 0; /* leftmost */
    xev2.xclient.data.l[3] = 1; /* rightmost */
    xev2.xclient.data.l[4] = 0; /* source indication */

    XSendEvent (display, DefaultRootWindow(display), False,
                SubstructureRedirectMask | SubstructureNotifyMask, &xev2);


    qDebug() << "At bottom";
    return app.exec();
}
