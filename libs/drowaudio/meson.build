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

drowaudio_lib = static_library (
  'drowaudio',
  sources: [
    'source' / 'dRowAudio' / 'dRowAudio.cpp' ],
  dependencies: lib_deps,
  include_directories: [
    include_directories ('.'),
    include_directories ('build-drowaudio'),
    include_directories ('..' / 'juce' / 'source'),
    include_directories ('..' / 'juce' / 'source' / 'modules'),
    ],
  cpp_args: lib_cflags,
  pic: true,
  install: false,
  )
