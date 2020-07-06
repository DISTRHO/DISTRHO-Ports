
dofile("../../../scripts/make-project.lua")

package = make_library_project("juced")

package.includepaths = {
  ".",
  "../../juce-legacy/source",
  "../../juce-legacy/source/modules"
}

package.files = {
  matchfiles (
    "../source/juced.cpp"
  )
}
