# felidae

Lightweight, minimal, crossplatform, and straightforward OpenGL- (?...and futurely Vulkan-) based 
rendering and windowing libraries.

## Scope of this project

### felidae-common

- [x] Graphics API-agnostic payload definitions

### felidae-graphics

**Dependencies:** `gl` `egl`

- [x] Windowing subproject interoperability: Unix/X11
- [ ] Windowing subproject interoperability: Win32
- [ ] Drawing of geometric shapes
- [ ] Font rendering
- [ ] GLSL shaders
- [ ] SPIR-V intermediate shader language support
- [ ] Lossless render scaling / aspect ratio

### felidae-windowing

**Dependencies (UNIX-like systems):** `libxcb`

- [x] Unix/X11
  - [x] Window initialization
  - [x] Colormap initialization
  - [x] Multi-monitor handling
  - [x] Window property updating
- [ ] Win32
  - [ ] Window initialization
  - [ ] Colormap initialization
  - [ ] Multi-monitor handling
  - [ ] Window property updating
- [x] Poll-based event API

### felidae-simple

**Dependencies:** `felidae-*`

- [x] High-level windowing API
  - [x] Window and colormap initialization
  - [x] Multi-monitor support
  - [ ] Window property updating
- [ ] High-level OpenGL graphics
  - [ ] Drawing of geometric shapes
  - [ ] Font rendering
  - [ ] Shading languages support

## Compiling

```bash
meson setup build
ninja -C build
```
