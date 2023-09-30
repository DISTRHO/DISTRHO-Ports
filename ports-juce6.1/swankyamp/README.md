# Swanky Amp

Swanky Amp is a tube amplifier simulation DSP plugin which aims to capture the details in the dynamics of tube amplifiers.

You can find the installers on the website:

<https://www.resonantdsp.com/swankyamp/>

![Swanky Amp](https://www.resonantdsp.com/images/swanky-amp.jpg)

## Demo

[![Swanky Amp Demo](https://img.youtube.com/vi/pz3v87VMQfk/0.jpg)](https://www.youtube.com/watch?v=pz3v87VMQfk)

## Usage

Load the plugin into your favorite VST host or DAW (into a mono or stereo track), and have fun!

Hover over a parameter to get a tooltip giving you information on how to use it.

In general, you will want to first gain stage, then set the drive according to your needs, and then fine tune.

## The model

The model was developed by running finite-difference simulation methods (spice), storing the outputs, developing empirical models using a mix of python and C++, and then fitting the model paramters to the simulation results. That code is very experimental and outside the scope of this repository.

Some more information about the process can be found here:

<https://www.resonantdsp.com/blog/tube-emulation/tube-emulation-4.html>

## Building

The DSP is primarily written in [FAUST](https://faust.grame.fr/), whereas the UI is written in C++ using the [JUCE](https://www.juce.com) toolkit.

This repository includes code to:

* Build the VST3 or AAX using Visual Studio 2019
* Build the AU using Xcode
* Create the Windows installer using [WiX](https://wixtoolset.org/)
* Create the Mac installers using a drag-and-drop DMG image.

### For Windows

You will need the following softare to compile the plugin for windows:

* Visual Studio 2019
* The VST3 SDK
* The JUCE SDK

The project is configured assuming the VST3 SDK is found at: `C:\SDKs\VST_SDK\VST3_SDK\`, and that the JUCE SDK is found at `C:\JUCE\modules`. This can be changed in the VS solution files.

* Open the VS solution file at: `Builds\VisualStudio2019\SwankyAmp.sln`
* Change the target in VS to the x64 Release.
* Build the VST3 target.
* Move the VST3 to your VST3 system directory (usually `C:\Program Files\Common Files\VST3`)

To build the installer:

* Move into the `package` directory
* Run `bash build-win.sh` (probably want to do that using WSL)

### For Mac

To build the installer:

* Open the project in Xcode
* Choose the desired solution (VST3 or AU) in the top menu
* Re-open the menu and choose "Edit Scheme"
* Change the build target to "Release"
* From the project room in a terminal:
  `bash build-mach.sh`
* When you are prompted to edit the images:
  * Mount the images
  * Open the mounted images
  * Right click in the window, choose "Show View Options"
  * Choose the background image (to show hidden files "cmd + shift + .")
  * Re-size the window and place the icons appropriately
* Unmount the images and press enter to finish the process

### Testing

Only available with VisualStudio at the moment.

* Open the solution
* Build the `SwanyAmp_SharedCode` project
* Open the `SwankyAmp_Tests` project
* Build the Debug version
* Run `Builds\VisualStudio\x64\Debug\SwankyAmp.exe`

## Change log

Version 1.4.0:

* rework staging in pre-amp: move pre-amp into FAUST and use multiplexing to mix stages
* rework low cut: increase range, add non-linearity
* rework state loading, convert on XML load and avoid making changes to memory in the audio thread (instead use a flag to indicate state changed)
* add vst ID registered with Steinberg to avoid registry problems
* review the plugin state reset and loading to avoid issues
* fix dmg installer issue for older macos releases

Version 1.3.1:

* fix issue where processor thread calls editor and can hang the audio for a bit
  * processor sets a flag when the state is changed
  * the editor monitors that on a timer
  * eventually should implement a better message queue / state system
* fix issue where pre amp drive would go too low and start getting digital artifacts
* improve pre amp drive range mapping

Version 1.3.0:

* re-worked the preset manager to fix issues with re-loading preset names
* added an AC30-like tone stack with mids control

Version 1.2.0:

* re-wokred drive ranges
* re-worked sag slightly
* use sinh transformation to make central range more sensitive
* changed windows installer to MSI
* changes to the layout

Version 1.1.0:

* Added ability to blend the tone stacks

Version 1.0.0:

* add Marshall-like tone stack
* reduce default power amp drive

Version 0.10.2:

* rework sag onset

Version 0.10.1:

* preset name will restore correctly after re-loading the plugin host
* pre amp drive no longer affects the power amp drive
* the power amp drive is easier to tune, and has an improved range
* the sag range has been improved
* the overall level normalization has been improved

Version 0.10.0:

* dynamic cabinet
* cabinet rework
* using a new tone stack provided by Dave Clark
* rework of dsp functionality: each part of the amp is now its own class, compiled in a separate project to the header files
* possibly fixed issue with rumble building up over time
* preset save icon now un-greys when creating a new preset or renaming a preset
* improved version string parsing

Version 0.9.0:

* fix bug that would cause audio level to decrease and vanish over time
* preset management can now be done fully from the directory
* preset names are now picked up from file names
* preset numbers for re-ordering are handled in a more intuitive way
* changing the preset name will now prompt for making a new preset or renaming
* small improvements to pre amp model and filtering
* small stability improvements to pre amp

Version 0.8.1:

* add mac support

Version 0.8.0:

* add variance to pre amp stages
* change preset management to directory sort order
* add button to open preset directory
* new presets are discovered on start
* no need to save preset to preserve order

Version 0.7.3:

* renamed to swanky amp
* allow reordering presets using preset names
* fixed possible issue with preset chaning causing bad state
* fixed issue with saving presets

Version 0.7.2:

* improved preset management: can now re-order, rename and remove presets from a single file `presetMaster.xml`
* added preset navigation buttons

Version 0.7.1:

* added decimal places to knob values when adjusting
* improved knob sensitivity

Version 0.7.0:

* improve filtering
* improve power amp drive and sag

Version 0.6.2:

* add tooltips

Version 0.6.1:

* fix multiple issues with preset management

Version 0.6.0:

* imporved preset management: save and remove icons, icons are disabled when the action is unavailable
* added version number to the bottom right of the UI

Version 0.5.3:

* tweaked sag ranged based on power amp drive to get more steady level
* tweaked presets accordingly

Version 0.5.2:

* fix pre amp drive calibration so that it doesn't get louder at higher levels
* updated presets to accomodate change

Version 0.5.1:

* modify tone stack so that the upper lows stay cut when mids change
* additional mids boost is now narrower

Version 0.5.0:

* preset management
* reworked sag model
* imporved high gain behaviour
* minor pass on the UI

Version 0.4.2:

* Reworked the sag parameter range to increase useable range and allow finer control
* Moved the `filter` control from the Staging section to the Pre Amp section and renamed it `low cut` to make it more intuitive

Version 0.4.1:

* The pre amp tight control is now easier to work with as it automatically adjusts based on pre amp drive to a suitable value
* Likewise for the sag control, based on the power amp drive.

Version 0.4.0:

* Introduced voltage sag
* Small changes to the default power amp model to accomodate the voltage sag
* Overall the new power amp model is a better representation of the underlying amp

Version 0.3.0:

* New cabinet model is a more accurate depiction of a 4x12 cabinet into a dynamic mic.
* New cabinet controls to get more variation out of the final sound.
* Touch controls re-wired into tight controls.

Version 0.2.1:

* Added some texturing to the dials and buttons
* Technical changes to support further UI developement

Version 0.2.0:

* Add grit control to pre amp

Version 0.1.0:

* Built a proper UI
* Improved the parameter ranges, especially the drive
* Simplified the drive tone control
* Reduced the mids control slightly

Version 0.0.6:

* Rework the power amp again, capture slow dynamics
* In particular capturing bias drift contributing to distorted compression
* Improvements transfered over also to the pre amp section
* Some of the dsp code was streamlined to be more efficient

Version 0.0.5:

* Reworked tone stack to behave more like a normal amp
* Don't normalize tone stack levels (pushing up levels will increase distortion)

Version 0.0.4:

* Stability fixes
* Minor tweaks to power amp model
* Improvement to the pre amp model

Version 0.0.3:

* Added a parameter smooth cross over distortion
* Adjusted drive range for less spinal tap level 11
* Fixed issue with gain stage parameter not working as intended

Version 0.0.2:

* The power amplifier model is re-written and is a better representation of the simulation.
* The perceived volume of the plugin is more invariant to changes in the drive parameters.
* All model fits have been improved, default parameters should better ressemble the simulated circuits.
* Parameter ranges have been refined to give better control over the distortion sound.
* The cabinet mix has been made into a toggle for clarity.

## JUCE usage in code

* Following the JUCE guidelines loosely, with the notable change that the code uses tabs specifically to avoid visual indentation.
* Components should have sensible defaults so that a default conctructor can be provided, otherwise the no copy convention causes problems.
* The Look and Feel (LAF) contains defintions that can change on-the-fly (mainly colours). Default dimensions should be defined in component classes, as those don't need to be looked-up at each paint call.

