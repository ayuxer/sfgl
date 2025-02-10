os = build_machine.system()
if os == 'linux' or os == 'freebsd' or os == 'openbsd'
  felidae_opengl_src = files(
    'graphics.c',
    'wrapper.c',
    'context.c',
    'batch.c',
    '../third_party/glad/src/gl.c',
    '../third_party/stb/src/image.c'
  )
  felidae_opengl_inc = include_directories('include', '../third_party/glad/include', '../third_party/stb/include', '../third_party/cglm/include')

  egl_dep = dependency('egl')
  math_dep = cc.find_library('m')
  felidae_opengl_deps = [math_dep, egl_dep, libfelidae_common_dep, libfelidae_windowing_dep]
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
