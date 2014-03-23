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

#include "CabbagePluginProcessor.h"
#include "CabbagePluginEditor.h"
#include <iostream>


#define MAX_BUFFER_SIZE 1024

#define LOGGER 0

//these two lines may need to be copied to top part of csound.h
//#define int32 int
//#define uint32 unsigned int

CabbageLookAndFeel* lookAndFeel;
CabbageLookAndFeelBasic* lookAndFeelBasic;

//==============================================================================
// There are two different CabbagePluginAudioProcessor constructors. One for the
// standalone application and the other for the plugin library
//==============================================================================
//#ifdef Cabbage_Build_Standalone
#if defined(Cabbage_Build_Standalone) || defined(Cabbage_Plugin_Host)
//===========================================================
// STANDALONE - CONSTRUCTOR
//===========================================================
CabbagePluginAudioProcessor::CabbagePluginAudioProcessor(String inputfile, bool guiOnOff, int _pluginType)
:csoundStatus(false),
csdFile(File(inputfile)),
showMIDI(false),
csCompileResult(1),
changeMessageType(""),
guiON(false),
currentLine(-99),
noSteps(0),
noPatterns(0),
timeCounter(0),
beat(0),
bpm(120),
patMatrixActive(0),
masterCounter(0),
xyAutosCreated(false),
updateTable(false),
yieldCallbackBool(false),
yieldCounter(10),
isNativeThreadRunning(false),
soundFileIndex(0),
scoreEvents(),
nativePluginEditor(false),
averageSampleIndex(0),
pluginType(_pluginType),
automationAmp(0),
isAutomator(false),
automationParamID(-1),
debugMessage(""),
guiRefreshRate(20)
{
//suspendProcessing(true);
codeEditor = nullptr;
#ifdef Cabbage_Logger
logFile = File((appProperties->getCommonSettings(true)->getFile().getParentDirectory().getFullPathName()+"/CabbageLog.txt"));
fileLogger = new FileLogger(logFile, String("Cabbage Log.."));
Logger::setCurrentLogger(fileLogger);
#endif

//reset patMatrix. If this has more than one we know that
//pattern matrix object is being used
patStepMatrix.clear();

patPfieldMatrix.clear();
setPlayConfigDetails(2, 2, 44100, 512);

#ifndef Cabbage_No_Csound
//don't start of run Csound in edit mode
setOpcodeDirEnv();

csound = new Csound();

#ifdef CSOUND6
csound->SetHostImplementedMIDIIO(true);
#endif
csound->Reset();
//Logger::writeToLog(csound->GetEnv("OPCODEDIR64"));
#ifdef CSOUND5
csound->PreCompile(); 
#endif
csound->SetHostData(this);
csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
csound->SetExternalMidiReadCallback(ReadMidiData);
csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
csound->SetExternalMidiWriteCallback(WriteMidiData);

#ifndef Cabbage_Plugin_Host
if(!getPreference(appProperties, "UseCabbageIO")){
	csoundPerfThread = new CsoundPerformanceThread(csound);
	csoundPerfThread->SetProcessCallback(CabbagePluginAudioProcessor::YieldCallback, (void*)this);
}
#endif

if(pluginType==AUTOMATION_PLUGIN)
 isAutomator = true;

csoundChanList = NULL;
numCsoundChannels = 0;
csndIndex = 32;

//set up PVS struct
dataout = new PVSDATEXT;

if(inputfile.isNotEmpty()){
File(inputfile).setAsCurrentWorkingDirectory();
#ifdef CSOUND6
csoundParams = new CSOUND_PARAMS();
csoundParams->nchnls_override =2;
csound->SetParams(csoundParams);
#endif
csCompileResult = csound->Compile(const_cast<char*>(inputfile.toUTF8().getAddress()));

if(csCompileResult==0){

//send root directory path to Csound.	
	setPlayConfigDetails(getNumberCsoundOutChannels(),
						getNumberCsoundOutChannels(),
						getCsoundSamplingRate(),
						getCsoundKsmpsSize());

        //simple hack to allow tables to be set up correctly.
        csound->PerformKsmps();
        csound->SetScoreOffsetSeconds(0);
        csound->RewindScore();
		#ifdef WIN32
		csound->SetChannel("CSD_PATH", File(inputfile).getParentDirectory().getFullPathName().replace("\\", "\\\\").toUTF8().getAddress());	
		#else
		csound->SetChannel("CSD_PATH", File(inputfile).getParentDirectory().getFullPathName().toUTF8().getAddress());	
		#endif
        Logger::writeToLog("Csound compiled your file");
  
		//csound->SetYieldCallback(CabbagePluginAudioProcessor::yieldCallback);
        if(csound->GetSpout()==nullptr);
        CSspout = csound->GetSpout();
        CSspin  = csound->GetSpin();
        numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
        csndIndex = csound->GetKsmps();
		csdKsmps = csound->GetKsmps();
		soundFilerTempVector = new MYFLT[csdKsmps];
        cs_scale = csound->Get0dBFS();
        csoundStatus = true;
        debugMessageArray.add(CABBAGE_VERSION);
        debugMessageArray.add(String("\n"));
		this->setLatencySamples(csound->GetKsmps());
		updateHostDisplay();
	
}
else{
        Logger::writeToLog("Csound couldn't compile your file");
        csoundStatus=false;
        //debugMessage = "Csound did not compile correctly. Check for snytax errors by compiling with WinXound";
}
}
else
Logger::writeToLog("Welcome to Cabbage");

#endif
lookAndFeel = new CabbageLookAndFeel();
lookAndFeelBasic = new CabbageLookAndFeelBasic();
}
#else

//===========================================================
// PLUGIN - CONSTRUCTOR
//===========================================================
CabbagePluginAudioProcessor::CabbagePluginAudioProcessor():
csoundStatus(false),
showMIDI(false),
csCompileResult(1),
changeMessageType(""),
guiON(false),
currentLine(-99),
noSteps(0),
noPatterns(0),
timeCounter(0),
beat(0),
bpm(120),
patMatrixActive(0),
masterCounter(0),
xyAutosCreated(false),
updateTable(false),
yieldCallbackBool(false),
yieldCounter(10),
soundFileIndex(0),
nativePluginEditor(false),
averageSampleIndex(0)
{
//Cabbage plugins always try to load a csd file with the same name as the plugin library.
//Therefore we need to find the name of the library and append a '.csd' to it.
#ifdef MACOSX
String osxCSD = File::getSpecialLocation(File::currentApplicationFile).getFullPathName()+String("/Contents/")+File::getSpecialLocation(File::currentApplicationFile).getFileName();
File thisFile(osxCSD);
Logger::writeToLog("MACOSX defined OK");
#else
File thisFile(File::getSpecialLocation(File::currentExecutableFile));
#endif
csdFile = thisFile.withFileExtension(String(".csd")).getFullPathName();


Logger::writeToLog(File::getSpecialLocation(File::currentExecutableFile).getFullPathName());


if(csdFile.exists())
Logger::writeToLog("File exists:"+String(csdFile.getFullPathName()));
else
Logger::writeToLog("File doesn't exist"+String(csdFile.getFullPathName()));

File(csdFile.getFullPathName()).setAsCurrentWorkingDirectory();

//set logger
#ifdef Cabbage_Logger
logFile = File(File(csdFile.getFullPathName()).getParentDirectory().getFullPathName()+"/CabbageLog.txt");
fileLogger = new FileLogger(logFile, String("Cabbage Log.."));
Logger::setCurrentLogger(fileLogger);
#endif

setOpcodeDirEnv();

#ifndef Cabbage_No_Csound
csound = new Csound();


csound->SetHostImplementedMIDIIO(true);
//csound->Reset();
//csound->PreCompile();
csound->SetHostData(this);
midiOutputBuffer.clear();
//for host midi to get sent to Csound, don't need this for standalone
//but might use it in the future for midi mapping to controls
csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
csound->SetExternalMidiReadCallback(ReadMidiData);
csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
csound->SetExternalMidiWriteCallback(WriteMidiData);


patStepMatrix.clear();
patternNames.clear();
patPfieldMatrix.clear();

csoundChanList = NULL;
numCsoundChannels = 0;
csndIndex = 32;
startTimer(20);
#ifdef CSOUND6
csoundParams = new CSOUND_PARAMS();
csoundParams->nchnls_override =2;
csound->SetParams(csoundParams);
#endif

csdFile.setAsCurrentWorkingDirectory();

csCompileResult = csound->Compile(const_cast<char*>(csdFile.getFullPathName().toUTF8().getAddress()));
csdFile.setAsCurrentWorkingDirectory();
if(csCompileResult==0){
	
	Logger::writeToLog("compiled Ok");
		keyboardState.allNotesOff(0);
		keyboardState.reset();
        //simple hack to allow tables to be set up correctly.
        csound->PerformKsmps();
        csound->SetScoreOffsetSeconds(0);
        csound->RewindScore();
        //set up PVS struct
		dataout = new PVSDATEXT;
		csdKsmps = csound->GetKsmps();
        soundFilerTempVector = new MYFLT[csdKsmps];
		if(csound->GetSpout()==nullptr);
        CSspout = csound->GetSpout();
        CSspin  = csound->GetSpin();
        cs_scale = csound->Get0dBFS();
        numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
        csndIndex = csound->GetKsmps();
		this->setLatencySamples(csound->GetKsmps());
		updateHostDisplay();
		//soundFilerVector = new MYFLT[csdKsmps];
        csoundStatus = true;
        debugMessageArray.add(VERSION);
        debugMessageArray.add(String("\n"));
		#ifdef WIN32
		csound->SetChannel("CSD_PATH", File(csdFile).getParentDirectory().getFullPathName().replace("\\", "\\\\").toUTF8().getAddress());	
		#else
		csound->SetChannel("CSD_PATH", File(csdFile).getParentDirectory().getFullPathName().toUTF8().getAddress());	
		#endif	
		
		//send host info before performance.. 
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
				csound->SetChannel(CabbageIDs::hostbpm.toUTF8(), hostInfo.bpm);
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
				csound->SetChannel(CabbageIDs::timeinseconds.toUTF8(), hostInfo.timeInSeconds);
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
				csound->SetChannel(CabbageIDs::isplaying.toUTF8(), hostInfo.isPlaying);
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
				csound->SetChannel(CabbageIDs::isrecording.toUTF8(), hostInfo.isRecording);
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
				csound->SetChannel(CabbageIDs::hostppqpos.toUTF8(), hostInfo.ppqPosition);

}
else{
        Logger::writeToLog("Csound couldn't compile your file");
        csoundStatus=false;
}
#endif
createGUI(csdFile.loadFileAsString(), true);
}
#endif

//===========================================================
// PLUGIN - DESTRUCTOR
//===========================================================
CabbagePluginAudioProcessor::~CabbagePluginAudioProcessor()
{
deleteAndZero(lookAndFeel);
deleteAndZero(lookAndFeelBasic);
Logger::writeToLog("~CabbagePluginAudioProcessor()");
Logger::setCurrentLogger (nullptr);
suspendProcessing(true);
removeAllChangeListeners();
#ifndef Cabbage_No_Csound
patStepMatrix.clear();
patternNames.clear();
patPfieldMatrix.clear();
xyAutomation.clear();

        //const MessageManagerLock mmLock;
        if(csound){
			if(csoundPerfThread){
				csoundPerfThread->Stop();
				csoundPerfThread = nullptr;
			}
				//csound->SetHostImplementedMIDIIO(false);
                csound->DeleteChannelList(csoundChanList);
                Logger::writeToLog("about to cleanup Csound");
                //csound->Cleanup();
				//csound->SetHostImplementedMIDIIO(false);

                csound->Reset();

                csound = nullptr;
                Logger::writeToLog("Csound cleaned up");
				if(audioSourcesArray.size()>0){
				for(int i=0;i<audioSourcesArray.size();i++){
					audioSourcesArray[i]->sourceChannelInfo.buffer = nullptr;
					audioSourcesArray[i]->audioSourceBuffer = nullptr;
				}
				audioSourcesArray.clear();
				}
        }

		soundFilerTempVector = nullptr;

#endif
}

int CabbagePluginAudioProcessor::performEntireScore(){
#ifndef Cabbage_No_Csound
	if(!isNativeThreadRunning){
	csoundPerfThread->Play();
	isNativeThreadRunning = true;
	}
#endif
	return 1;
}

//this callback will be employed when users run with Csound audio IO rather than Cabbage
void CabbagePluginAudioProcessor::YieldCallback(void* data){
	CabbagePluginAudioProcessor *cabbage = (CabbagePluginAudioProcessor *)data;
	cabbage->sendOutgoingMessagesToCsound();
	cabbage->updateCabbageControls();
}

//============================================================================
//RECOMPILE CSOUND. THIS IS CALLED FROM THE PLUGIN HOST WHEN UDPATES ARE MADE ON THE FLY
//============================================================================
void CabbagePluginAudioProcessor::reCompileCsound(File file)
{
#ifndef Cabbage_No_Csound
suspendProcessing(true);
soundFileIndex = 0;
midiOutputBuffer.clear();
getCallbackLock().enter();
csound->DeleteChannelList(csoundChanList);
csound->SetHostImplementedMIDIIO(true);

csound->Reset();
xyAutosCreated = false;
//csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
numCsoundChannels = 0;
//csound->SetParams(csoundParams);
//csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
csound->SetExternalMidiReadCallback(ReadMidiData);
csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
csound->SetExternalMidiWriteCallback(WriteMidiData);

CSspout = nullptr;
CSspin = nullptr;

csCompileResult = csound->Compile(const_cast<char*>(file.getFullPathName().toUTF8().getAddress()));

if(csCompileResult==0){
        //simple hack to allow tables to be set up correctly.
		keyboardState.allNotesOff(0);
		keyboardState.reset();
		csndIndex = 0;
        CSspout = csound->GetSpout();
        CSspin  = csound->GetSpin();
        csound->PerformKsmps();
        csound->SetScoreOffsetSeconds(0);
        csound->RewindScore();
        Logger::writeToLog("Csound compiled your file");
        numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
        cs_scale = csound->Get0dBFS();
        csoundStatus = true;
        debugMessageArray.add(CABBAGE_VERSION);
        debugMessageArray.add(String("\n"));
		//removeAllChangeListeners();
		getCallbackLock().exit();

		//init all channels with their init val
		for(int i=0;i<guiCtrls.size();i++)
		{
		csound->SetChannel( guiCtrls.getReference(i).getStringProp(CabbageIDs::channel).toUTF8(),
												guiCtrls.getReference(i).getNumProp(CabbageIDs::value));
		}

		#ifdef WIN32
		csound->SetChannel("CSD_PATH", file.getParentDirectory().getFullPathName().replace("\\", "\\\\").toUTF8().getAddress());	
		#else
		csound->SetChannel("CSD_PATH", file.getParentDirectory().getFullPathName().toUTF8().getAddress());	
		#endif
		
		this->suspendProcessing(false);
		
		return;
}
else{
    Logger::writeToLog("Csound couldn't compile your file");
    csoundStatus=false;
	}
getCallbackLock().exit();
#endif
}
//===========================================================
// PARSE CSD FILE AND FILL GUI/GUI-LAYOUT VECTORs.
// NO JUCE WIDGETS GET CREATED IN THIS CLASS. ALL
// GUI OBJECTS ARE CREATED ON THE FLY IN THE CABBAGE PLUGIN
// EDITOR FROM INFORMATION HELD IN THE GUICONTROLS VECTOR
//===========================================================
//maybe this should only be done at the end of a k-rate cycle..
void CabbagePluginAudioProcessor::createGUI(String source, bool refresh)
{
//clear arrays if refresh is set
if(refresh==true){
	guiLayoutCtrls.clear();
	guiCtrls.clear();
	CabbagePluginAudioProcessorEditor* editor = dynamic_cast<CabbagePluginAudioProcessorEditor*>(this->getActiveEditor());
	if(editor){
		editor->comps.clear();
		editor->layoutComps.clear();
	}
	//if rebuilding the entire plugin, reset soundfilers, if present
	if(audioSourcesArray.size()>0){
		for(int i=0;i<audioSourcesArray.size();i++){
			audioSourcesArray[i]->sourceChannelInfo.buffer = nullptr;
			audioSourcesArray[i]->audioSourceBuffer = nullptr;
		}
		audioSourcesArray.clear();
	}

}

int indexOfLastGUICtrl = guiCtrls.size();
int indexOfLastLayoutCtrl = guiLayoutCtrls.size();

int test=100;
int checkGUI = isGuiEnabled();
//setGuiEnabled((false));
int guiID=0;
StringArray csdText;
int lines=1;
String csdLine("");
csdText.addLines(source);
bool multiComment = false;
bool multiLine = false;
//check for minimal Cabbage GUI
    for(int i=0;i<csdText.size();i++)
        {
		if(csdText[i].indexOfWholeWordIgnoreCase(String("</Cabbage>"))==-1)
		{
		if(!csdText[i].contains("multitab "))//we don't enter for multitab, plants need to be created first
			if(csdText[i].trim().isNotEmpty()){
					if(csdText[i].contains("), \\")||
						csdText[i].contains("),\\")||
						csdText[i].contains(") \\")){
							multiLine = true;
							csdLine="";
							lines=0;
							while(multiLine){
							if(csdText[i+lines].contains("), \\")||
								csdText[i+lines].contains("),\\")||
								csdText[i+lines].contains(") \\"))
									lines++;
							else multiLine=false;
							}
							for(int y=0;y<=lines;y++)
							csdLine = csdLine + " "+ csdText[i+y].trim()+" ";
							i=i+lines;
					}
					else
					csdLine = csdText[i];
					//tidy up string
					csdLine = csdLine.trimStart();
					//csdLine = csdLine.removeCharacters(" \\");
					//csdLine = csdLine.removeCharacters(",\\");
					//Logger::writeToLog(csdLine);
					StringArray tokes;
					tokes.addTokens(csdLine.trimEnd(), ", ", "\"");

					if(tokes[0].containsIgnoreCase(String("/*"))){
					multiComment = true;
					}
					if(tokes[0].containsIgnoreCase(String("*\\"))){
					multiComment = false;
					}

					if(tokes[0].containsIgnoreCase(String(";"))){
					//allows for single line comments
					}
					else if(tokes[0].containsIgnoreCase(String("}"))){
					plantFlag = ""; //reset plantFlag when a closing bracket is found
					presetFlag = "";
					}

					if(!multiComment)
					//populate the guiLayoutCtrls vector with non-interactive widgets
					//the host widgets aren't GUI based but they can be added to this
					//vector too, as can the editor button.
					if(tokes[0].equalsIgnoreCase(String("form"))
									||tokes[0].equalsIgnoreCase(String("image"))
									||tokes[0].equalsIgnoreCase(String("keyboard"))
									||tokes[0].equalsIgnoreCase(String("csoundoutput"))
									||tokes[0].equalsIgnoreCase(String("line"))
									||tokes[0].equalsIgnoreCase(String("label"))
									||tokes[0].equalsIgnoreCase(String("hostbpm"))
									||tokes[0].equalsIgnoreCase(String("hosttime"))
									||tokes[0].equalsIgnoreCase(String("hostplaying"))
									||tokes[0].equalsIgnoreCase(String("hostppqpos"))
									||tokes[0].equalsIgnoreCase(String("vumeter"))
									||tokes[0].equalsIgnoreCase(String("patmatrix"))
									||tokes[0].equalsIgnoreCase(String("source"))
									||tokes[0].equalsIgnoreCase(String("multitab"))
									||tokes[0].equalsIgnoreCase(String("infobutton"))
									||tokes[0].equalsIgnoreCase(String("filebutton"))
									||tokes[0].equalsIgnoreCase(String("soundfiler"))
									||tokes[0].equalsIgnoreCase(String("snapshot"))
									||tokes[0].equalsIgnoreCase(String("table"))
									||tokes[0].equalsIgnoreCase(String("pvsview"))
									||tokes[0].equalsIgnoreCase(String("hostrecording"))
									||tokes[0].equalsIgnoreCase(String("directorylist"))
									||tokes[0].equalsIgnoreCase(String("transport"))
									||tokes[0].equalsIgnoreCase(String("groupbox")))
					{
							CabbageGUIClass cAttr(csdLine.trimEnd(), guiID);
							if(cAttr.getStringProp("native").length()>0){
								//create generic plugin editor and break..
								setupNativePluginEditor();
								nativePluginEditor = true;
								return;
							}
							if(cAttr.getNumProp(CabbageIDs::guirefresh)>1)
								guiRefreshRate = cAttr.getNumProp(CabbageIDs::guirefresh);

							//showMessage(cAttr.getStringProp("type"));
							csdLine = "";
							//add soundfiler buffering sources
							if(tokes[0].equalsIgnoreCase(String("soundfiler"))){
								addSoundfilerSource(cAttr.getStringProp(("file")), cAttr.getChannels());
								Logger::writeToLog("createGUI, soundfiler size:"+String(audioSourcesArray.size()-1));
								cAttr.setNumProp("soundfilerIndex", audioSourcesArray.size()-1);
							}


							//set up stuff for tables
							if(tokes[0].equalsIgnoreCase(String("table"))){
								if(cAttr.getStringArrayProp(CabbageIDs::channel).size()==0)
								for(int i=0;i<cAttr.getIntArrayProp("tablenumber").size();i++)
								cAttr.addDummyChannel("dummy"+String(i));

								for(int i=0;i<cAttr.getStringArrayProp(CabbageIDs::channel).size();i++)
								cAttr.addTableChannelValues();


							}

							//set up plant flag if needed for other widgets
							if(cAttr.getStringProp(String("plant")).isNotEmpty()){
									plantFlag = cAttr.getStringProp(String("plant"));
									presetFlag = cAttr.getStringProp(String("preset"));
							}
							else if(cAttr.getStringProp(String("reltoplant")).equalsIgnoreCase(String("")))
									cAttr.setStringProp(String("reltoplant"), plantFlag);
							guiLayoutCtrls.add(cAttr);


							guiID++;

							if(cAttr.getStringProp("type").containsIgnoreCase("form"))
									if(cAttr.getStringProp("text").length()>2)
											setPluginName(cAttr.getStringProp("text"));
									else if(cAttr.getStringProp("caption").length()>2)
											setPluginName(cAttr.getStringProp("caption"));
									else setPluginName("Untitled Cabbage Patch!");

							//StringArray log = logGUIAttributes(cAttr, String("Non-Interactive"));
							//debugMessageArray.addArray(logGUIAttributes(cAttr, String("Non-Interactive")));
							sendChangeMessage();
							//if instrument uses any of the host widgets, or an xypad, turn
							//on the timer
							if(tokes[0].equalsIgnoreCase(String("hostbpm"))
									||tokes[0].equalsIgnoreCase(String("hosttime"))
									||tokes[0].equalsIgnoreCase(String("hostplaying"))
									||tokes[0].equalsIgnoreCase(String("hostppqpos"))
									||tokes[0].equalsIgnoreCase(String("hostrecording")))
									startTimer(20);
					}
					//populate the guiCtrls vector with interactive widgets
					else if(tokes[0].equalsIgnoreCase(String("hslider"))
									||tokes[0].equalsIgnoreCase(String("vslider"))
									||tokes[0].equalsIgnoreCase(String("rslider"))
									||tokes[0].equalsIgnoreCase(String("combobox"))
									||tokes[0].equalsIgnoreCase(String("checkbox"))
									||tokes[0].equalsIgnoreCase(String("xypad"))
									||tokes[0].equalsIgnoreCase(String("button"))){
							CabbageGUIClass cAttr(csdLine.trimEnd(), guiID);
							//Logger::writeToLog(csdLine.trimEnd());
							csdLine = "";
							//Logger::writeToLog(tokes[0]);
							//attach widget to plant if need be
							if(cAttr.getStringProp(String("reltoplant")).equalsIgnoreCase(String(""))){
									//showMessage(cAttr.getStringProp(String("relToPlant")));
									cAttr.setStringProp(String("reltoplant"), plantFlag);
									//showMessage(String("presetFlag:")+presetFlag);
									//showMessage(cAttr.getStringProp("name"));
									if(cAttr.getStringProp("preset").length()<1)
											cAttr.setStringProp(String("preset"), presetFlag.trim());
									//showMessage(cAttr.getStringProp("preset"));
							}
					//xypad contain two control paramters, one for x axis and another for y. As such we add two
					//to our contorl vector so that plugin hosts display two sliders. We name one of the xypad pads
					// 'dummy' so that our editor doesn't display it. Our editor only needs to show one xypad.
							if(tokes[0].equalsIgnoreCase(String("xypad"))){
									cAttr.setStringProp(CabbageIDs::xychannel, String("X"));
									cAttr.setNumProp(CabbageIDs::range,  cAttr.getNumProp(CabbageIDs::rangex));
									cAttr.setNumProp(CabbageIDs::min,  cAttr.getNumProp(CabbageIDs::minx));
									cAttr.setNumProp(CabbageIDs::max,  cAttr.getNumProp(CabbageIDs::maxx));
									cAttr.setNumProp(CabbageIDs::value, cAttr.getNumProp(CabbageIDs::valuex));
									cAttr.setStringProp(String(CabbageIDs::channel), cAttr.getStringProp(CabbageIDs::xchannel));
									guiCtrls.add(cAttr);

									cAttr.setStringProp(CabbageIDs::xychannel, String("Y"));
									cAttr.setNumProp(CabbageIDs::range,  cAttr.getNumProp(CabbageIDs::rangey));
									cAttr.setNumProp(CabbageIDs::min,  cAttr.getNumProp(CabbageIDs::miny));
									cAttr.setNumProp(CabbageIDs::max,  cAttr.getNumProp(CabbageIDs::maxy));
									cAttr.setNumProp(CabbageIDs::value, cAttr.getNumProp(CabbageIDs::valuey));
									cAttr.setStringProp(String(CabbageIDs::channel), cAttr.getStringProp(CabbageIDs::ychannel));
									//append 'dummy' to name so the editor know not to display the
									//second xypad
									cAttr.setStringProp("name", cAttr.getStringProp(CabbageIDs::name)+String("dummy"));
									guiCtrls.add(cAttr);
									guiID++;
									startTimer(10);
							}
							else{
								//Logger::writeToLog("Value:"+String(cAttr.getNumProp(CabbageIDs::value)));
							guiCtrls.add(cAttr);
							guiID++;
							}

							//debugMessageArray.addArray(logGUIAttributes(cAttr, String("Interactive")));
							sendChangeMessage();
					}

			}
		}
                else break;
        } //end of scan through entire csd text, control vectors are now populated

		//create multitabs now that plants have been inserted to control vector..
		for(int i=0;i<csdText.size();i++)
			{
			if(csdText[i].contains("multitab ") && !csdText[i].contains(";"))
			{
				csdLine = csdText[i];
				csdLine = csdLine.trimStart();
				StringArray tokes;
				tokes.addTokens(csdLine.trimEnd(), ", ", "\"");
				if(tokes[0].equalsIgnoreCase(String("multitab"))){
					CabbageGUIClass cAttr(csdLine.trimEnd(), guiID);
					//showMessage(cAttr.getStringProp("type"));
					csdLine = "";
					//set up plant flag if needed for other widgets
					if(cAttr.getStringProp(String("plant")).isNotEmpty()){
					plantFlag = cAttr.getStringProp(String("plant"));
					presetFlag = cAttr.getStringProp(String("preset"));
					}
					else if(cAttr.getStringProp(String("reltoplant")).equalsIgnoreCase(String("")))
					cAttr.setStringProp(String("reltoplant"), plantFlag);
					guiLayoutCtrls.add(cAttr);
					guiID++;
				}
			}
		}//end of multitab check


		//init all channels with their init val, and set parameters
		for(int i=0;i<guiCtrls.size();i++)
		{
//		Logger::writeToLog(guiCtrls.getReference(i).getStringProp(CabbageIDs::channel)+": "+String(guiCtrls[i].getNumProp(CabbageIDs::value)));
#ifndef Cabbage_No_Csound
		if(guiCtrls.getReference(i).getStringProp("channeltype")=="string")
		//deal with combobox strings..
		csound->SetChannel(guiCtrls.getReference(i).getStringProp(CabbageIDs::channel).toUTF8(), "");
//									guiCtrls.getReference(i).getStringArrayPropValue("text", guiCtrls[i].getNumProp(CabbageIDs::value)-1).toUTF8().getAddress());
		else
		csound->SetChannel( guiCtrls.getReference(i).getStringProp(CabbageIDs::channel).toUTF8(), guiCtrls[i].getNumProp(CabbageIDs::value));
#endif
		}

		#ifdef Cabbage_Build_Standalone

		if(this->getActiveEditor()){
			CabbagePluginAudioProcessorEditor* editor = dynamic_cast<CabbagePluginAudioProcessorEditor*>(this->getActiveEditor());

			if(refresh){
			editor->comps.clear();
			editor->layoutComps.clear();
			editor->repaint();
			//((CabbagePluginAudioProcessorEditor*)getActiveEditor())->setEditMode(false);
			//editor->setEditMode(false);
			}
			//!this will not work as we are moving through our entire control vector
			for(int i=indexOfLastLayoutCtrl;i<guiLayoutCtrls.size();i++)
			editor->InsertGUIControls(guiLayoutCtrls[i]);
			for(int i=indexOfLastGUICtrl;i<guiCtrls.size();i++)
			editor->InsertGUIControls(guiCtrls[i]);

			if(refresh)
			editor->setEditMode(checkGUI);
		}

#endif
}

//============================================================================
//dynamically remove components from editor window, used in EDIT mode
//============================================================================
void CabbagePluginAudioProcessor::removeGUIComponent(int index, String type){
//remove component struct from our abstract vector
CabbagePluginAudioProcessorEditor* editor = dynamic_cast<CabbagePluginAudioProcessorEditor*>(this->getActiveEditor());

		if(type=="interactive"){
		//remove GUI abstract structure from vector
		guiCtrls.remove(index);
		}
		else{
		//remove GUI abstract structure from vector
		guiLayoutCtrls.remove(index);
		}

		editor->updateLayoutEditorFrames();
		editor->repaint();
}

//============================================================================
//SETS UP A GENERIC PLUGIN EDITOR
//============================================================================
void CabbagePluginAudioProcessor::setupNativePluginEditor()
{
	/*
	//create a basic 'native' gui if specificed by the user.
		int guiID = 0;
		guiCtrls.clear();
		for(int i=0;i<numCsoundChannels;i++){
			const CsoundChannelListEntry& entry = csoundChanList[i];
			if (entry.type & CSOUND_CONTROL_CHANNEL && entry.type & CSOUND_INPUT_CHANNEL) {
				MYFLT ddefault, dmin, dmax;
				int value_type = getCsound()->GetControlChannelParams(entry.name, ddefault, dmin, dmax);
				String parameterInfo;
				float initVal = (ddefault<dmin ? dmin : ddefault);
				parameterInfo << "channel(\"" << entry.name << "\"), " << "range(" << String(dmin) << ", " << String(dmax) << ", " << String(initVal) << ")";
				Logger::writeToLog(parameterInfo);
				CabbageGUIClass cAttr(parameterInfo, guiID);
				cAttr.setNumProp("range", dmax-dmin);
				//cAttr.setStringProp(CabbageIDs::channel, entry.name);
				//cAttr.setNumProp("max", (dmax>0 ? dmax : 1));
				//cAttr.setNumProp("init", (ddefault<dmin ? dmin : ddefault));

				    switch(value_type) {
						case CSOUND_CONTROL_CHANNEL_INT:
						cAttr.setNumProp("incr", 1);
						break;

						case CSOUND_CONTROL_CHANNEL_LIN:
						cAttr.setNumProp("incr", .01);
						break;

						case CSOUND_CONTROL_CHANNEL_EXP:
						cAttr.setNumProp("skew", .5);
						break;

						default:

						break;
					}

				guiCtrls.add(cAttr);
				setPluginName("Test Plugin");
				guiID++;
				}
			}
			 */
}

//===========================================================
// SHOW SOURCE EDITOR
//===========================================================
//void CabbagePluginAudioProcessor::createAndShowSourceEditor(LookAndFeel* looky)
//{
//if(!cabbageCsoundEditor){
//cabbageCsoundEditor = new CabbageEditorWindow(looky);
//cabbageCsoundEditor->setCsoundFile(csdFile);
//cabbageCsoundEditor->setCsoundOutputText(csoundOutput);
//}
//cabbageCsoundEditor->setVisible(true);
//}

//===========================================================
// CALLBACKS FOR STANDALONE
//===========================================================
#ifndef Cabbage_No_Csound
void CabbagePluginAudioProcessor::messageCallback(CSOUND* csound, int /*attr*/,  const char* fmt, va_list args)
{
  CabbagePluginAudioProcessor* ud = (CabbagePluginAudioProcessor *) csoundGetHostData(csound);
  char msg[MAX_BUFFER_SIZE];
  vsnprintf(msg, MAX_BUFFER_SIZE, fmt, args);
// MOD - Stefano Bonetti
  ud->debugMessage += String(msg); //We have to append the incoming msg
  ud->csoundOutput += ud->debugMessage;
  ud->debugMessageArray.add(ud->debugMessage);
  //Logger::writeToLog(String(msg).trim());
  ud->sendChangeMessage();
// MOD - End
  ud->debugMessage = "";
  ud = nullptr;
}
#endif


//==============================================================================
#if defined(Cabbage_Build_Standalone) || defined(Cabbage_Plugin_Host)
CabbagePluginAudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String inputfile, bool guiOnOff, int pluginType)
{
    return new CabbagePluginAudioProcessor(inputfile, false, pluginType);
}
#else
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CabbagePluginAudioProcessor();
}
#endif

//==========================================================================
//action listener. Listen to messages being sent form xypad automations
//==========================================================================
void CabbagePluginAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
	float xVal, yVal;
	//is message coming from an xypad
	XYPadAutomation* xyPad = dynamic_cast< XYPadAutomation*>(source);
	if(xyPad){
	#ifdef Cabbage_Build_Standalone
		setParameterNotifyingHost(xyPad->paramIndex, xyPad->getXValue());
		setParameterNotifyingHost(xyPad->paramIndex+1, xyPad->getYValue());
	#else
	if(xyPad->getMinimumXValue()>=0)
		xVal = (xyPad->getXValue()/xyPad->getXRange())+(fabs(xyPad->getMinimumXValue())/xyPad->getXRange());
	else
		xVal = (xyPad->getXValue()/xyPad->getXRange())-(fabs(xyPad->getMinimumXValue())/xyPad->getXRange());

	if(xyPad->getMinimumYValue()<=0)
		yVal = (xyPad->getYValue()/xyPad->getYRange())+(fabs(xyPad->getMinimumYValue())/xyPad->getYRange());
	else
		yVal = (xyPad->getYValue()/xyPad->getYRange())-(fabs(xyPad->getMinimumYValue())/xyPad->getYRange());

	//Logger::writeToLog("Param:"+String(xyPad->paramIndex)+"  xyPadXVal:"+String(xVal));
	//Logger::writeToLog("Param:"+String(xyPad->paramIndex+1)+"  xyPadYVal:"+String(yVal));

	setParameterNotifyingHost(xyPad->paramIndex, xVal);
	setParameterNotifyingHost(xyPad->paramIndex+1, yVal);
	#endif
	}
}

//==============================================================================
// getTable data from Csound so table editor can draw table
//==============================================================================
const Array<double, CriticalSection> CabbagePluginAudioProcessor::getTable(int tableNum){
		Array<double, CriticalSection> points;

		int tableSize=0;
#ifndef Cabbage_No_Csound
        MYFLT* temp;
		tableSize = csound->GetTable(temp, tableNum);
#else
        float *temp;
#endif
		if(tableSize>0)
		points = Array<double, CriticalSection>(temp, tableSize);
		return points;
}


//=================================================================================
// Get and Set Parameter methods, called by our editor, and the plugin host...
//=================================================================================
float CabbagePluginAudioProcessor::getParameter (int index)
{
float range = getGUICtrls(index).getNumProp(CabbageIDs::range);
float min = getGUICtrls(index).getNumProp(CabbageIDs::min);
//Logger::writeToLog("parameterGet-"+String(index)+String("-Min:")+String(min)+" Range:"+String(range)+ " Val:"+String(getGUICtrls(index).getNumProp(CabbageIDs::value)));
//Logger::writeToLog("parameterGet:"+String(index)+String(":")+String(guiCtrls[index].getNumProp(CabbageIDs::value)));

/* this gets called at any time by our host or out GUI editor */
if(index<(int)guiCtrls.size()){//make sure index isn't out of range
	#ifndef Cabbage_Build_Standalone
	float val = (getGUICtrls(index).getNumProp(CabbageIDs::value)/range)-(min/range);
	if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::combobox)
	return (getGUICtrls(index).getNumProp(CabbageIDs::value)/getGUICtrls(index).getNumProp(CabbageIDs::comborange));
	else if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::checkbox ||
			getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::button)
	return getGUICtrls(index).getNumProp(CabbageIDs::value);			
	else
	return (getGUICtrls(index).getNumProp(CabbageIDs::value)/range)-(min/range);
	#else
	return guiCtrls[index].getNumProp(CabbageIDs::value);
	#endif
	}
else
	return 0.0f;
}


void CabbagePluginAudioProcessor::setParameter (int index, float newValue)
{
String stringMessage;
#ifndef Cabbage_No_Csound
/* this will get called by the plugin GUI sliders or
by the host, via automation. The timer thread in the plugin's editor
will constantly update with the values that have been set here.
We don't actually change any parameters here, we simply add the messages
to a queue. See next method. The updates will only happen when it's safe to do. */
float range, min, max, comboRange;
//Logger::writeToLog("parameterSet:"+String(newValue));
if(index<(int)guiCtrls.size())//make sure index isn't out of range
   {
	#ifndef Cabbage_Build_Standalone
	//scaling in here because incoming values in plugin mode range from 0-1
	range = getGUICtrls(index).getNumProp(CabbageIDs::range);
	comboRange = getGUICtrls(index).getNumProp(CabbageIDs::comborange);
	//Logger::writeToLog("inValue:"+String(newValue));
	min = getGUICtrls(index).getNumProp(CabbageIDs::min);

	if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::xypad)
		newValue = (jmax(0.f, newValue)*range)+min;
	else if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::combobox)//combo box value need to be rounded...
		newValue = (newValue*comboRange);
	else if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::checkbox ||
			getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::button)
		range=1;
	else
		newValue = (newValue*range)+min;

	//guiCtrls.getReference(index).setNumProp(CabbageIDs::value, newValue);
	//messageQueue.addOutgoingChannelMessageToQueue(guiCtrls.getReference(index).getStringProp(CabbageIDs::channel).toUTF8(),  newValue,
	//guiCtrls.getReference(index).getStringProp("type"));
	//Logger::writeToLog(String("parameterSet:"+String(newValue)));
	#endif
	//Logger::writeToLog(String("parameterSet:"+String(newValue)));
	//no need to scale here when in standalone mode
	
	if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::combobox &&
								getGUICtrls(index).getStringProp(CabbageIDs::channeltype)==CabbageIDs::stringchannel)
	  {
		stringMessage = getGUICtrls(index).getStringArrayPropValue(CabbageIDs::text, newValue-1);
		//Logger::writeToLog(stringMessage);
		messageQueue.addOutgoingChannelMessageToQueue(guiCtrls.getReference(index).getStringProp(CabbageIDs::channel), 
												  stringMessage,
												  CabbageIDs::stringchannel);
	  }
	else
		messageQueue.addOutgoingChannelMessageToQueue(guiCtrls.getReference(index).getStringProp(CabbageIDs::channel), 
												  newValue,
												  guiCtrls.getReference(index).getStringProp(CabbageIDs::type));
	
	
	guiCtrls.getReference(index).setNumProp(CabbageIDs::value, newValue);	
   }
#endif
}

//==============================================================================
//this method gets called after a performKsmps() to update our GUI controls
//with messages from Csound. For instance, a user might wish to change the position
//of a GUI slider from Csound by using a chnset opcode. The speed at which this is
//updated can be teaked, so as not to hog resources. It might be worth allowing users
//the option of setting how fast this update...
void CabbagePluginAudioProcessor::updateCabbageControls()
{
#ifndef Cabbage_No_Csound
String chanName;
if(!CSCompResult)
	{
	MYFLT* val=0;
	//update all control widgets
	for(int index=0;index<getGUICtrlsSize();index++)
		{
		if(guiCtrls[index].getStringProp(CabbageIDs::channeltype).equalsIgnoreCase(CabbageIDs::stringchannel)){
		//argghhh!! THIS NEEDS TO ALLOW COMBOBOXEX THAT CONTAIN SNAPSHOTS TO UPDATE!	
		}
		else{	
			float value = csound->GetChannel(guiCtrls[index].getStringProp(CabbageIDs::channel).getCharPointer());
			//Logger::writeToLog("Channel:"+guiCtrls[index].getStringProp(CabbageIDs::channel));
			//Logger::writeToLog("value:"+String(value));
			guiCtrls.getReference(index).setNumProp(CabbageIDs::value, value);
			}
		}
//update all layout control widgets
//currently this is only needed for table widgets as other layout controls
//don't use channel messages...
	for(int index=0;index<getGUILayoutCtrlsSize();index++)
		{
		if(guiLayoutCtrls[index].getStringProp(CabbageIDs::type)==CabbageIDs::table)
			{
			for(int y=0;y<guiLayoutCtrls[index].getStringArrayProp(CabbageIDs::channel).size();y++){
				//String test = getGUILayoutCtrls(index).getStringArrayPropValue(CabbageIDs::channel, y);
				float value = csound->GetChannel(guiLayoutCtrls[index].getStringArrayPropValue(CabbageIDs::channel, y).getCharPointer());
				guiLayoutCtrls[index].setTableChannelValues(y, value);
				}
			}
		}
	}
sendChangeMessage();
#endif
}

//==============================================================================
//this method only gets called when it's safe to do so, i.e., between calls to performKsmps()
//this method sends any channel messages that are in the queue to from Cabbage to Csound
void CabbagePluginAudioProcessor::sendOutgoingMessagesToCsound()
{
#ifndef Cabbage_No_Csound
if(!csCompileResult){
#ifndef Cabbage_Build_Standalone

                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(CabbageIDs::hostbpm.toUTF8(), hostInfo.bpm);
                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(CabbageIDs::timeinseconds.toUTF8(), hostInfo.timeInSeconds);
                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(CabbageIDs::isplaying.toUTF8(), hostInfo.isPlaying);
                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(CabbageIDs::isrecording.toUTF8(), hostInfo.isRecording);
                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(CabbageIDs::hostppqpos.toUTF8(), hostInfo.ppqPosition);

#endif



for(int i=0;i<messageQueue.getNumberOfOutgoingChannelMessagesInQueue();i++)
		{
		//Logger::writeToLog("MessageType:"+messageQueue.getOutgoingChannelMessageFromQueue(i).type);
		if(messageQueue.getOutgoingChannelMessageFromQueue(i).type=="directoryList"){
			for(int y=0;y<scoreEvents.size();y++)
			csound->InputMessage(scoreEvents[y].toUTF8());
			//scoreEvents.clear();
		}
		//update Csound function tables with values from table widget
		else if(messageQueue.getOutgoingChannelMessageFromQueue(i).type=="updateTable"){
			//Logger::writeToLog(messageQueue.getOutgoingChannelMessageFromQueue(i).fStatement.toUTF8());
			csound->InputMessage(messageQueue.getOutgoingChannelMessageFromQueue(i).fStatement.getCharPointer());
		}
		//catch string messags
		else if(messageQueue.getOutgoingChannelMessageFromQueue(i).type==CabbageIDs::stringchannel){	
		csound->SetChannel(messageQueue.getOutgoingChannelMessageFromQueue(i).channelName.getCharPointer(),
						   messageQueue.getOutgoingChannelMessageFromQueue(i).stringVal.toUTF8().getAddress());
		}
		else
		csound->SetChannel(messageQueue.getOutgoingChannelMessageFromQueue(i).channelName.getCharPointer(),
						   messageQueue.getOutgoingChannelMessageFromQueue(i).value);
		}

	messageQueue.flushOutgoingChannelMessages();

	if(isAutomator){
		//sendChangeMessage();
		//sendActionMessage("update automation:"+String(automationParamID)+"|"+String(automationAmp));
		//Logger::writeToLog("update automation:"+String(automationAmp));
		}
	}

#endif
}

//==============================================================================
//set up buffered audio source for each sound file object. The method below this one
//fills Csound channels with sampler from our soundfiler controls..
void CabbagePluginAudioProcessor::addSoundfilerSource(String filename, StringArray channels)
{
#ifndef Cabbage_No_Csound
	audioSourcesArray.add(new CabbageAudioSource(filename, csound->GetKsmps()));
	Logger::writeToLog("Number of soundfilers:"+String(audioSourcesArray.size()));
	audioSourcesArray[audioSourcesArray.size()-1]->channels = channels;


	if(File(filename).exists())Logger::writeToLog("File exists");
	else{
		Logger::writeToLog("Soundfiler can't find file");
		//set default sampling rate in case of no file name given
		audioSourcesArray[audioSourcesArray.size()-1]->isValidFile = false;
		audioSourcesArray[audioSourcesArray.size()-1]->sampleRate = 44100;
	}
#endif
}


//gets sample data from any soundfiler controls and passes it to Csound
void CabbagePluginAudioProcessor::sendAudioToCsoundFromSoundFilers(int numSamples)
{
#ifndef Cabbage_No_Csound
if(this->isSuspended()==false){
for(int i=0;i<audioSourcesArray.size();i++){
	AudioSampleBuffer output (2, numSamples);
	audioSourcesArray[i]->sourceChannelInfo.buffer = &output;
	audioSourcesArray[i]->sourceChannelInfo.startSample = 0;
	audioSourcesArray[i]->sourceChannelInfo.numSamples = output.getNumSamples();

	if(audioSourcesArray[i]->isSourcePlaying && audioSourcesArray[i]->isValidFile)
	audioSourcesArray[i]->audioSourceBuffer->getNextAudioBlock(audioSourcesArray[i]->sourceChannelInfo);
	else
		output.clear();

	for(int index=0;index<audioSourcesArray[i]->channels.size();index++)
		{
		float* samples = output.getSampleData(index);

		for(int y=0;y<numSamples;y+=2)
			soundFilerTempVector[y] = samples[y];

			//Logger::writeToLog(audioSourcesArray[i]->channels[index]);

			if(csoundGetChannelPtr(csound->GetCsound(), &soundFilerTempVector, audioSourcesArray[i]->channels[index].toUTF8(),
							CSOUND_INPUT_CHANNEL | CSOUND_AUDIO_CHANNEL) != 0)
							Logger::writeToLog("error sending audio to Csound");
		}
	}
}
#endif
}

//========================================================================
// Standard plugin methods, getName, getNumParameters, setParamterName, get ProgramName, etc....
//==============================================================================
const String CabbagePluginAudioProcessor::getName() const
{
    return pluginName;
}

int CabbagePluginAudioProcessor::getNumParameters()
{
    return guiCtrls.size();
}

const String CabbagePluginAudioProcessor::getParameterName (int index)
{
    if(index<(int)guiCtrls.size())//make sure index isn't out of range
                return guiCtrls.getReference(index).getStringProp(CabbageIDs::channel);
        else return String::empty;
}

const String CabbagePluginAudioProcessor::getParameterText (int index)
{
        if(index<(int)guiCtrls.size())//make sure index isn't out of range
    return String (guiCtrls.getReference(index).getNumProp(CabbageIDs::value), 2);
        else return String::empty;
}

const String CabbagePluginAudioProcessor::getInputChannelName (int channelIndex) const
{
        if(channelIndex<(int)guiCtrls.size())//make sure index isn't out of range
    return String (channelIndex + 1);
        else return String::empty;
}

const String CabbagePluginAudioProcessor::getOutputChannelName (int channelIndex) const
{
        if(channelIndex<(int)guiCtrls.size())//make sure index isn't out of range
    return String (channelIndex + 1);
        else return String::empty;
}

bool CabbagePluginAudioProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool CabbagePluginAudioProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool CabbagePluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool CabbagePluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

void CabbagePluginAudioProcessor::setGuiEnabled(bool val){
#ifdef Cabbage_Build_Standalone
	guiON = val;
	CabbagePluginAudioProcessorEditor* editor = dynamic_cast< CabbagePluginAudioProcessorEditor*>(this->getActiveEditor());
	if(editor){
	if(val==false){
		int val = getPreference(appProperties, "ExternalEditor");
		if(val)
		csdFile.replaceWithText(codeEditor->getAllText());
		//editor->resizer->setVisible(false);
		//editor->propsWindow->setVisible(false);
	}
	else{
	//editor->resizer->setVisible(true);
	}
	}
#endif
}

int CabbagePluginAudioProcessor::getNumPrograms()
{
    return 0;
}

int CabbagePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CabbagePluginAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String CabbagePluginAudioProcessor::getProgramName (int /*index*/)
{
    return String::empty;
}

void CabbagePluginAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void CabbagePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    keyboardState.reset();
	for(int i=0;i<audioSourcesArray.size();i++)
		if(audioSourcesArray[i]->isValidFile)
			audioSourcesArray[i]->audioSourceBuffer->prepareToPlay(samplesPerBlock, sampleRate);
}
//==============================================================================
void CabbagePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
        keyboardState.reset();
}
//==============================================================================
//this following callback only runs in plugin mode, and only when one of the
//host widgets are being used
void CabbagePluginAudioProcessor::timerCallback(){
#ifndef Cabbage_No_Csound
for(int y=0;y<xyAutomation.size();y++){
	if(xyAutomation[y])
	xyAutomation[y]->update();
	}
#endif
}

//==============================================================================
void CabbagePluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
if(!isSuspended() && !isGuiEnabled()){
	float* audioBuffer;
	float lastOutputAmp;
	#ifndef Cabbage_No_Csound
	int numSamples = buffer.getNumSamples();

	if(csCompileResult==0){
	keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
	midiBuffer = midiMessages;
	ccBuffer = midiMessages;

	//if no inputs are used clear buffer in case it's not empty..
	if(getNumInputChannels()==0)
		buffer.clear();

#if JucePlugin_ProducesMidiOutput
	if(!midiOutputBuffer.isEmpty())
		midiMessages.swapWith(midiOutputBuffer);
#endif

	for(int i=0;i<numSamples;i++, csndIndex++)
	   {
			if(csndIndex == csound->GetKsmps())
			{
				getCallbackLock().enter();
				//slow down calls to these functions, no need for them to be firing at k-rate
				yieldCounter = (yieldCounter>guiRefreshRate) ? 0 : yieldCounter+1;
				if(yieldCounter==0){
				sendOutgoingMessagesToCsound();
				updateCabbageControls();
				}
				if(audioSourcesArray.size()>0)
				sendAudioToCsoundFromSoundFilers(csound->GetKsmps());

				csCompileResult = csound->PerformKsmps();
				if(csCompileResult!=0)
					suspendProcessing(true);

				getCallbackLock().exit();
				csndIndex = 0;
			}
			if(!csCompileResult)
				{
				for(int channel = 0; channel < getNumOutputChannels(); channel++ )
					{
					audioBuffer = buffer.getSampleData(channel,0);
					pos = csndIndex*getNumOutputChannels();
					CSspin[channel+pos] = audioBuffer[i]*cs_scale;
					audioBuffer[i] = (CSspout[channel+pos]/cs_scale);
					}
				}
			else
				buffer.clear();


		}
	}//if not compiled just mute output
	else{
		for(int channel = 0; channel < getNumInputChannels(); channel++)
		{
			audioBuffer = buffer.getSampleData(channel,0);
			for(int i=0; i<numSamples;i++, csndIndex++)
			{
				audioBuffer[i]=0;
			}
		}
	}

	#endif
}

#if JucePlugin_ProducesMidiOutput
	if(!midiBuffer.isEmpty())
	midiMessages.swapWith(midiOutputBuffer);
#endif
}



//==============================================================================
// MIDI functions
//==============================================================================

#ifndef Cabbage_No_Csound
int CabbagePluginAudioProcessor::OpenMidiInputDevice(CSOUND * csound, void **userData, const char* /*devName*/)
{
*userData = csoundGetHostData(csound);
if(!userData)
cout << "\n\ncan't open midi in\n\n";
return 0;
}

//==============================================================================
// Reads MIDI input data from host, gets called every time there is MIDI input to our plugin
//==============================================================================
int CabbagePluginAudioProcessor::ReadMidiData(CSOUND* /*csound*/, void *userData,
unsigned char *mbuf, int nbytes)
{
        CabbagePluginAudioProcessor *midiData = (CabbagePluginAudioProcessor *)userData;
        if(!userData){
                cout << "\n\nInvalid";
                return 0;
                }
        int cnt=0;

        if(!midiData->midiBuffer.isEmpty() && cnt <= (nbytes - 3)){
           MidiMessage message(0xf4, 0, 0, 0);
           MidiBuffer::Iterator i (midiData->midiBuffer);
           int messageFrameRelativeTothisProcess;
           while (i.getNextEvent (message, messageFrameRelativeTothisProcess))
           {
                   if(message.isNoteOn()){
                        *mbuf++ = (unsigned char)0x90 + message.getChannel();
                   *mbuf++ = (unsigned char)message.getNoteNumber();
                   *mbuf++ = (unsigned char)message.getVelocity();
                   cnt += 3;
                   }
                   else if(message.isNoteOff()){
                        *mbuf++ = (unsigned char)0x80 + message.getChannel();
                   *mbuf++ = (unsigned char)message.getNoteNumber();
                   *mbuf++ = (unsigned char)message.getVelocity();
                   cnt += 3;
                   }
				   else if(message.isAllSoundOff()){
                        *mbuf++ = (unsigned char)0x7B + message.getChannel();
                   *mbuf++ = (unsigned char)message.getNoteNumber();
                   *mbuf++ = (unsigned char)message.getVelocity();
                   cnt += 3;
                   }
				   else if(message.isController()){
						*mbuf++ = (unsigned char)0xB0 + message.getChannel()-1;
				   *mbuf++ = (unsigned char)message.getControllerNumber();
				   *mbuf++ = (unsigned char)message.getControllerValue();
				   cnt += 3;
				   }

           }
           midiData->midiBuffer.clear();
        }
 return cnt;
}

//==============================================================================
// Opens MIDI output device, adding -QN to your CsOptions will causes this method to be called
// as soon as your plugin loads
//==============================================================================
int CabbagePluginAudioProcessor::OpenMidiOutputDevice(CSOUND * csound, void **userData, const char* /*devName*/)
{
*userData = csoundGetHostData(csound);
if(!userData)
	Logger::writeToLog("\n\ncan't open midi out\n\n");
return 0;
}

//==============================================================================
// Write MIDI data to plugin's MIDI output. Each time Csound outputs a midi message this
// method should be called. Note: you must have -Q set in your CsOptions
//==============================================================================
int CabbagePluginAudioProcessor::WriteMidiData(CSOUND* /*csound*/, void *_userData,
const unsigned char *mbuf, int nbytes)
{
CabbagePluginAudioProcessor *userData = (CabbagePluginAudioProcessor *)_userData;
if(!userData){
	Logger::writeToLog("\n\nInvalid");
    return 0;
  }

MidiMessage message(mbuf, nbytes, 0);
//Logger::writeToLog(String(message.getNoteNumber()));
userData->midiOutputBuffer.addEvent(message, 0);
return nbytes;
}

#endif
//==============================================================================
bool CabbagePluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CabbagePluginAudioProcessor::createEditor()
{
    if(!nativePluginEditor)
		return new CabbagePluginAudioProcessorEditor (this);
	else
		return new CabbageGenericAudioProcessorEditor (this);
}

//==============================================================================
void CabbagePluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

    // Create an outer XML element..
    XmlElement xml ("CABBAGE_PLUGIN_SETTINGS");
	
	for(int i=0;i<guiCtrls.size();i++)
			xml.setAttribute (guiCtrls[i].getStringProp(CabbageIDs::channel), guiCtrls[i].getNumProp(CabbageIDs::value));


    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void CabbagePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("CABBAGE_PLUGIN_SETTINGS"))
        {
		for(int i=0;i<this->getNumParameters();i++)
			this->setParameter(i, (float)xmlState->getDoubleAttribute(guiCtrls[i].getStringProp(CabbageIDs::channel)));

        }
    }
}

//==============================================================================


