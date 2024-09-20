project('sfgl', 'c',
  license : 'BSD-3-Clause',
  version : '0.1.0',
  default_options : ['c_std=c23', 'warning_level=3'])

inc = include_directories('include')
src = ['src/sfgl/unix/x11/egl.c', 'src/sfgl/unix/x11/window.c', 'src/sfgl/unix/x11/event.c']
lib = static_library('sfgl', src, include_directories : inc, install : true)
deps = [declare_dependency(link_with : lib, include_directories : inc)]

os = build_machine.system()
if os == 'linux' or os == 'freebsd' or os == 'openbsd'
  deps += [dependency('xcb'), dependency('egl')]
endif

example = executable('example', 'examples/example.c', dependencies : deps)
