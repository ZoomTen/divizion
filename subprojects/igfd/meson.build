project('igfd', 'cpp')
imgui_dep = dependency('imgui_patched')

igfd_inc = include_directories('.')

igfd_lib = static_library('igfd',
  'ImGuiFileDialog.cpp',
  dependencies: imgui_dep
)

igfd_dep = declare_dependency(
  link_with           : igfd_lib,
  include_directories : igfd_inc,
)