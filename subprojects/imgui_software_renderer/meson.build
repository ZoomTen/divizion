project('imgui_sw', 'cpp')
imgui_dep = dependency('imgui_patched')

cmake     = import('cmake')
sdl2_proj = cmake.subproject('sdl2')
sdl2_dep  = sdl2_proj.dependency('SDL2-static')

imgui_sw_inc = include_directories('.')

imgui_sw_lib = static_library('imgui_sw',
  'imgui_sw.cpp',
  include_directories : imgui_sw_inc,
  dependencies: [imgui_dep, sdl2_dep]
)

imgui_sw_dep = declare_dependency(
  link_with           : imgui_sw_lib,
  include_directories : imgui_sw_inc,
)