/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#ifndef __MAINHOSTWINDOW_JUCEHEADER__
#define __MAINHOSTWINDOW_JUCEHEADER__

#include "FilterGraph.h"
#include "GraphEditorPanel.h"


//==============================================================================
namespace CommandIDs
{
    static const int open                   = 0x30000;
    static const int save                   = 0x30001;
    static const int saveAs                 = 0x30002;
    static const int showPluginListEditor   = 0x30100;
}

//==============================================================================
/**
*/
class MainHostWindow    : public GraphDocumentComponent,
                          public MenuBarModel,
                          public ApplicationCommandTarget,
                          public ChangeListener,
                          public FileDragAndDropTarget
{
public:
    //==============================================================================
    MainHostWindow (AudioPluginFormatManager& formatManager, FilterGraph& graph, ApplicationProperties& appProperties);
    ~MainHostWindow();

    //==============================================================================
    void changeListenerCallback (ChangeBroadcaster*);

    bool isInterestedInFileDrag (const StringArray& files);
    void fileDragEnter (const StringArray& files, int, int);
    void fileDragMove (const StringArray& files, int, int);
    void fileDragExit (const StringArray& files);
    void filesDropped (const StringArray& files, int, int);

    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName);
    void menuItemSelected (int menuItemID, int topLevelMenuIndex);
    ApplicationCommandTarget* getNextCommandTarget();
    void getAllCommands (Array <CommandID>& commands);
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result);
    bool perform (const InvocationInfo& info);

    void addPluginsToMenu (PopupMenu& m) const;
    const PluginDescription* getChosenType (const int menuID) const;

private:
    //==============================================================================
    AudioPluginFormatManager& formatManager;

    KnownPluginList knownPluginList;
    KnownPluginList::SortMethod pluginSortMethod;

    class PluginListWindow;
    ScopedPointer <PluginListWindow> pluginListWindow;

    ApplicationCommandManager commandManager;
    ApplicationProperties& appProperties;
    LookAndFeel_V3 lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainHostWindow)
};


#endif   // __MAINHOSTWINDOW_JUCEHEADER__
