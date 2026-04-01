project ('Divizion', 'cpp',
  version: '0.0.1',
  default_options: ['cpp_std=c++14'],
  meson_version: '>=0.56')

## modules
cpp = meson.get_compiler('cpp')
cmake = import('cmake')

shlwapi = cpp.find_library('shlwapi',
  required: host_machine.system()=='windows')

add_global_arguments( # fix SDL compilation errors
  '-D__FIReference_1_int=__FIReference_1_INT32',
  '-D__FIReference_1_int_get_Value=__FIReference_1_INT32_get_Value',
  '-D__FIReference_1_int_Release=__FIReference_1_INT32_Release',
  language:['c','cpp'])

# SDL deps
sdl2_proj = cmake.subproject('sdl2',
  cmake_options : [
    '-DSDL_STATIC=ON',
    '-DSDL_SHARED=OFF',
    '-DSDL_LIBC=ON',
    '-DCMAKE_POLICY_VERSION_MINIMUM=3.5',
    '-DSDL_AUDIO=OFF',
  ])
sdl2_dep = sdl2_proj.dependency('SDL2-static')
sdl2main_dep = sdl2_proj.dependency('SDL2main')

# dearimgui deps
imgui_proj = subproject('imgui_patched')
imgui_dep = imgui_proj.get_variable('imgui_dep')

# igfd_proj = subproject('igfd')
# igfd_dep = igfd_proj.get_variable('igfd_dep')

imgui_sw_proj = subproject('imgui_software_renderer')
imgui_sw_dep = imgui_sw_proj.get_variable('imgui_sw_dep')

# furnace deps
furnace_opts = cmake.subproject_options()
furnace_proj = cmake.subproject('furnace', options: furnace_opts)
furnace_dep = furnace_proj.dependency('furnace')

## source files

divizion_src = files(

)

gui_src = files(
  'src/gui.cpp'
)

mock_src = files(
  'src/mock/mock.cpp'
)

src = [divizion_src, gui_src, mock_src]

if get_option('standalone')
  src += 'src/standalone/main.cpp'
endif

## windows def file
# def = 'src/divizion.def'

## includes
inc = []
# inc += imgui_inc

## dependencies
deps = [shlwapi,
  sdl2_dep, sdl2main_dep,
  imgui_dep, imgui_sw_dep,
  furnace_dep]

## compiler flags
ldflags = []
if host_machine.system() == 'windows'
  compiler = cpp.get_id()
  # as a static binary
  if compiler == 'gcc'
    add_project_link_arguments(
      '-static',
      language: ['c','cpp'])
    ldflags = [meson.project_source_root() / def]
  elif compiler == 'msvc'
    add_project_arguments(
      '/D_CRT_SECURE_NO_WARNINGS',
      language: ['c','cpp'])
  endif

  # More SDL stuff
  ldflags += '-lsetupapi'
  ldflags += '-limm32'
  ldflags += '-lcfgmgr32'
  ldflags += '-lversion'
endif

# ldflags += '-Wl,--strip-all'
ldflags += '-Wl,--as-needed'

## build
if get_option('vst2')
  shared_module('Divizion',
    src,
    include_directories: inc,
    dependencies: deps,
    name_prefix: '',
    link_args: ldflags,
    install: false)
elif get_option('standalone')
  executable('divizion',
    src,
    include_directories: inc,
    dependencies: deps,
    name_prefix: '',
    link_args: ldflags,
    install: false)
endif

