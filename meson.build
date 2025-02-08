project('felidae', 'c',
  license : 'BSD-3-Clause',
  version : '0.1.0',
  default_options : ['c_std=c23', 'warning_level=3'])

cc = meson.get_compiler('c')
add_project_arguments('-D_POSIX_C_SOURCE=199309L', language : 'c')

subdir('felidae-common')
subdir('felidae-windowing')
subdir('felidae-opengl')
subdir('felidae-simple')
subdir('examples')
