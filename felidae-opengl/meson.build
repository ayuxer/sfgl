os = build_machine.system()
if os == 'linux' or os == 'freebsd' or os == 'openbsd'
  felidae_opengl_src = files(
    'graphics.c',
    'context.c'
  )
  felidae_opengl_inc = include_directories('include')

  egl_dep = dependency('egl')
  felidae_opengl_deps = [egl_dep, libfelidae_common_dep, libfelidae_windowing_dep]
else
  error('Unsupported system: ' + os)
endif

libfelidae_opengl = static_library('felidae_opengl', felidae_opengl_src,
  dependencies: felidae_opengl_deps,
  include_directories : felidae_opengl_inc,
  install : true)

libfelidae_opengl_dep = declare_dependency(
  link_with: libfelidae_opengl,
  include_directories: felidae_opengl_inc,
  dependencies: felidae_opengl_deps)
