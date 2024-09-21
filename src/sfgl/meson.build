os = build_machine.system()
if os == 'linux' or os == 'freebsd' or os == 'openbsd'
  sfgl_src = files(
    'graphics.c',
    'opengl/context.c',
    'unix/x11/window.c',
    'unix/x11/event.c',
  )

  xcb_dep = dependency('xcb')
  egl_dep = dependency('egl')
  sfgl_deps = [xcb_dep, egl_dep]
else
  error('Unsupported system: ' + os)
endif

sfgl_lib = static_library('sfgl', sfgl_src,
  dependencies: sfgl_deps,
  include_directories : sfgl_inc,
  install : true)

sfgl_dep = declare_dependency(
  link_with: sfgl_lib,
  include_directories:  sfgl_inc,
  dependencies: sfgl_deps)
