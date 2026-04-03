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
  '-DFMT_HEADER_ONLY',
  language:['c','cpp'])

if host_machine.system() == 'windows'
  add_project_arguments('-DWIN32', language: ['c', 'cpp'])
endif

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

xxd = find_program('xxd')
fontawesome_header = custom_target(
  'gen_fontawesome_header',
  input: 'src/FontAwesome.otf',
  output: 'gen_fontawesome.cpp',
  command: [xxd, '-i', '@INPUT@', '@OUTPUT@'],
)

furicon_header = custom_target(
  'gen_furicon_header',
  input: 'src/icons.ttf',
  output: 'gen_ficons.cpp',
  command: [xxd, '-i', '@INPUT@', '@OUTPUT@'],
)

## source files

divizion_src = files(
  'src/divizion.cpp'
)

gui_src = files(
  'src/const.cpp',
  'src/furnace_widgets.cpp',
  'src/furnace_gui_utils.cpp',
  'src/gui.cpp',
  'src/gui_render_list_item.cpp',
  'src/gui_draw_macros.cpp',
  'src/impl_actions.cpp',
  'src/impl_actions_draw_chip_info.cpp',
  # 'src/impl_actions_draw_chan_info.cpp',
  'src/impl_actions_sys_categories.cpp',
  'src/impl_actions_draw_instr_info.cpp',
  'src/plot_nolerp.cpp',
  'src/text_input_widget.cpp',
  'src/mml.cpp',
  'src/waveEdit.cpp',
  'src/gui_process_drags.cpp'
)
gui_src += [fontawesome_header, furicon_header]

vst_src = files(
  'src/vst2/dispatch.cpp',
  'src/vst2/entry.cpp',
  'src/vst2/output.cpp',
  'src/vst2/params.cpp',
  'src/vst2/process.cpp',
  'src/vst2/io.cpp',
  'src/vst2/handle_ui.cpp',
)

src = [divizion_src, gui_src]

if host_machine.system() == 'windows'
  win = import('windows')
  res = win.compile_resources('src/res.rc')
  src = src + [res]
endif

vst_total_src = src + vst_src

vst_def = 'src/vst2/divizion.def'

standalone_total_src = src + ['src/standalone/main.cpp']
standalone_def = '../src/standalone/divizion.def' # ??????????

## includes
inc = []
# inc += imgui_inc

zlib_dep = dependency('zlib', required:host_machine.system()=='linux')
dl_dep = cpp.find_library('dl', required:host_machine.system()=='linux')
rtmidi_dep = cpp.find_library('rtmidi', required:host_machine.system()=='linux')

## dependencies
deps = [shlwapi,
  sdl2_dep, sdl2main_dep,
  imgui_dep, imgui_sw_dep,
  furnace_dep,
  zlib_dep, dl_dep, rtmidi_dep]

## compiler flags
ldflags = []
if host_machine.system() == 'windows'
  compiler = cpp.get_id()
  # as a static binary
  if compiler == 'gcc'
    add_project_link_arguments(
      '-static',
      language: ['c','cpp'])
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
  ldflags += '-lcomctl32'
  ldflags += '-s'
  
  vst_total_ldflags = ldflags + [meson.project_source_root() / vst_def]
endif

standalone_total_ldflags = ldflags
if host_machine.system() == 'windows'
  standalone_total_ldflags += [standalone_def]
endif

# ldflags += '-Wl,--strip-all'
ldflags += '-Wl,--as-needed'

## build
if get_option('vst2') and host_machine.system() == 'windows'
  shared_module('Divizion',
    vst_total_src,
    include_directories: inc,
    dependencies: deps,
    name_prefix: '',
    link_args: vst_total_ldflags,
    install: false)
endif

if get_option('standalone')
  executable('divizion',
    standalone_total_src,
    include_directories: inc,
    dependencies: deps,
    name_prefix: '',
    link_args: standalone_total_ldflags,
    install: false)
endif

