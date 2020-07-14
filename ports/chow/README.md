# CHOW - a maximally truculent distortion effect 

[![Download Latest](https://img.shields.io/badge/download-latest-blue.svg)](https://github.com/jatinchowdhury18/CHOW/releases/latest)
[![Build Status](https://travis-ci.com/jatinchowdhury18/CHOW.svg?branch=master)](https://travis-ci.com/jatinchowdhury18/CHOW)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-brightgreen.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Definition
**truculent** adjective<br/>
tru·​cu·​lent | \ˈtrə-kyə-lənt also ˈtrü-  \
Definition of *truculent*<br/>
1 : aggressively self-assertive : BELLIGERENT<br/>
2 : scathingly harsh : VITRIOLIC<br/>
&nbsp;&nbsp;&nbsp;// *truculent* criticism<br/>
3 : feeling or displaying ferocity : CRUEL, SAVAGE<br/>
4 : DEADLY, DESTRUCTIVE<br/>

## Description
![Pic](https://raw.githubusercontent.com/jatinchowdhury18/CHOW/master/screenshot.PNG)

CHOW is a digital ditortion effect designed for maximum truculence, somewhere between a true half-wave rectifier and a noisy vintage compressor. Useful for mixing guitars, drums, even vocals when a heavily degraded sound is desired. Feel free to build and use for your own enjoyment!

CHOW is currently available in the following formats:
  - VST
  - VST3
  - AU
  - Standalone plugin

## Instructions For Building
If you would like to build CHOW for yourself, use the following steps, otherwise you can download the latest builds [here](https://github.com/jatinchowdhury18/CHOW/releases/latest).
- Install [Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)
- Run the following git commands:
  ```bash
  # Clone the repository
  git clone --recursive https://github.com/jatinchowdhury18/CHOW.git
  
  # Enter the repository
  cd CHOW
  
  # Initialize JUCE submodule
  git submodule update --init --recursive
  ```
- Navigate to "JUCE/extras/Projucer/Builds/"
- Build the Projucer using XCode (Mac), Visual Studio (Windows), or Makefile (Linux)
- From the Projucer, open CHOW.jucer
- From the "File" menu, open "Global Paths..."
  - Set "Path To JUCE" to "...\CHOW\JUCE"
  - Set "JUCE Modules" to "...\CHOW\JUCE\modules"
  - Set "VST (Legacy) SDK" to "...CHOW\VST2_SDK"
  - Close "Global Paths" window
- Select "Save and open in IDE"
- Build CHOW

## License
CHOW is freely distributable and is covered by the terms of the GNU licensing agreement. Enjoy!
