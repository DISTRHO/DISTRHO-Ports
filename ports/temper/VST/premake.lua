
dofile("../../../scripts/make-project.lua")

package = make_juce_vst_project("Temper")

if (windows) then
  -- TODO
elseif (macosx) then
  -- TODO
else
  package.linkoptions = { package.linkoptions, "`pkg-config --libs gl`" }
end

package.files = {
  matchfiles (
    "../source/*.cpp",
    "../../../libs/juce-plugin/JucePluginMain.cpp"
  )
}
