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

plugins = [
  'arctican-function',
  'arctican-pilgrim',
  'dexed',
  ]

foreach plugin : plugins

  subdir (plugin)

  plugin_lv2_lib = shared_library (
    plugin,
    name_prefix: '',
    sources: plugin_src,
    dependencies: lv2_deps,
    include_directories: [
      include_directories (plugin + '/source'),
      plugin_include_dirs,
      ],
    cpp_args: lv2_cflags,
    link_args: plugin_ldflags,
    link_with: juce_lib,
    install: true,
    install_dir: lv2dir / plugin + '.lv2',
    )

  plugin_ttl = custom_target (
    plugin + '-ttl',
    output: [
      plugin + '.lv2',
      ],
    input: plugin_lv2_lib,
    command: [
      'rm', '-rf',
      meson.current_build_dir () / plugin + '.lv2', '&&',
      'mkdir', meson.current_build_dir () / plugin + '.lv2',
      '&&',
      'cd', meson.current_build_dir () / plugin + '.lv2',
      '&&',
      lv2_ttl_generator, plugin_lv2_lib.full_path (),
      ],
    install: true,
    install_dir: lv2dir,
    )

  plugin_vst_lib = shared_library (
    plugin + '-vst',
    name_prefix: '',
    sources: plugin_src,
    dependencies: vst_deps,
    include_directories: [
      include_directories (plugin + '/source'),
      plugin_include_dirs,
      ],
    cpp_args: vst_cflags,
    link_args: plugin_ldflags,
    link_with: juce_lib,
    install: true,
    install_dir: vstdir,
    )
endforeach
