# Copyright (C) 2020 Alexandros Theodotou <alex at zrythm dot org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

juce_srcs = [
  'source/modules/juce_audio_basics/juce_audio_basics.cpp',
  'source/modules/juce_audio_devices/juce_audio_devices.cpp',
  'source/modules/juce_audio_formats/juce_audio_formats.cpp',
  'source/modules/juce_audio_processors/juce_audio_processors.cpp',
  'source/modules/juce_audio_utils/juce_audio_utils.cpp',
  'source/modules/juce_core/juce_core.cpp',
  'source/modules/juce_cryptography/juce_cryptography.cpp',
  'source/modules/juce_data_structures/juce_data_structures.cpp',
  'source/modules/juce_dsp/juce_dsp.cpp',
  'source/modules/juce_events/juce_events.cpp',
  ]

if not get_option ('linux-embed')
  juce_srcs = [
    'source/modules/juce_graphics/juce_graphics.cpp',
    'source/modules/juce_gui_basics/juce_gui_basics.cpp',
    'source/modules/juce_gui_extra/juce_gui_extra.cpp',
    'source/modules/juce_opengl/juce_opengl.cpp'
    ]
endif

juce_inc = [
  include_directories ('build-juce'),
  include_directories ('source'),
  include_directories ('source' / 'modules'),
  ]

juce_lib = static_library (
  'juce',
  sources: juce_srcs,
  dependencies: lib_deps,
  include_directories: juce_inc,
  cpp_args: lib_cflags,
  pic: true,
  install: false,
  )
