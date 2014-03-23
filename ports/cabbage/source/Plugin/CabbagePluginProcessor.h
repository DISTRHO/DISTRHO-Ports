/*
  Copyright (C) 2009 Rory Walsh

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

#ifndef __PLUGINPROCESSOR_H_FE85D052__
#define __PLUGINPROCESSOR_H_FE85D052__

#include "../JuceHeader.h"
#include "../CabbageUtils.h"
#include "../CabbageGUIClass.h"
#include "../Editor/CodeEditor.h"
#include "../XYPadAutomation.h"
#include "../CabbageMessageSystem.h"
#include "../Soundfiler.h"
#include "CabbageGenericAudioProcessorEditor.h"
#include "../CabbageLookAndFeel.h"

#ifndef Cabbage_No_Csound
#include <csound.hpp>
#include "cwindow.h"
#include "../csPerfThread.hpp"
#endif

//#ifndef Cabbage_Build_Standalone
//#include "../Editor/CabbageEditorWindow.h"
//#endif

#define CABBAGE_VERSION "Cabbage v0.5.07 Alpha"

#define AUDIO_PLUGIN 1
#define EXTERNAL_PLUGIN 2
#define AUTOMATION_PLUGIN 3

#ifdef Cabbage_Build_Standalone
class CsoundCodeEditor;
#endif

extern CabbageLookAndFeel* lookAndFeel;
extern CabbageLookAndFeelBasic* lookAndFeelBasic;

//==============================================================================
// CabbagePluginAudioProcessor definition
//==============================================================================
class CabbagePluginAudioProcessor  : public AudioProcessor,
                                                                         public CabbageUtils,
                                                                         public ChangeBroadcaster,
                                                                         public Timer,
																		 public ActionBroadcaster,
																		 public ChangeListener
{
    //==============================================================================
protected:
       File csdFile;
		int masterCounter;
        String filename;
        String pluginName;
        bool csoundStatus;
        int csCompileResult;
        void timerCallback();
        String csoundOutput;
        void changeListenerCallback(ChangeBroadcaster *source);
        String changeMessageType;
        bool guiON;
        int currentLine;
		bool xyAutosCreated;
		bool updateTable;
		Array<int> tableNumbers;
		AudioSourceChannelInfo soundfilerChannelData;
		AudioPlayHead::CurrentPositionInfo hostInfo;
		int soundFileIndex;
		//ScopedPointer<FileLogger> fileLogger;


		File logFile;
		bool isAutomator;
		bool isNativeThreadRunning;

        //============== Csound related variables/methods ==============================
#ifndef Cabbage_No_Csound
        ScopedPointer<CSOUND_PARAMS> csoundParams;
		ScopedPointer<CsoundPerformanceThread> csoundPerfThread;
        PVSDATEXT* dataout;
        MYFLT cs_scale;
        ScopedPointer<Csound> csound;                           //Csound instance
        MYFLT *CSspin, *CSspout;        //Csound audio IO pointers
        int csndIndex;                          //Csound sample counter
		int csdKsmps;
		MYFLT *soundFilerTempVector;
        int CSCompResult;                       //result of Csound performKsmps
		controlChannelInfo_s* csoundChanList;
        int numCsoundChannels;          //number of Csound channels
        static void messageCallback(CSOUND *csound, int attr, const char *fmt, va_list args);  //message callback function
        int pos;
        //Csound API functions for deailing with midi input
        static int OpenMidiInputDevice(CSOUND * csnd, void **userData, const char *devName);
        static int OpenMidiOutputDevice(CSOUND * csnd, void **userData, const char *devName);
        static int ReadMidiData(CSOUND *csound, void *userData, unsigned char *mbuf, int nbytes);
        static int WriteMidiData(CSOUND *csound, void *userData, const unsigned char *mbuf, int nbytes);

		int getNumberCsoundOutChannels(){
			return csound->GetNchnls();
		}

		int getNumberCsoundInChannels(){
			//return csound->GetInNchnls();
		}
  
		int getCsoundSamplingRate(){
			return csound->GetSr();
		}

		int getCsoundKsmpsSize(){
			return csound->GetKsmps();
		}
#endif
		static void YieldCallback(void* data);
		void updateCabbageControls();
		void sendOutgoingMessagesToCsound();
		void sendAudioToCsoundFromSoundFilers(int numSamples);


        StringArray debugInfo;
		//basic classes that hold all information regarding GUI objects
		//guiLayoutControls are not used to send data to Csound, and don't show
		//as parameters in a host, guiCtrls do show are parameters, and can send
		//channel messages to Csound.
        Array<CabbageGUIClass, CriticalSection> guiLayoutCtrls;
        Array<CabbageGUIClass, CriticalSection> guiCtrls;
        String plantFlag, presetFlag;
        String debugMessage;
        StringArray debugMessageArray;
		String currentLineText;
		bool editorReOpened;
		OwnedArray<XYPadAutomation, CriticalSection> xyAutomation;
		void updateGUIControlsKsmps(int speed);
		int guiRefreshRate;
public:

    //==============================================================================

#if defined(Cabbage_Build_Standalone) || (Cabbage_Plugin_Host)
    CabbagePluginAudioProcessor(String inputfile, bool guiOnOff, int pluginType);
#else
        CabbagePluginAudioProcessor();
#endif
    ~CabbagePluginAudioProcessor();

    #ifdef Cabbage_Build_Standalone
	CsoundCodeEditor* codeEditor;
    #endif

	bool haveXYAutosBeenCreated(){
		return xyAutosCreated;
	}

	void setHaveXYAutoBeenCreated(bool val){
		xyAutosCreated = val;
	}

	double getTailLengthSeconds(void) const {
	return 1;
	}

	int performEntireScore();
	void reCompileCsound(File file);
	void setupNativePluginEditor();
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
    virtual void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;
    //==============================================================================
    const String getName() const;
    int getNumParameters();
    float getParameter (int index);
    void setParameter (int index, float newValue);
    const String getParameterName (int index);
    const String getParameterText (int index);
    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;
    bool acceptsMidi() const;
    bool producesMidi() const;
	Array<CabbagePatternMatrixStepData> patStepMatrix;
	StringArray patternNames;
	Array<CabbagePatternMatrixPfieldData> patPfieldMatrix;
	//Array<int> pField4, pField5, pField6, pField7;
	int noSteps;
	int noPatterns;
	int timeCounter;
	int beat;
	int patMatrixActive;
	float bpm;
    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);
    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
	const Array<double, CriticalSection> getTable(int tableNum);
	void createGUI(String source, bool refresh);
	MidiKeyboardState keyboardState;
	//midiBuffers
	MidiBuffer midiBuffer;
	MidiBuffer midiOutputBuffer;
	MidiBuffer ccBuffer;
	bool showMIDI;
	bool yieldCallbackBool;
	int yieldCounter;
	bool nativePluginEditor;
	CabbageMessageQueue messageQueue;
	OwnedArray<CabbageAudioSource, CriticalSection> audioSourcesArray;
	void addSoundfilerSource(String filename, StringArray channels);
	StringArray scoreEvents;
	int averageSampleIndex;
	float outputNo1;
	int pluginType;
	float automationAmp;
	int automationParamID;
	int pluginCalls, csoundCalls;


	//==============================================================================
        File getCsoundInputFile(){
                return csdFile;
        }

        inline String getCsoundInputFileText(){
        String ret="";
        #ifdef Cabbage_Build_Standalone
		if(codeEditor)
                ret = codeEditor->getAllText();
			else
				ret = csdFile.loadFileAsString();
        #endif
        return ret;
        }

        void updateCsoundFile(String text){
			//csdFile.replaceWithText(text);
        #ifdef Cabbage_Build_Standalone
			//codeEditor->textChanged = true;
			codeEditor->setAllText(text);
        #endif
        }
        
		bool hasTextChanged(){
		#ifdef Cabbage_Build_Standalone	
		if(codeEditor)
		return codeEditor->textChanged;	
		else return false;
		#else
		return false;
		#endif
		}
		
		void saveText(){
		#ifdef Cabbage_Build_Standalone
		codeEditor->textChanged = false;	
		#endif
		}
		
		void setOpcodeDirEnv(){
		#ifdef WIN32		
		//showMessage(getenv("OPCODE6DIR64"));
		String opcodeDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"\\CsoundPlugins";
		//showMessage(opcodeDir);
			if(File(opcodeDir).exists()){
			String env = "OPCODE6DIR64="+opcodeDir;
			_putenv(env.toUTF8().getAddress());
			//showMessage(getenv("OPCODE6DIR64"));
			}
		#endif
		}
		
		void highlightLine(String text){
        #ifdef Cabbage_Build_Standalone
		codeEditor->highlightLine(text);
        #endif
		}		
		
        String getDebugMessage(){
        return debugMessage;
        }

        void addDebugMessage(String message){
                debugMessageArray.add(message);;
        }

        StringArray getDebugMessageArray(){
        return debugMessageArray;
        }

        void clearDebugMessageArray(){
                debugMessageArray.clear();
        }

        int getCompileStatus(){
                return csCompileResult;
        }

        void clearDebugMessage(){
                debugMessage="";
        }

        void setPluginName(String name){
        pluginName = name;
        }

        String getPluginName(){
        return pluginName;
        }

		Array<Array <float > > tableArrays;

		Array<float> getTableArray(int index){
			return tableArrays[index];
		}

        void setMidiDebug(bool val){
                showMIDI=val;
        }
        bool getMidiDebug(){
                return showMIDI;
        }
        //======== log information about GUI controls ===============
        StringArray logGUIAttributes(CabbageGUIClass cAttr, String type){

                StringArray arr;
				/*
                arr.add(String("----------- ")+type+String(" -----------"));
                arr.add(String("Name:")+cAttr.getStringProp("name")+String(", Type:")+cAttr.getStringProp("type")+String(", Caption:")+cAttr.getStringProp("caption")+String(", RelToPlant:")+cAttr.getStringProp("reltoplant")+String(", Plant:")+cAttr.getStringProp("plant"));
                arr.add(String("PosX:")+String(cAttr.getNumProp("left"))+String(", PosY:")+String(cAttr.getNumProp("top"))+String(", Width:")+String(cAttr.getNumProp("width"))+String(", Height:")+String(cAttr.getNumProp("height")));
                */
				arr.add(String("   "));
                //Logger::writeToLog(String("----------- ")+type+String(" -----------"));
                //Logger::writeToLog(String("Name:")+cAttr.getStringProp("name")+String(", Type:")+cAttr.getStringProp("type")+String(", Caption:")+cAttr.getStringProp("caption")+String(", RelToPlant:")+cAttr.getStringProp("reltoplant")+String(", Plant:")+cAttr.getStringProp("plant"));
                //Logger::writeToLog(String("PosX:")+String(cAttr.getNumProp("left"))+String(", PosY:")+String(cAttr.getNumProp("top"))+String(", Width:")+String(cAttr.getNumProp("width"))+String(", Height:")+String(cAttr.getNumProp("height")));
                //Logger::writeToLog("   ");
                return arr;
        }
        inline bool getCsoundStatus(){
                return csoundStatus;
        }

        //===========================================================
        inline int getGUICtrlsSize(){
                return (int)guiCtrls.size();
        }

        inline int getGUILayoutCtrlsSize(){
                return (int)guiLayoutCtrls.size();
        }

        inline CabbageGUIClass &getGUILayoutCtrls(int index){
                return guiLayoutCtrls.getReference(index);
        }

        inline CabbageGUIClass &getGUICtrls(int index){
                return guiCtrls.getReference(index);
        }

        inline String getChangeMessageType(){
                return changeMessageType;
        }

        inline String getCsoundOutput(){
                return csoundOutput;
        }

        inline void setChangeMessageType(String text){
                changeMessageType = text;
        }

        inline int getCurrentLine(){
                return currentLine;
        }

        inline void setCurrentLine(int line){
                currentLine = line;
        }

        inline void setCurrentLineText(String lineText){
                currentLineText = lineText;
        }

        String getCurrentLineText(){
                return currentLineText;
        }

		void removeGUIComponent(int index, String type);

#ifndef Cabbage_No_Csound
        Csound* getCsound(){
                return csound;
        }

        CSOUND* getCsoundStruct(){
                return csound->GetCsound();
        }

        MYFLT getCSScale(){
                return cs_scale;
        }

        PVSDATEXT* getPVSDataOut(){
                return dataout;
        }

#endif
		void addLayoutCtrl(CabbageGUIClass cAttr){
		guiLayoutCtrls.add(cAttr);
		}

		void addGUICtrl(CabbageGUIClass cAttr){
		guiCtrls.add(cAttr);
		}


        bool isGuiEnabled(){
                return guiON;
        }

        void setGuiEnabled(bool val);

		void addXYAutomater(XYPadAutomation* xyAuto){
			xyAutomation.add(xyAuto);
		}

		XYPadAutomation* getXYAutomater(int index){
			return xyAutomation[index];
		}

		int getXYAutomaterSize(){
			return xyAutomation.size();
		}

		void removeXYAutomaters(){
			xyAutomation.clear();
		}

		bool silenceInProducesSilenceOut() const{
			return true;
			}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePluginAudioProcessor);

};

//pecial auotmation only plugin type. Does not output any audio.
#ifdef Cabbage_Host
class CabbagePluginAutomationProcessor : public CabbagePluginAudioProcessor
{
public:
	CabbagePluginAutomationProcessor(AudioProcessor* filter, String inputfile, int pluginType):
	CabbagePluginAudioProcessor(inputfile, false, pluginType)
	{}
	~CabbagePluginAutomationProcessor(){}


};
#endif
#endif  // __PLUGINPROCESSOR_H_FE85D052__
