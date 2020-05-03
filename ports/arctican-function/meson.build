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

arctican_function_src_lv2 = [
  'source' / 'PluginProcessor.cpp',
  '..' / '..' / 'libs' / 'juce-plugin' / 'JucePluginMain.cpp',
  ]

if not get_option ('linux-embed')
  arctican_function_src_lv2 += [
    'source' / 'PluginEditor.cpp',
    'source/Headers/UI/Knob.cpp',
    'source/Headers/UI/FancyButton.cpp',
    'source/Headers/Binary Data/Backgrounds/thefunctionbackground.cpp',
    'source/Headers/Binary Data/UI/button.cpp',
    'source/Headers/Binary Data/UI/knobs.cpp',
    ]
endif

arctican_function_lv2 = shared_library (
  'arctican-function',
  sources: arctican_function_src_lv2,
  dependencies: lv2_deps,
  include_directories: [
    include_directories ('source'),
    plugin_include_dirs,
    ],
  cpp_args: lv2_cflags,
  link_with: juce_lib,
  install: true,
  install_dir: lv2dir,
  )
