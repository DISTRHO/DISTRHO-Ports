
--=======================================================================================--

function make_library_project(name)
  project.name    = name
  project.bindir  = "../.."
  project.libdir  = project.bindir
  project.configs = { "Release", "Debug" }

  package = newpackage()
  package.name = project.name
  package.kind = "lib"
  package.language = "c++"

  package.target       = project.name
  package.objdir       = "intermediate"
  package.defines      = {}
  package.buildoptions = { "-fPIC", "-DPIC", os.getenv("CXXFLAGS") }

  package.config["Release"].target       = project.name
  package.config["Release"].objdir       = "intermediate/Release"
  package.config["Release"].defines      = { "NDEBUG=1" }
  package.config["Release"].buildoptions = { "-O3", "-ffast-math", "-fomit-frame-pointer", "-mtune=generic", "-msse", "-msse2", "-mfpmath=sse", "-fvisibility=hidden", "-fvisibility-inlines-hidden" }

  if (not macosx) then
    package.config["Release"].buildoptions = { package.config["Release"].buildoptions, "-fdata-sections", "-ffunction-sections" }
  end

  package.config["Debug"].target       = project.name .. "_debug"
  package.config["Debug"].objdir       = "intermediate/Debug"
  package.config["Debug"].defines      = { "DEBUG=1", "_DEBUG=1" }
  package.config["Debug"].buildoptions = { "-O0", "-ggdb" }

  if (windows) then
    package.defines      = { "WINDOWS=1" }
    package.buildoptions = { package.buildoptions, "-fpermissive", "-std=c++0x" }
  elseif (macosx) then
    package.defines      = { "MAC=1" }
    package.buildoptions = { package.buildoptions, "-ObjC++" }
  else
    package.defines      = { "LINUX=1" }
    package.buildoptions = { package.buildoptions, "`pkg-config --cflags freetype2 x11 xext`", "-std=c++0x" }
  end

  return package
end

--=======================================================================================--

function make_plugin_project(name, spec)
  if (spec == "LADSPA") then
    project.name   = name
    project.bindir = "../../../bin/ladspa"
  elseif (spec == "DSSI") then
    project.name   = name
    project.bindir = "../../../bin/dssi"
  elseif (spec == "LV2") then
    project.name   = name .. ".lv2/" .. name
    project.bindir = "../../../bin/lv2"
  elseif (spec == "VST") then
    project.name   = name
    project.bindir = "../../../bin/vst"
  end

  project.libdir  = project.bindir
  project.configs = { "Release", "Debug" }

  package = newpackage()
  package.name = project.name
  package.kind = "dll"
  package.language = "c++"

  if (spec == "LADSPA") then
    package.defines = { "DISTRHO_PLUGIN_TARGET_LADSPA=1" }
  elseif (spec == "DSSI") then
    package.defines = { "DISTRHO_PLUGIN_TARGET_DSSI=1" }
  elseif (spec == "LV2") then
    package.defines = { "DISTRHO_PLUGIN_TARGET_LV2=1", "JucePlugin_Build_AU=0", "JucePlugin_Build_LV2=1", "JucePlugin_Build_RTAS=0", "JucePlugin_Build_VST=0", "JucePlugin_Build_Standalone=0" }
  elseif (spec == "VST") then
    package.defines = { "DISTRHO_PLUGIN_TARGET_VST=1", "JucePlugin_Build_AU=0", "JucePlugin_Build_LV2=0", "JucePlugin_Build_RTAS=0", "JucePlugin_Build_VST=1", "JucePlugin_Build_Standalone=0" }
  end

  package.target       = project.name
  package.targetprefix = ""
  package.objdir       = "intermediate"
  package.buildoptions = { os.getenv("CXXFLAGS") }
  package.links        = {}
  package.linkoptions  = { os.getenv("LDFLAGS") }

  package.config["Release"].target       = project.name
  package.config["Release"].objdir       = "intermediate/Release"
  package.config["Release"].defines      = { "NDEBUG=1", "CONFIGURATION=\"Release\"" }
  package.config["Release"].buildoptions = { "-O3", "-ffast-math", "-fomit-frame-pointer", "-mtune=generic", "-msse", "-msse2", "-mfpmath=sse", "-fvisibility=hidden", "-fvisibility-inlines-hidden" }
  package.config["Release"].links        = {}

  if (not macosx) then
    package.linkoptions                    = { package.linkoptions, "-Wl,--no-undefined" }
    package.config["Release"].buildoptions = { package.config["Release"].buildoptions, "-fdata-sections", "-ffunction-sections" }
    package.config["Release"].linkoptions  = { "-Wl,--gc-sections", "-Wl,--strip-all" }
  end

  package.config["Debug"].target       = project.name .. "_debug"
  package.config["Debug"].objdir       = "intermediate/Debug"
  package.config["Debug"].defines      = { "DEBUG=1", "_DEBUG=1", "CONFIGURATION=\"Debug\"" }
  package.config["Debug"].buildoptions = { "-O0", "-ggdb" }
  package.config["Debug"].links        = {}

  package.includepaths = {
    "../source"
  }

  package.libpaths = {
    "../../../libs"
  }

  if (windows) then
    package.defines      = { package.defines, "WINDOWS=1", "BINTYPE=\"Windows-" .. spec .. "\"" }
    package.buildoptions = { package.buildoptions, "-fpermissive", "-std=c++0x" }
  elseif (macosx) then
    package.defines      = { package.defines, "MAC=1", "BINTYPE=\"Mac-" .. spec .. "\"" }
    package.buildoptions = { package.buildoptions, "-ObjC++" }
    package.linkoptions  = { package.linkoptions, "-dynamiclib" }
    package.targetextension = "dylib"
  else
    package.defines      = { package.defines, "LINUX=1", "BINTYPE=\"Linux-" .. spec .. "\"" }
    package.buildoptions = { package.buildoptions, "-std=c++0x" }
  end

  return package
end

--=======================================================================================--

function make_distrho_ladspa_project(name)
  package = make_plugin_project(name, "LADSPA")

  package.includepaths = {
    package.includepaths,
    "../../../libs/distrho"
  }

  return package
end

function make_distrho_dssi_project(name)
  package = make_plugin_project(name, "DSSI")

  package.includepaths = {
    package.includepaths,
    "../../../libs/distrho"
  }

  return package
end

function make_distrho_dssi_ui_project(name)
  package = make_plugin_project(name, "DSSI")

  project.name = project.name .. "/" .. project.name .. "_gl"
  package.kind = "exe"

  package.buildoptions = { package.buildoptions, "`pkg-config --cflags liblo`" }
  package.linkoptions  = { package.linkoptions, "`pkg-config --libs liblo`" }

  package.config["Release"].links  = { "dgl" }
  package.config["Release"].target = project.name

  package.config["Debug"].links  = { "dgl_debug" }
  package.config["Debug"].target = project.name .. "Debug"

  package.includepaths = {
    package.includepaths,
    "../../../libs/distrho",
    "../../../libs/dgl"
  }

  if (windows) then
    package.links       = { "opengl32", "gdi32" }
  elseif (macosx) then
    package.linkoptions = { package.linkoptions, "-framework OpenGL", "-framework Cocoa" }
  else
    package.linkoptions = { package.linkoptions, "`pkg-config --libs gl x11`" }
  end

  return package
end

function make_distrho_lv2_project(name)
  package = make_plugin_project(name, "LV2")

  package.includepaths = { package.includepaths, "../../../libs/distrho" }

  return package
end

function make_distrho_lv2_ui_project(name)
  package = make_plugin_project(name, "LV2")

  project.name = name .. ".lv2/" .. name .. "_ui"

  package.config["Release"].links  = { "dgl" }
  package.config["Release"].target = project.name

  package.config["Debug"].links  = { "dgl_debug" }
  package.config["Debug"].target = name .. ".lv2/" .. name .. "_debug_ui"

  package.includepaths = {
    package.includepaths,
    "../../../libs/distrho",
    "../../../libs/dgl"
  }

  if (windows) then
    package.links       = { "opengl32", "gdi32" }
  elseif (macosx) then
    package.linkoptions = { package.linkoptions, "-framework OpenGL", "-framework Cocoa" }
  else
    package.linkoptions = { package.linkoptions, "`pkg-config --libs gl x11`" }
  end

  return package
end

function make_distrho_lv2_combined_project(name)
  package = make_plugin_project(name, "LV2")

  package.config["Release"].links = { "dgl" }
  package.config["Debug"].links   = { "dgl_debug" }

  package.includepaths = {
    package.includepaths,
    "../../../libs/distrho",
    "../../../libs/dgl"
  }

  if (windows) then
    package.links       = { "opengl32", "gdi32" }
  elseif (macosx) then
    package.linkoptions = { package.linkoptions, "-framework OpenGL", "-framework Cocoa" }
  else
    package.linkoptions = { package.linkoptions, "`pkg-config --libs gl x11`" }
  end

  return package
end

function make_distrho_vst_project(name)
  package = make_plugin_project(name, "VST")

  package.config["Release"].links = { "dgl" }
  package.config["Debug"].links   = { "dgl_debug" }

  package.includepaths = {
    package.includepaths,
    "../../../libs/distrho",
    "../../../libs/dgl"
  }

  if (windows) then
    package.links       = { "opengl32", "gdi32" }
  elseif (macosx) then
    package.linkoptions = { package.linkoptions, "-framework OpenGL", "-framework Cocoa" }
  else
    package.linkoptions = { package.linkoptions, "`pkg-config --libs gl x11`" }
  end

  return package
end

--=======================================================================================--

function make_juce_lv2_project(name)
  package = make_plugin_project(name, "LV2")

  package.config["Release"].links = { "juce" }
  package.config["Debug"].links   = { "juce_debug" }

  package.includepaths = {
    package.includepaths,
    "../../../libs/juce/source",
    "../../../libs/drowaudio/source",
    "../../../libs/juced/source",
    "../../../libs/juce-plugin"
  }

  if (windows) then
    package.links       = { "comdlg32", "gdi32", "imm32", "ole32", "oleaut32", "shlwapi", "uuid", "version", "winmm", "wininet", "ws2_32" }
  elseif (macosx) then
    package.linkoptions = { package.linkoptions,
                            "-framework Accelerate", "-framework AudioToolbox", "-framework AudioUnit", "-framework Carbon", "-framework Cocoa",
                            "-framework CoreAudio", "-framework CoreMIDI", "-framework IOKit", "-framework QuartzCore", "-framework WebKit" }
  else
    package.links       = { "dl", "pthread", "rt" }
    package.linkoptions = { package.linkoptions, "`pkg-config --libs freetype2 x11 xext`" }
  end

  return package
end

function make_juce_vst_project(name)
  package = make_plugin_project(name, "VST")

  package.config["Release"].links = { "juce" }
  package.config["Debug"].links   = { "juce_debug" }

  package.includepaths = {
    package.includepaths,
    "../../../libs/juce/source",
    "../../../libs/drowaudio/source",
    "../../../libs/juced/source",
    "../../../libs/juce-plugin",
    "../../../sdks/vstsdk2.4"
  }

  if (windows) then
    package.links       = { "comdlg32", "gdi32", "imm32", "ole32", "oleaut32", "shlwapi", "uuid", "version", "winmm", "wininet", "ws2_32" }
  elseif (macosx) then
    package.linkoptions = { package.linkoptions,
                            "-framework Accelerate", "-framework AudioToolbox", "-framework AudioUnit", "-framework Carbon", "-framework Cocoa",
                            "-framework CoreAudio", "-framework CoreMIDI", "-framework IOKit", "-framework QuartzCore", "-framework WebKit" }
  else
    package.links       = { "dl", "pthread", "rt" }
    package.linkoptions = { package.linkoptions, "`pkg-config --libs freetype2 x11 xext`" }
  end

  return package
end

--=======================================================================================--
