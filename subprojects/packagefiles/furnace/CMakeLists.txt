cmake_minimum_required(VERSION 3.0...3.16)

# This CMakeList tries to compile Furnace "as-a-library"
# Includes the GUI functions, should I want to include it inside of the VST.

project(furnace)
set(FURNACE furnace)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 6)
set(CMAKE_PROJECT_VERSION_PATCH 8)

include(CheckIncludeFile)
include(TestBigEndian)

execute_process(COMMAND git status RESULT_VARIABLE DONT_HAVE_GIT WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
if (NOT DONT_HAVE_GIT)
  message(STATUS "Git is available")
else()
  message(WARNING "either Git is not available, or this Git repository has not been initialized.
if you have used the \"Source code\" option in the GitHub release page, you are doing it wrong! unless you manually initialize submodules, the build is guaranteed to FAIL spectacularly!
read the \"developer info\" section of README.md for more information.")
endif()

# option(USE_SNDFILE "Build with libsndfile. Required in order to work with audio files." OFF)
# option(WITH_OGG "Whether to build with Ogg and Vorbis support." OFF)
# option(WITH_MPEG "Whether to build with MPEG audio support (including MP3)." OFF)

option(WARNINGS_ARE_ERRORS "Whether warnings in furnace's C++ code should be treated as errors" OFF)

set(DEPENDENCIES_INCLUDE_DIRS extern/IconFontCppHeaders extern/blip_buf src/icon)
set(DEPENDENCIES_DEFINES "")
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if (IS_BIG_ENDIAN)
  list(APPEND DEPENDENCIES_DEFINES "TA_BIG_ENDIAN")
endif()
set(DEPENDENCIES_COMPILE_OPTIONS "-include" "stdint.h")
set(DEPENDENCIES_LIBRARIES "")
set(DEPENDENCIES_LIBRARY_DIRS "")
set(DEPENDENCIES_LINK_OPTIONS "")
set(DEPENDENCIES_LEGACY_LDFLAGS "")

#list(APPEND DEPENDENCIES_INCLUDE_DIRS "extern/SAASound/include")
list(APPEND DEPENDENCIES_INCLUDE_DIRS "extern/vgsound_emu-modified")

# threads
find_package(Threads REQUIRED)
list(APPEND DEPENDENCIES_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

# xp support
list(APPEND DEPENDENCIES_DEFINES "_WIN32_WINNT=0x0500")
list(APPEND DEPENDENCIES_DEFINES "SUPPORT_XP")
if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(WITH_OUR_MALLOC ON CACHE BOOL "aaa" FORCE)
endif()
set(BUILD_TESTS OFF CACHE BOOL "come on" FORCE)

# fftw
add_subdirectory(extern/fftw EXCLUDE_FROM_ALL)
list(APPEND DEPENDENCIES_INCLUDE_DIRS extern/fftw/api)
list(APPEND DEPENDENCIES_LIBRARIES fftw3)
message(STATUS "Using vendored FFTW")

# fmt
add_subdirectory(extern/fmt EXCLUDE_FROM_ALL)
list(APPEND DEPENDENCIES_INCLUDE_DIRS extern/fmt/include)
list(APPEND DEPENDENCIES_LIBRARIES fmt)
message(STATUS "Using vendored fmt")

# opus
# set(OPUS_X86_MAY_HAVE_AVX2 OFF CACHE BOOL "Enable AVX2 for Opus" FORCE)
# set(OPUS_FORTIFY_SOURCE OFF CACHE BOOL "Enable Fortify Source" FORCE)
# set(OPUS_STACK_PROTECTOR OFF CACHE BOOL "Enable Fortify Source" FORCE)
# add_subdirectory(extern/opus EXCLUDE_FROM_ALL)

# libsndfile
# list(APPEND DEPENDENCIES_DEFINES HAVE_SNDFILE)
# # Ogg, Vorbis, FLAC and MP3
# if (WITH_OGG)
#   list(APPEND DEPENDENCIES_DEFINES HAVE_OGG)
#   set(OGG_INCLUDE_DIR extern/libogg/include CACHE STRING "Ogg include dir" FORCE)
#   set(OGG_LIBRARY Ogg::ogg CACHE STRING "Ogg library" FORCE)
#   set(Vorbis_Vorbis_INCLUDE_DIR extern/libvorbis/include CACHE STRING "Vorbis include dir" FORCE)
#   set(Vorbis_Enc_INCLUDE_DIR extern/libvorbis/include CACHE STRING "Vorbis enc include dir" FORCE)
#   set(Vorbis_File_INCLUDE_DIR extern/libvorbis/include CACHE STRING "Vorbis file include dir" FORCE)
#   set(Vorbis_Vorbis_LIBRARY Vorbis::vorbis CACHE STRING "Vorbis library" FORCE)
#   set(Vorbis_Enc_LIBRARY Vorbis::enc CACHE STRING "Vorbis enc library" FORCE)
#   set(Vorbis_File_LIBRARY Vorbis::file CACHE STRING "Vorbis file library" FORCE)
#   set(FLAC_INCLUDE_DIR extern/flac/include CACHE STRING "FLAC include dir" FORCE)
#   set(FLAC_LIBRARY FLAC CACHE STRING "FLAC library" FORCE)
#   set(OPUS_INCLUDE_DIR extern/opus/include CACHE STRING "Opus include dir" FORCE)
#   set(OPUS_LIBRARY opus CACHE STRING "Opus library" FORCE)
#   add_subdirectory(extern/libogg EXCLUDE_FROM_ALL)
#   add_subdirectory(extern/libvorbis EXCLUDE_FROM_ALL)
#   set(WITH_FORTIFY_SOURCE OFF CACHE BOOL "Enable Fortify Source" FORCE)
#   set(WITH_STACK_PROTECTOR OFF CACHE BOOL "Enable Fortify Source" FORCE)
#   add_subdirectory(extern/flac EXCLUDE_FROM_ALL)
# endif()
# if (WITH_MPEG)
#   list(APPEND DEPENDENCIES_DEFINES HAVE_MP3_EXPORT)
#   set(MP3LAME_INCLUDE_DIR extern/lame/include CACHE STRING "LAME include dir" FORCE)
#   set(MP3LAME_LIBRARY mp3lame CACHE STRING "LAME library" FORCE)
#   add_subdirectory(extern/lame EXCLUDE_FROM_ALL)
#   add_subdirectory(extern/mpg123 EXCLUDE_FROM_ALL)
#   set(mpg123_INCLUDE_DIR extern/mpg123/src/include CACHE STRING "mpg123 include dir" FORCE)
#   set(mpg123_LIBRARY mpg123 CACHE STRING "mpg123 library" FORCE)
# endif()
# # the rest
# set(BUILD_TESTING OFF CACHE BOOL "aaaaaa" FORCE)
# set(BUILD_PROGRAMS OFF CACHE BOOL "aaa" FORCE)
# set(BUILD_EXAMPLES OFF CACHE BOOL "a" FORCE)
# set(ENABLE_EXTERNAL_LIBS ${WITH_OGG} CACHE BOOL "come on" FORCE)
# set(ENABLE_MPEG ${WITH_MPEG} CACHE BOOL "come on" FORCE)
# add_subdirectory(extern/libsndfile-modified EXCLUDE_FROM_ALL)
# list(APPEND DEPENDENCIES_LIBRARIES sndfile)
# message(STATUS "Using vendored libsndfile")

# rtmidi
add_subdirectory(extern/rtmidi EXCLUDE_FROM_ALL)
list(APPEND DEPENDENCIES_LIBRARIES rtmidi)
message(STATUS "Using vendored RtMidi")

# zlib
set(BUILD_TESTING OFF CACHE BOOL "aaaaaa" FORCE)
set(BUILD_PROGRAMS OFF CACHE BOOL "aaa" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "a" FORCE)
set(ENABLE_EXTERNAL_LIBS OFF CACHE BOOL "come on" FORCE)
set(ENABLE_MPEG OFF CACHE BOOL "come on" FORCE)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "come on" FORCE)
add_subdirectory(extern/zlib EXCLUDE_FROM_ALL)
list(APPEND DEPENDENCIES_INCLUDE_DIRS extern/zlib ${CMAKE_CURRENT_BINARY_DIR}/extern/zlib)
list(APPEND DEPENDENCIES_LIBRARIES zlibstatic)
message(STATUS "Using vendored zlib")

# I've already got SDL for the VST GUI.. do I need this?
# set(SDL_SHARED OFF CACHE BOOL "Force no dynamically-linked SDL" FORCE)
# set(SDL_STATIC ON CACHE BOOL "Force statically-linked SDL" FORCE)
# set(SDL_PIPEWIRE OFF CACHE BOOL "Use Pipewire audio" FORCE)
# set(SDL_LIBC ON CACHE BOOL "Tell SDL that we want it to use our C runtime (required for proper static linking)" FORCE)
# set(SDL_RENDER_D3D OFF CACHE BOOL "Enable the Direct3D render driver" FORCE)
# set(SDL_OPENGLES OFF CACHE BOOL "Enable OpenGL ES" FORCE)
# add_subdirectory(extern/SDL EXCLUDE_FROM_ALL)
# list(APPEND DEPENDENCIES_DEFINES HAVE_SDL2)
# list(APPEND DEPENDENCIES_INCLUDE_DIRS extern/SDL/include)
# list(APPEND DEPENDENCIES_LIBRARIES SDL2-static)
# # Work around add_subdirectory'd SDL not propagating HAVE_LIBC to MSVC furnace build
# if (MSVC)
#   list(APPEND DEPENDENCIES_COMPILE_OPTIONS "/DHAVE_LIBC")
# endif()
# ist(APPEND DEPENDENCIES_LIBRARIES SDL2main)
# message(STATUS "Using vendored SDL2")

# audio engines
set(AUDIO_SOURCES
  src/audio/abstract.cpp
  src/audio/midi.cpp
  src/audio/pipe.cpp
)

# rtmidi
list(APPEND AUDIO_SOURCES src/audio/rtmidi.cpp)
message(STATUS "Building with RtMidi")
list(APPEND DEPENDENCIES_DEFINES HAVE_RTMIDI)

# this one has to be copied from the original CMakeLists file!!
set(ENGINE_SOURCES
src/log.cpp
src/baseutils.cpp
src/fileutils.cpp
src/utfutils.cpp

extern/itcompress/compression.c

extern/SAASound/src/SAAAmp.cpp
extern/SAASound/src/SAADevice.cpp
extern/SAASound/src/SAAEnv.cpp
extern/SAASound/src/SAAFreq.cpp
extern/SAASound/src/SAAImpl.cpp
extern/SAASound/src/SAANoise.cpp
extern/SAASound/src/SAASndC.cpp
extern/SAASound/src/SAASound.cpp

extern/vgsound_emu-modified/vgsound_emu/src/core/vox/vox.cpp

extern/vgsound_emu-modified/vgsound_emu/src/es550x/es550x.cpp
extern/vgsound_emu-modified/vgsound_emu/src/es550x/es550x_alu.cpp
extern/vgsound_emu-modified/vgsound_emu/src/es550x/es550x_filter.cpp
extern/vgsound_emu-modified/vgsound_emu/src/es550x/es5504.cpp
extern/vgsound_emu-modified/vgsound_emu/src/es550x/es5505.cpp
extern/vgsound_emu-modified/vgsound_emu/src/es550x/es5506.cpp

extern/vgsound_emu-modified/vgsound_emu/src/k005289/k005289.cpp

extern/vgsound_emu-modified/vgsound_emu/src/k007232/k007232.cpp

extern/vgsound_emu-modified/vgsound_emu/src/k053260/k053260.cpp

extern/vgsound_emu-modified/vgsound_emu/src/msm6295/msm6295.cpp

extern/vgsound_emu-modified/vgsound_emu/src/n163/n163.cpp

extern/vgsound_emu-modified/vgsound_emu/src/scc/scc.cpp

extern/vgsound_emu-modified/vgsound_emu/src/vrcvi/vrcvi.cpp

extern/vgsound_emu-modified/vgsound_emu/src/x1_010/x1_010.cpp

extern/adpcm/bs_codec.c
extern/adpcm/oki_codec.c
extern/adpcm/yma_codec.c
extern/adpcm/ymb_codec.c
extern/adpcm/ymz_codec.c

extern/adpcm-xq-s/adpcm-lib.c

extern/opn/ym3438.c
extern/Nuked-PSG/ympsg.c
extern/opm/opm.c
extern/Nuked-OPLL/opll.c
extern/opl/opl3.c
extern/YM3812-LLE/fmopl2.c
extern/YMF262-LLE/fmopl3.c
extern/YMF276-LLE/fmopn2.c
extern/ESFMu/esfm.c
extern/ESFMu/esfm_registers.c
extern/emu2413/emu2413.c
extern/YM2608-LLE/fmopna_2608.c
extern/YM2608-LLE/fmopna_2610.c
extern/YM2608-LLE/fmopna_2612.c

extern/pwrnoise/pwrnoise.c

extern/blip_buf/blip_buf.c

src/pch.cpp

src/engine/platform/sound/sn76496.cpp
src/engine/platform/sound/ay8910.cpp
src/engine/platform/sound/saa1099.cpp
src/engine/platform/sound/namco.cpp
src/engine/platform/sound/segapcm.cpp
src/engine/platform/sound/gb/apu.c
src/engine/platform/sound/gb/timing.c
src/engine/platform/sound/pce_psg.cpp
src/engine/platform/sound/nes/apu.cpp
src/engine/platform/sound/nes/fds.cpp
src/engine/platform/sound/nes/mmc5.cpp
src/engine/platform/sound/vera_psg.c
src/engine/platform/sound/vera_pcm.c
src/engine/platform/sound/ymf278b/ymf278.cpp

src/engine/platform/sound/atomicssg/ssg.c

src/engine/platform/sound/nes_nsfplay/5e01_apu.cpp
src/engine/platform/sound/nes_nsfplay/5e01_dmc.cpp
src/engine/platform/sound/nes_nsfplay/nes_apu.cpp
src/engine/platform/sound/nes_nsfplay/nes_dmc.cpp
src/engine/platform/sound/nes_nsfplay/nes_fds.cpp
src/engine/platform/sound/nes_nsfplay/nes_mmc5.cpp
src/engine/platform/sound/nes_nsfplay/nes_n106.cpp
src/engine/platform/sound/nes_nsfplay/nes_vrc6.cpp

src/engine/platform/sound/c64/sid.cc
src/engine/platform/sound/c64/voice.cc
src/engine/platform/sound/c64/wave.cc
src/engine/platform/sound/c64/envelope.cc
src/engine/platform/sound/c64/filter.cc
src/engine/platform/sound/c64/extfilt.cc
src/engine/platform/sound/c64/pot.cc
src/engine/platform/sound/c64/version.cc

src/engine/platform/sound/c64/wave6581_PS_.cc
src/engine/platform/sound/c64/wave6581_PST.cc
src/engine/platform/sound/c64/wave6581_P_T.cc
src/engine/platform/sound/c64/wave6581__ST.cc
src/engine/platform/sound/c64/wave8580_PS_.cc
src/engine/platform/sound/c64/wave8580_PST.cc
src/engine/platform/sound/c64/wave8580_P_T.cc
src/engine/platform/sound/c64/wave8580__ST.cc

src/engine/platform/sound/c64_fp/array.cpp
src/engine/platform/sound/c64_fp/Dac.cpp
src/engine/platform/sound/c64_fp/EnvelopeGenerator.cpp
src/engine/platform/sound/c64_fp/ExternalFilter.cpp
src/engine/platform/sound/c64_fp/Filter6581.cpp
src/engine/platform/sound/c64_fp/Filter8580.cpp
src/engine/platform/sound/c64_fp/Filter.cpp
src/engine/platform/sound/c64_fp/FilterModelConfig6581.cpp
src/engine/platform/sound/c64_fp/FilterModelConfig8580.cpp
src/engine/platform/sound/c64_fp/FilterModelConfig.cpp
src/engine/platform/sound/c64_fp/Integrator6581.cpp
src/engine/platform/sound/c64_fp/Integrator8580.cpp
src/engine/platform/sound/c64_fp/OpAmp.cpp
src/engine/platform/sound/c64_fp/SID.cpp
src/engine/platform/sound/c64_fp/Spline.cpp
src/engine/platform/sound/c64_fp/WaveformCalculator.cpp
src/engine/platform/sound/c64_fp/WaveformGenerator.cpp
src/engine/platform/sound/c64_fp/resample/SincResampler.cpp

src/engine/platform/sound/c64_d/dsid.c

src/engine/platform/sound/tia/AudioChannel.cpp
src/engine/platform/sound/tia/Audio.cpp

src/engine/platform/sound/ymfm/ymfm_adpcm.cpp
src/engine/platform/sound/ymfm/ymfm_opl.cpp
src/engine/platform/sound/ymfm/ymfm_opm.cpp
src/engine/platform/sound/ymfm/ymfm_opn.cpp
src/engine/platform/sound/ymfm/ymfm_opz.cpp
src/engine/platform/sound/ymfm/ymfm_pcm.cpp
src/engine/platform/sound/ymfm/ymfm_ssg.cpp

src/engine/platform/sound/lynx/Mikey.cpp

src/engine/platform/sound/pokey/mzpokeysnd.c
src/engine/platform/sound/pokey/AltASAP.cpp

src/engine/platform/sound/qsound.c

src/engine/platform/sound/swan.c
src/engine/platform/sound/swan_mdfn.cpp

src/engine/platform/sound/su.cpp

src/engine/platform/sound/vic20sound.c

src/engine/platform/sound/ymz280b.cpp

src/engine/platform/sound/vsu.cpp

src/engine/platform/sound/t6w28/T6W28_Apu.cpp

src/engine/platform/sound/rf5c68.cpp

src/engine/platform/sound/oki/msm5232.cpp
src/engine/platform/sound/oki/okim6258.cpp

src/engine/platform/sound/snes/SPC_DSP.cpp

src/engine/platform/sound/ga20/iremga20.cpp

src/engine/platform/sound/sm8521.c

src/engine/platform/sound/supervision.c

src/engine/platform/sound/upd1771.cpp

src/engine/platform/sound/d65modified.c

src/engine/platform/sound/ted-sound.c

src/engine/platform/sound/c140_c219.c

src/engine/platform/sound/dave/dave.cpp

src/engine/platform/sound/nds.cpp

src/engine/platform/sound/sid2/envelope.cc
src/engine/platform/sound/sid2/filter.cc
src/engine/platform/sound/sid2/sid.cc
src/engine/platform/sound/sid2/version.cc
src/engine/platform/sound/sid2/voice.cc
src/engine/platform/sound/sid2/wave8580_PS_.cc
src/engine/platform/sound/sid2/wave8580_PST.cc
src/engine/platform/sound/sid2/wave8580_P_T.cc
src/engine/platform/sound/sid2/wave8580__ST.cc
src/engine/platform/sound/sid2/wave.cc

src/engine/platform/sound/sid3.c

src/engine/platform/oplAInterface.cpp
src/engine/platform/ym2608Interface.cpp
src/engine/platform/ym2610Interface.cpp

src/engine/fileOps/fileOpsCommon.cpp
src/engine/fileOps/dmf.cpp
src/engine/fileOps/fc.cpp
src/engine/fileOps/ftm.cpp
src/engine/fileOps/fur.cpp
src/engine/fileOps/it.cpp
src/engine/fileOps/mod.cpp
src/engine/fileOps/s3m.cpp
src/engine/fileOps/text.cpp
src/engine/fileOps/tfm.cpp
src/engine/fileOps/xm.cpp

src/engine/fileOps/p.cpp
src/engine/fileOps/p86.cpp
src/engine/fileOps/pdx.cpp
src/engine/fileOps/ppc.cpp
src/engine/fileOps/pps.cpp
src/engine/fileOps/pvi.cpp
src/engine/fileOps/pzi.cpp

src/engine/brrUtils.c
src/engine/safeReader.cpp
src/engine/safeWriter.cpp
src/engine/workPool.cpp
src/engine/cmdStream.cpp
src/engine/cmdStreamOps.cpp
src/engine/config.cpp
src/engine/configEngine.cpp
src/engine/dispatchContainer.cpp
src/engine/engine.cpp
src/engine/export.cpp
src/engine/exportDef.cpp
src/engine/fileOpsIns.cpp
src/engine/fileOpsSample.cpp
src/engine/filter.cpp
src/engine/instrument.cpp
src/engine/macroInt.cpp
src/engine/pattern.cpp
src/engine/pitchTable.cpp
src/engine/playback.cpp
src/engine/sample.cpp
src/engine/song.cpp
src/engine/sysDef.cpp
src/engine/wavetable.cpp
src/engine/waveSynth.cpp
src/engine/wavOps.cpp
src/engine/vgmOps.cpp

src/engine/platform/abstract.cpp
src/engine/platform/genesis.cpp
src/engine/platform/genesisext.cpp
src/engine/platform/sms.cpp
src/engine/platform/opll.cpp
src/engine/platform/gb.cpp
src/engine/platform/pce.cpp
src/engine/platform/mmc5.cpp
src/engine/platform/nes.cpp
src/engine/platform/c64.cpp
src/engine/platform/arcade.cpp
src/engine/platform/tx81z.cpp
src/engine/platform/ym2203.cpp
src/engine/platform/ym2203ext.cpp
src/engine/platform/ym2608.cpp
src/engine/platform/ym2608ext.cpp
src/engine/platform/ym2610.cpp
src/engine/platform/ym2610ext.cpp
src/engine/platform/ym2610b.cpp
src/engine/platform/ym2610bext.cpp
src/engine/platform/ay.cpp
src/engine/platform/ay8930.cpp
src/engine/platform/opl.cpp
src/engine/platform/fds.cpp
src/engine/platform/tia.cpp
src/engine/platform/saa.cpp
src/engine/platform/amiga.cpp
src/engine/platform/msm5232.cpp
src/engine/platform/msm6258.cpp
src/engine/platform/msm6295.cpp
src/engine/platform/pcspkr.cpp
src/engine/platform/segapcm.cpp
src/engine/platform/qsound.cpp
src/engine/platform/x1_010.cpp
src/engine/platform/pokey.cpp
src/engine/platform/lynx.cpp
src/engine/platform/su.cpp
src/engine/platform/swan.cpp
src/engine/platform/t6w28.cpp
src/engine/platform/vb.cpp
src/engine/platform/vera.cpp
src/engine/platform/zxbeeper.cpp
src/engine/platform/zxbeeperquadtone.cpp
src/engine/platform/bubsyswsg.cpp
src/engine/platform/n163.cpp
src/engine/platform/pet.cpp
src/engine/platform/pokemini.cpp
src/engine/platform/pong.cpp
src/engine/platform/vic20.cpp
src/engine/platform/vrc6.cpp
src/engine/platform/es5506.cpp
src/engine/platform/scc.cpp
src/engine/platform/ymz280b.cpp
src/engine/platform/namcowsg.cpp
src/engine/platform/rf5c68.cpp
src/engine/platform/snes.cpp
src/engine/platform/k007232.cpp
src/engine/platform/ga20.cpp
src/engine/platform/sm8521.cpp
src/engine/platform/supervision.cpp
src/engine/platform/scvtone.cpp
src/engine/platform/pv1000.cpp
src/engine/platform/k053260.cpp
src/engine/platform/ted.cpp
src/engine/platform/c140.cpp
src/engine/platform/esfm.cpp
src/engine/platform/powernoise.cpp
src/engine/platform/dave.cpp
src/engine/platform/gbadma.cpp
src/engine/platform/gbaminmod.cpp
src/engine/platform/nds.cpp
src/engine/platform/bifurcator.cpp
src/engine/platform/sid2.cpp
src/engine/platform/sid3.cpp
src/engine/platform/pcmdac.cpp
src/engine/platform/dummy.cpp

src/engine/export/abstract.cpp
src/engine/export/amigaValidation.cpp
src/engine/export/sapr.cpp
src/engine/export/tiuna.cpp
src/engine/export/zsm.cpp

src/engine/effect/abstract.cpp
src/engine/effect/dummy.cpp
)

# if (USE_SNDFILE)
#   list(APPEND ENGINE_SOURCES src/engine/sfWrapper.cpp)
# endif()

list(APPEND ENGINE_SOURCES src/utfutils.cpp)
list(APPEND ENGINE_SOURCES src/engine/winStuff.cpp)

set(GUI_SOURCES
src/gui/plot_nolerp.cpp

src/gui/render.cpp
src/gui/render/abstract.cpp
src/gui/render/renderSoftware.cpp

src/gui/font_exo.cpp
src/gui/font_liberationSans.cpp
src/gui/font_mononoki.cpp
src/gui/font_plexMono.cpp
src/gui/font_plexSans.cpp
src/gui/font_plexSansJP.cpp
src/gui/font_plexSansKR.cpp
src/gui/font_proggyClean.cpp
src/gui/font_ptMono.cpp
src/gui/font_unifont.cpp
src/gui/font_icon.cpp
src/gui/font_furicon.cpp
src/gui/fonts.cpp
src/gui/fontzlib.cpp

src/gui/image_icon.cpp
src/gui/image.cpp

src/gui/debug.cpp
src/gui/fileDialog.cpp

src/gui/intConst.cpp
src/gui/guiConst.cpp

src/gui/about.cpp
src/gui/channels.cpp
src/gui/chanOsc.cpp
src/gui/clock.cpp
src/gui/compatFlags.cpp
src/gui/csPlayer.cpp
src/gui/cursor.cpp
src/gui/dataList.cpp
src/gui/debugWindow.cpp
src/gui/doAction.cpp
src/gui/editing.cpp
src/gui/editControls.cpp
src/gui/effectList.cpp
src/gui/exportOptions.cpp
src/gui/findReplace.cpp
src/gui/fmPreview.cpp
src/gui/gradient.cpp
src/gui/grooves.cpp
src/gui/insEdit.cpp
src/gui/log.cpp
src/gui/memory.cpp
src/gui/mixer.cpp
src/gui/midiMap.cpp
src/gui/newSong.cpp
src/gui/commandPalette.cpp
src/gui/orders.cpp
src/gui/osc.cpp
src/gui/patManager.cpp
src/gui/pattern.cpp
src/gui/piano.cpp
src/gui/presets.cpp
src/gui/regView.cpp
src/gui/sampleEdit.cpp
src/gui/scaling.cpp
src/gui/settings.cpp
src/gui/songInfo.cpp
src/gui/songNotes.cpp
src/gui/speed.cpp
src/gui/spoiler.cpp
src/gui/stats.cpp
src/gui/subSongs.cpp
src/gui/sysConf.cpp
src/gui/sysEx.cpp
src/gui/sysManager.cpp
src/gui/sysMiscInfo.cpp
src/gui/sysPicker.cpp
src/gui/tutorial.cpp
src/gui/userPresets.cpp
src/gui/util.cpp
src/gui/waveEdit.cpp
src/gui/volMeter.cpp
src/gui/xyOsc.cpp
src/gui/gui.cpp
)

list(APPEND DEPENDENCIES_DEFINES NO_INTRO)

set(USED_SOURCES ${ENGINE_SOURCES} ${AUDIO_SOURCES} ${GUI_SOURCES})

message(STATUS "Building headless")

list(APPEND DEPENDENCIES_LIBRARIES shlwapi)
if (NOT MSVC)
  list(APPEND DEPENDENCIES_LIBRARIES -static)
endif()

if (NOT MSVC)
  set(WARNING_FLAGS -Wall -Wextra -Wno-unused-parameter)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0.0)
      # nothing
    else()
      list(APPEND WARNING_FLAGS -Wno-cast-function-type)
    endif()
  endif()
  if (WARNINGS_ARE_ERRORS)
    list(APPEND WARNING_FLAGS -Werror)
  endif()
else()
  add_compile_options("/utf-8")
  set(WARNING_FLAGS /W2 /D_CRT_SECURE_NO_WARNINGS)
  list(APPEND WARNING_FLAGS
    /wd4244 # implicit type conversions
    /wd4305 # truncations
    /wd4309 # truncations of constant values
  )
  if (WARNINGS_ARE_ERRORS)
    list(APPEND WARNING_FLAGS /WX)
  endif()
endif()

# Nicer but cannot be narrowed down to just C++
# target_compile_options(furnace PRIVATE ${WARNING_FLAGS})
string(REPLACE ";" " " WARNING_FLAGS_STRING "${WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS_STRING}")
if (WARNINGS_ARE_ERRORS)
  message(STATUS
    "Treating all warnings in furnace's C++ code as errors! "
    "Please report any errors you encounter on the bug tracker."
  )
endif()

add_library(${FURNACE} STATIC ${USED_SOURCES})
target_include_directories(${FURNACE} PUBLIC ${DEPENDENCIES_INCLUDE_DIRS})
target_compile_options(${FURNACE} PUBLIC ${DEPENDENCIES_COMPILE_OPTIONS})
target_link_libraries(${FURNACE} PUBLIC ${DEPENDENCIES_LIBRARIES})
target_compile_definitions(${FURNACE} PUBLIC ${DEPENDENCIES_DEFINES})
