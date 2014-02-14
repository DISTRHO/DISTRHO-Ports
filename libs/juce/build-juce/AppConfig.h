/*
  ==============================================================================

   Build options for juce static library

  ==============================================================================
*/

#ifndef BUILD_JUCE_APPCONFIG_H_INCLUDED
#define BUILD_JUCE_APPCONFIG_H_INCLUDED

#define JUCE_MODULE_AVAILABLE_juce_audio_basics          1
#define JUCE_MODULE_AVAILABLE_juce_audio_devices         1
#define JUCE_MODULE_AVAILABLE_juce_audio_formats         1
#define JUCE_MODULE_AVAILABLE_juce_audio_processors      1
#define JUCE_MODULE_AVAILABLE_juce_audio_utils           1
#define JUCE_MODULE_AVAILABLE_juce_core                  1
#define JUCE_MODULE_AVAILABLE_juce_cryptography          0
#define JUCE_MODULE_AVAILABLE_juce_data_structures       1
#define JUCE_MODULE_AVAILABLE_juce_events                1
#define JUCE_MODULE_AVAILABLE_juce_graphics              1
#define JUCE_MODULE_AVAILABLE_juce_gui_basics            1
#define JUCE_MODULE_AVAILABLE_juce_gui_extra             1
#define JUCE_MODULE_AVAILABLE_juce_opengl                0
#define JUCE_MODULE_AVAILABLE_juce_video                 0

//=============================================================================
// juce_audio_basics

//=============================================================================
// juce_audio_devices

//=============================================================================
/** Config: JUCE_ASIO
    Enables ASIO audio devices (MS Windows only).
    Turning this on means that you'll need to have the Steinberg ASIO SDK installed
    on your Windows build machine.

    See the comments in the ASIOAudioIODevice class's header file for more
    info about this.
*/
#define JUCE_ASIO 0

/** Config: JUCE_WASAPI
    Enables WASAPI audio devices (Windows Vista and above).
*/
#define JUCE_WASAPI 0

/** Config: JUCE_DIRECTSOUND
    Enables DirectSound audio (MS Windows only).
*/
#define JUCE_DIRECTSOUND 0

/** Config: JUCE_ALSA
    Enables ALSA audio devices (Linux only).
*/
#define JUCE_ALSA 0

/** Config: JUCE_JACK
    Enables JACK audio devices (Linux only).
*/
#define JUCE_JACK 0

//=============================================================================
/** Config: JUCE_USE_CDREADER
    Enables the AudioCDReader class (on supported platforms).
*/
#define JUCE_USE_CDREADER 0

/** Config: JUCE_USE_CDBURNER
    Enables the AudioCDBurner class (on supported platforms).
*/
#define JUCE_USE_CDBURNER 0

//=============================================================================
// juce_audio_formats

//=============================================================================
/** Config: JUCE_USE_FLAC
    Enables the FLAC audio codec classes (available on all platforms).
    If your app doesn't need to read FLAC files, you might want to disable this to
    reduce the size of your codebase and build time.
*/
#define JUCE_USE_FLAC 0

/** Config: JUCE_USE_OGGVORBIS
    Enables the Ogg-Vorbis audio codec classes (available on all platforms).
    If your app doesn't need to read Ogg-Vorbis files, you might want to disable this to
    reduce the size of your codebase and build time.
*/
#define JUCE_USE_OGGVORBIS 0

/** Config: JUCE_USE_MP3AUDIOFORMAT
    Enables the software-based MP3AudioFormat class.
    IMPORTANT DISCLAIMER: By choosing to enable the JUCE_USE_MP3AUDIOFORMAT flag and to compile
    this MP3 code into your software, you do so AT YOUR OWN RISK! By doing so, you are agreeing
    that Raw Material Software is in no way responsible for any patent, copyright, or other
    legal issues that you may suffer as a result.

    The code in juce_MP3AudioFormat.cpp is NOT guaranteed to be free from infringements of 3rd-party
    intellectual property. If you wish to use it, please seek your own independent advice about the
    legality of doing so. If you are not willing to accept full responsibility for the consequences
    of using this code, then do not enable this setting.
*/
#define JUCE_USE_MP3AUDIOFORMAT 0

/** Config: JUCE_USE_LAME_AUDIO_FORMAT
    Enables the LameEncoderAudioFormat class.
*/
#define JUCE_USE_LAME_AUDIO_FORMAT 0

/** Config: JUCE_USE_WINDOWS_MEDIA_FORMAT
    Enables the Windows Media SDK codecs.
*/
#define JUCE_USE_WINDOWS_MEDIA_FORMAT 0

//=============================================================================
// juce_audio_processors

//=============================================================================
/** Config: JUCE_PLUGINHOST_VST
    Enables the VST audio plugin hosting classes. This requires the Steinberg VST SDK to be
    installed on your machine.

    @see VSTPluginFormat, AudioPluginFormat, AudioPluginFormatManager, JUCE_PLUGINHOST_AU
*/
#define JUCE_PLUGINHOST_VST 1
#define JUCE_USE_VSTSDK_2_4 1

/** Config: JUCE_PLUGINHOST_AU
    Enables the AudioUnit plugin hosting classes. This is Mac-only, of course.

    @see AudioUnitPluginFormat, AudioPluginFormat, AudioPluginFormatManager, JUCE_PLUGINHOST_VST
*/
#if MAC && AUDIO_UNIT_VERSION >= 1060
 #define JUCE_PLUGINHOST_AU 1
#else
 #define JUCE_PLUGINHOST_AU 0
#endif

#if LINUX
 #define JUCE_PLUGINHOST_LADSPA 1
#else
 #define JUCE_PLUGINHOST_LADSPA 0
#endif

//=============================================================================
// juce_core

//=============================================================================
/** Config: JUCE_FORCE_DEBUG

    Normally, JUCE_DEBUG is set to 1 or 0 based on compiler and project settings,
    but if you define this value, you can override this to force it to be true or false.
*/
#if DEBUG
 #define JUCE_FORCE_DEBUG 1
#else
 #define JUCE_FORCE_DEBUG 0
#endif

//=============================================================================
/** Config: JUCE_LOG_ASSERTIONS

    If this flag is enabled, the the jassert and jassertfalse macros will always use Logger::writeToLog()
    to write a message when an assertion happens.

    Enabling it will also leave this turned on in release builds. When it's disabled,
    however, the jassert and jassertfalse macros will not be compiled in a
    release build.

    @see jassert, jassertfalse, Logger
*/
#if DEBUG
 #define JUCE_LOG_ASSERTIONS 1
#else
 #define JUCE_LOG_ASSERTIONS 0
#endif

//=============================================================================
/** Config: JUCE_CHECK_MEMORY_LEAKS

    Enables a memory-leak check for certain objects when the app terminates. See the LeakedObjectDetector
    class and the JUCE_LEAK_DETECTOR macro for more details about enabling leak checking for specific classes.
*/
#if DEBUG
 #define JUCE_CHECK_MEMORY_LEAKS 1
#else
 #define JUCE_CHECK_MEMORY_LEAKS 0
#endif

//=============================================================================
/** Config: JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES

    In a Visual C++  build, this can be used to stop the required system libs being
    automatically added to the link stage.
*/
#define JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES 0

/*  Config: JUCE_INCLUDE_ZLIB_CODE
    This can be used to disable Juce's embedded 3rd-party zlib code.
    You might need to tweak this if you're linking to an external zlib library in your app,
    but for normal apps, this option should be left alone.

    If you disable this, you might also want to set a value for JUCE_ZLIB_INCLUDE_PATH, to
    specify the path where your zlib headers live.
*/
#define JUCE_INCLUDE_ZLIB_CODE 1

/*  Config: JUCE_CATCH_UNHANDLED_EXCEPTIONS
    If enabled, this will add some exception-catching code to forward unhandled exceptions
    to your JUCEApplication::unhandledException() callback.
*/
#define JUCE_CATCH_UNHANDLED_EXCEPTIONS 0

//=============================================================================
// juce_data_structures

//=============================================================================
// juce_events

//=============================================================================
// juce_graphics

//=============================================================================
/** Config: JUCE_USE_COREIMAGE_LOADER

    On OSX, enabling this flag means that the CoreImage codecs will be used to load
    PNG/JPEG/GIF files. It is enabled by default, but you may want to disable it if
    you'd rather use libpng, libjpeg, etc.
*/
#if MAC
 #define JUCE_USE_COREIMAGE_LOADER 1
#else
 #define JUCE_USE_COREIMAGE_LOADER 0
#endif

/** Config: JUCE_USE_DIRECTWRITE

    Enabling this flag means that DirectWrite will be used when available for font
    management and layout.
*/
#if WINDOWS
 #define JUCE_USE_DIRECTWRITE 1
#else
 #define JUCE_USE_DIRECTWRITE 0
#endif

#define JUCE_INCLUDE_PNGLIB_CODE 1
#define JUCE_INCLUDE_JPEGLIB_CODE 1

#if MAC
 #define USE_COREGRAPHICS_RENDERING 1
#else
 #define USE_COREGRAPHICS_RENDERING 0
#endif

//=============================================================================
// juce_gui_basics

//=============================================================================
/** Config: JUCE_ENABLE_REPAINT_DEBUGGING
    If this option is turned on, each area of the screen that gets repainted will
    flash in a random colour, so that you can see exactly which bits of your
    components are being drawn.
*/
#define JUCE_ENABLE_REPAINT_DEBUGGING 0

/** JUCE_USE_XINERAMA: Enables Xinerama multi-monitor support (Linux only).
    Unless you specifically want to disable this, it's best to leave this option turned on.
*/
#if LINUX
 #define JUCE_USE_XINERAMA 1
#else
 #define JUCE_USE_XINERAMA 0
#endif

/** Config: JUCE_USE_XSHM
    Enables X shared memory for faster rendering on Linux. This is best left turned on
    unless you have a good reason to disable it.
*/
#if LINUX
 #define JUCE_USE_XSHM 1
#else
 #define JUCE_USE_XSHM 0
#endif

/** Config: JUCE_USE_XRENDER
    Enables XRender to allow semi-transparent windowing on Linux.
*/
#define JUCE_USE_XRENDER 0

/** Config: JUCE_USE_XCURSOR
    Uses XCursor to allow ARGB cursor on Linux. This is best left turned on unless you have
    a good reason to disable it.
*/
#if LINUX
 #define JUCE_USE_XCURSOR 1
#else
 #define JUCE_USE_XCURSOR 0
#endif

//=============================================================================
// juce_gui_extra

//=============================================================================
/** Config: JUCE_WEB_BROWSER
    This lets you disable the WebBrowserComponent class (Mac and Windows).
    If you're not using any embedded web-pages, turning this off may reduce your code size.
*/
#define JUCE_WEB_BROWSER 1

//=============================================================================
// drowaudio

//=============================================================================
/** Config: DROWAUDIO_USE_FFTREAL
    Enables the FFTReal library. By default this is enabled except on the Mac
    where the Accelerate framework is preferred. However, if you do explicity
    enable this setting fftreal can be used for testing purposes.
 */
#if JUCE_MAC
 #define DROWAUDIO_USE_FFTREAL 0
#else
 #define DROWAUDIO_USE_FFTREAL 1
#endif

/** Config: DROWAUDIO_USE_SOUNDTOUCH
    Enables the SoundTouch library and the associated SoundTouch classes for
    independant pitch and tempo scaling. By default this is enabled.
 */
#define DROWAUDIO_USE_SOUNDTOUCH 1

/** Config: DROWAUDIO_USE_CURL
    Enables the cURL library and the associated network classes. By default
    this is enabled.
 */
#define DROWAUDIO_USE_CURL 0

//=============================================================================
// juced

//=============================================================================
/** Config: JUCE_LASH
        Enables LASH support on Linux.
        Not enabled by default.
*/
#define JUCE_LASH 0

/** Config: JUCE_USE_GLX
        Enable this under Linux to use GLX for fast openGL rendering with alpha
        compositing support over a composite manager (compiz / xcompmgr).
        Not enabled by default.
*/
#define JUCE_USE_GLX 0

/** Config: JUCE_SUPPORT_SQLITE
        Setting this allows the build to use SQLITE libraries for access a self-contained,
        serverless, zero-configuration, transactional SQL database engine.
        Not enabled by default.
*/
#define JUCE_SUPPORT_SQLITE 0

/** Config: JUCE_SUPPORT_SCRIPTING
        Setting this allows the build to use Angelscript library for using scripting
        inside the juce library itself
        Not enabled by default.
*/
#define JUCE_SUPPORT_SCRIPTING 0

/** Config: JUCETICE_INCLUDE_ANGELSCRIPT_CODE
 Enables direct inclusion of the angelscript library.
 Enabled by default.
*/
#define JUCETICE_INCLUDE_ANGELSCRIPT_CODE  0

/** Config: JUCETICE_INCLUDE_CURL_CODE
 Enables direct inclusion of curl.

 // Currently not available //
*/
#define JUCETICE_INCLUDE_CURL_CODE 0

#endif // BUILD_JUCE_APPCONFIG_H_INCLUDED
