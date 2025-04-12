#pragma once

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

class KeyloggerDisplay {
public:
  void display() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
      std::cerr << "Cannot open X display\n";
      return;
    }
    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);
    // Unmanaged window (no decorations or resizing)
    XSetWindowAttributes attr;
    attr.override_redirect = True;
    attr.background_pixel = WhitePixel(display, screen);
    int x = 100, y = 100, width = 400, height = 200;
    Window window = XCreateWindow(
      display, root,
      x, y, width, height, 0,
      CopyFromParent, InputOutput,
      CopyFromParent,
      CWOverrideRedirect | CWBackPixel, &attr
    );
    XMapWindow(display, window);
    XFlush(display);
    // Wait for the window to be mapped
    XEvent ev;
    do {
      XNextEvent(display, &ev);
    } while (ev.type != MapNotify);
    // Graphics context
    GC gc = XCreateGC(display, window, 0, nullptr);
    XSetForeground(display, gc, BlackPixel(display, screen));
    XSetBackground(display, gc, WhitePixel(display, screen));
    // Load font safely
    XFontStruct* font = XLoadQueryFont(display, "fixed");
    if (!font) {
      std::cerr << "Failed to load font.\n";
      return;
    }
    XSetFont(display, gc, font->fid);
    const char* msg = "✅ Xlib text works!";
    XDrawString(display, window, gc, 50, 100, msg, std::strlen(msg));
    XFlush(display);
    std::cout << "Text drawn. Window stays up.\n";
    while (true) {
      usleep(100000); // Keep alive
    }
    XFreeFont(display, font);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
  }
};
