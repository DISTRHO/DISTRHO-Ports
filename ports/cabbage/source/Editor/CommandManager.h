/*
  Copyright (C) 2013 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

#ifndef CABBAGE_COMMAND_H
#define CABBAGE_COMMAND_H

#include "../JuceHeader.h"
//==============================================================================
namespace CommandIDs
{
    static const int fileNew			= 1000;
    static const int fileOpen			= 1001;
    static const int fileSave			= 1002;
    static const int fileSaveAs			= 1003;
    static const int fileSaveAll		= 1004;
	static const int fileExportSynth	= 1005;
	static const int fileExportEffect	= 1006;
	static const int fileUpdateGUI		= 1007;
	static const int fileQuit			= 1008;
	static const int fileKeyboardShorts	= 1009;

	static const int editUndo			= 2011;
	static const int editRedo			= 2012;
	static const int editCut			= 2013;
	static const int editCopy			= 2014;
	static const int editPaste			= 2015;
	static const int editZoomIn			= 2016;
	static const int editZoomOut		= 2017;
	static const int whiteBackground	= 2018;
	static const int blackBackground	= 2019;
	static const int editToggleComments	= 2020;
	static const int insertFromRepo		= 2021;
	static const int addFromRepo		= 2022;
	static const int insertRecentEvent	= 2023;
	static const int openPythonEditor	= 2024;
	static const int viewCsoundHelp		= 4025;
	static const int viewCabbageHelp	= 4026;
	static const int viewSource			= 4027;
	static const int AudioSettings		= 4028;
	
	static const int startSession				= 3023;
	static const int commOrchUpdateInstrument	= 3024;
	static const int commOrchUpdateSingleLine	= 3025;
	static const int commOrchUpdateMultiLine	= 3026;
	static const int commScoUpdateMultiLine		= 3027;
	static const int commScoUpdateSingleLine	= 3028;
	static const int commOrcUpdateChannel		= 3029;
	


};

//==============================================================================
namespace CommandCategories
{
    static const char* const file = "File";
    static const char* const edit = "Edit";
	static const char* const command = "Command";
	static const char* const help = "View";
}

//==============================================================================

class CommandManager : public ApplicationCommandManager
{
public:

	CommandManager ();
    ~CommandManager ();

	juce_DeclareSingleton (CommandManager, true)

};

#endif
