
dofile("../../../scripts/make-project.lua")

package = make_juce_lv2_project("Temper")

if (os.getenv("LINUX_EMBED")) then
package.files = {
  matchfiles (
    "../source/FaustUIBridge.cpp",
    "../source/BinaryData.cpp",
    "../source/PluginProcessor.cpp",
    "../../../libs/juce-plugin/JucePluginMain.cpp"
  )
}
else
if (windows) then
  package.linkoptions = { package.linkoptions, "-lopengl32" }
elseif (macosx) then
  package.linkoptions = { package.linkoptions, "-framework OpenGL" }
else
  package.linkoptions = { package.linkoptions, "`pkg-config --libs gl`" }
end
package.files = {
  matchfiles (
    "../source/*.cpp",
    "../../../libs/juce-plugin/JucePluginMain.cpp"
  )
}
end
