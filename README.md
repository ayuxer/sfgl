# sfgl

**S**traight**f**orward **G**raphics **L**ibrary: Lightweight, minimal, crossplatform
OpenGL-based rendering and windowing library.

## Status of this project

* [x] Window initialization
* [ ] Drawing of geometric shapes
* [ ] Font rendering
* [ ] Lossless render scaling / aspect ratio
* [x] Platform: UNIX-like systems, X11 via XCB
* [ ] Platform: Windows
* [ ] Platform: macOS

## Compiling

**Dependencies:** `libxcb` `egl`

```
meson setup build
ninja -C build
```
