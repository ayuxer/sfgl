os = build_machine.system()
if os == 'linux' or os == 'freebsd' or os == 'openbsd'
  felidae_windowing_src = files(
    'unix/x11/core.c',
    'unix/x11/event.c',
    'core.c'
  )
  felidae_windowing_inc = include_directories('include')

  xcb_dep = dependency('xcb')
  felidae_windowing_deps = [xcb_dep, libfelidae_common_dep]
else
  error('Unsupported system: ' + os)
endif

libfelidae_windowing = static_library('felidae_windowing', felidae_windowing_src,
  dependencies: felidae_windowing_deps,
  include_directories : felidae_windowing_inc,
  install : true)

libfelidae_windowing_dep = declare_dependency(
  link_with: libfelidae_windowing,
  include_directories: felidae_windowing_inc,
  dependencies: felidae_windowing_deps)
