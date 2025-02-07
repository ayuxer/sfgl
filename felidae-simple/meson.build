os = build_machine.system()
if os == 'linux' or os == 'freebsd' or os == 'openbsd'
  felidae_simple_src = files(
    'felidae.c'
  )
  felidae_simple_inc = include_directories('include')
  felidae_simple_deps = [libfelidae_common_dep, libfelidae_windowing_dep, libfelidae_opengl_dep]
else
  error('Unsupported system: ' + os)
endif

libfelidae_simple = static_library('felidae_simple', felidae_simple_src,
  dependencies: felidae_simple_deps,
  include_directories : felidae_simple_inc,
  install : true)

libfelidae_simple_dep = declare_dependency(
  link_with: libfelidae_simple,
  include_directories: felidae_simple_inc,
  dependencies: felidae_simple_deps)
