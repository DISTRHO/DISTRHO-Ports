
dofile("../../../scripts/make-project.lua")

package = make_library_project("dgl")

package.includepaths = {
  ".."
}

package.files = {
  matchfiles (
    "../src/*.cpp"
  )
}

if (macosx) then
  package.files = {
    package.files,
    "../src/pugl/pugl_osx.m.c",
    "../src/pugl/pugl_osx_extended.m.c"
  }
end
