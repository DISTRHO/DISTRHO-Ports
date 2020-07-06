
dofile("../../../scripts/make-project.lua")

package = make_library_project("drowaudio")

package.includepaths = {
  ".",
  "../../juce-legacy/source",
  "../../juce-legacy/source/modules"
}

package.files = {
  matchfiles (
    "../source/dRowAudio/dRowAudio.cpp"
  )
}
