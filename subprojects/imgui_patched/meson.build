project('imgui_patched', 'cpp')

cmake     = import('cmake')
sdl2_proj = cmake.subproject('sdl2')
sdl2_dep  = sdl2_proj.dependency('SDL2-static')

# # pick up generated SDL_config
# add_project_arguments(
#   '-include',
#   meson.global_build_root() / 'subprojects/sdl2/__CMake_build/include/SDL_config.h',
#   language : ['c', 'cpp'],
# )

imgui_inc = include_directories('.', 'backends')

imgui_lib = static_library('imgui',
  'imgui.cpp',
  'imgui_draw.cpp',
  'imgui_widgets.cpp',
  'imgui_tables.cpp',
  'backends/imgui_impl_sdl2.cpp',
  'backends/imgui_impl_sdlrenderer2.cpp',
  'misc/cpp/imgui_stdlib.cpp',
  include_directories : imgui_inc,
  dependencies        : sdl2_dep,
)

imgui_dep = declare_dependency(
  link_with           : imgui_lib,
  include_directories : imgui_inc,
)

meson.override_dependency('imgui_patched', imgui_dep)