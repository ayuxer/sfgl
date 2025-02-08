os = build_machine.system()
if os == 'linux' or os == 'freebsd' or os == 'openbsd'
  felidae_common_src = files('payload.c', 'platform.c')
  felidae_common_inc = include_directories('include')
  felidae_common_deps = []
else
  error('Unsupported system: ' + os)
endif

libfelidae_common = static_library('felidae_common', felidae_common_src,
  dependencies: felidae_common_deps,
  include_directories : felidae_common_inc,
  install : true)

libfelidae_common_dep = declare_dependency(
  link_with: libfelidae_common,
  include_directories: felidae_common_inc,
  dependencies: felidae_common_deps)
