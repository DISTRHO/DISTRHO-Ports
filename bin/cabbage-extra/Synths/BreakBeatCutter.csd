;BreakBeatCutter.csd
;Iain McCurdy, 2013.

; Break Beat Cut Up using the bbcut opcode with additional processing

;==BBCUT==========================================================================================================================
'Sub-division' determines the note duration used as the base unit in  cut-ups. 
For example a value of 8 represents quavers (eighth notes), 16 represents semiquavers (sixteenth notes) and so on.                                                   

'Bar Length' represents the number of beats per bar. For example, a value of 4 represents a 4/4 bar and so on. 

'Phrase' defines the number of bars that will elapse before the cutting up pattern restarts from the beginning.          

'Stutter' is a separate cut-up process which occasionally will take a very short fragment of the input audio and repeat
it many times. 

'Stutter Speed' defines the duration of each stutter in relation to 'Sub-division'. 
If subdivision is 8 (quavers / eighth notes) and 'Stutter Speed' is 2 then each stutter will be a semiquaver / sixteenth note.

'Stutter Chance' defines the frequency of stutter moments. 
The range for this parameter is 0 to 1. Zero means stuttering will be very unlikely, 1 means it will be very likely.       
'Repeats' defines the number of repeats that will be employed in normal cut-up events.                                     
When processing non-rhythmical, unmetered material it may be be more interesting to employ non-whole numbers for parameters such as 'Sub-division', 'Phrase' and 'Stutter Speed'.                                                      
;========================================================================================================================




;==FILTER================================================================================================================
Additionally in this example a randomly moving band-pass filter has been implemented. 

'Filter Mix' crossfades between the unfiltered bbcut signal and the filtered bbcut signal.   

'Cutoff Freq.' consists of two small sliders which determine the range from which random cutoff values are derived.       

'Interpolate<=>S&H' fades continuously between an interpolated random function and a sample and hold type random function. 

'Filter Div.' controls the frequency subdivision with which new random cutoff frequency values are generated - a value of '1' means that new values are generated once every bar.                                    
;========================================================================================================================



;==WGUIDE================================================================================================================
A waveguide effect can randomly and rhythmically cut into the audio stream
'Chance' defines the probability of this happening. 0=never 1=always
The range of frequencies the effect will choose from is defined by the user as note values.
Frequencies are quatised to adhere to equal temperament.
;========================================================================================================================



;==SQUARE MOD. (Square wave ring modulation)=============================================================================
This effect can similarly randomly and rhythmically cut into the audio stream using the 'chance' control
The range of frequencies the modulator waveform can move between is defined as 'oct' values.
;========================================================================================================================


;==F.SHIFT (Frequency Shifter)===========================================================================================
Similar to the above except using a frequency shifter effect.
;========================================================================================================================

<Cabbage>
form          size(440,485), caption("Break Beat Cutter"), pluginID("bbct")

groupbox bounds( 0,  0,440,145), text("CUTTER"), plant("cutter"), FontColour(silver),colour(40,40,40){
rslider bounds( 10, 25, 70, 70), colour("Tan"), tracker("Tan"), fontcolour("silver"), text("Sub-div."),       channel("subdiv"),  range(1,  512,  8, 1, 1)
rslider bounds( 80, 25, 70, 70), colour("Tan"), tracker("Tan"), fontcolour("silver"), text("Bar Length"),     channel("barlen"),  range(1,   16,  2, 1, 1)
rslider bounds(150, 25, 70, 70), colour("Tan"), tracker("Tan"), fontcolour("silver"), text("Phrase"),         channel("phrase"),  range(1, 512, 8, 1, 1)
rslider bounds(220, 25, 70, 70), colour("Tan"), tracker("Tan"), fontcolour("silver"), text("Repeats"),        channel("repeats"), range(1, 32, 2, 1, 1)
rslider bounds(290, 25, 70, 70), colour("Tan"), tracker("Tan"), fontcolour("silver"), text("Stut. Speed"),  channel("stutspd"), range(1, 32, 4, 1, 1)
rslider bounds(360, 25, 70, 70), colour("Tan"), tracker("Tan"), fontcolour("silver"), text("Stut. Chance"), channel("stutchnc"), range(0, 1.00, 0.5)
hslider bounds(  5, 95,425, 40), colour("Tan"), tracker("Tan"), fontcolour("silver"), textBox(1)    channel("BPM"), range(10,  500, 110,1,1)
label   bounds(198,127, 25, 11), text("BPM"), FontColour("silver")
}



groupbox bounds( 0,145,440,100), text("FILTER"), plant("filter"), FontColour(silver),colour(40,40,40){
rslider bounds( 10, 25, 70, 70), colour(200,100,50,255), tracker(200,100,50,255), fontcolour("silver"), text("Mix"),    channel("FltMix"), range(0, 1.00, 0.6)
rslider bounds( 80, 25, 70, 70), colour(200,100,50,255), tracker(200,100,50,255), fontcolour("silver"), text("Division"),    channel("fltdiv"), range(1, 16, 1,1,1)
rslider bounds(150, 25, 70, 70), colour(200,100,50,255), tracker(200,100,50,255), fontcolour("silver"), text("Bandwidth"),    channel("bw"), range(0.1, 10, 1, 0.5, 0.001)
hslider bounds(220, 25,140, 35), colour(200,100,50,255), tracker(200,100,50,255), fontcolour("silver"),    channel("cfmin"), range(50, 10000, 50  ,0.5,0.1)
hslider bounds(220, 50,140, 35), colour(200,100,50,255), tracker(200,100,50,255), fontcolour("silver"),    channel("cfmax"), range(50, 10000, 10000,0.5,0.1)
label   bounds(254, 77, 80, 12), text("Cutoff Freq."), FontColour("white")
rslider bounds(360, 25, 70, 70), colour(200,100,50,255), tracker(200,100,50,255), fontcolour("silver"), text("Int./S&H"),    channel("i_h"), range(0, 1, 0)
}

groupbox bounds( 0,245,220,100), text("WAVE GUIDE"), plant("waveguide"), FontColour(silver),colour(40,40,40){
rslider bounds( 10, 25, 70, 70), colour(150,150,50,255), tracker(150,150,50,255), fontcolour("silver"), text("Chance"),    channel("WguideChnc"), range(0, 1.00, 0.2)
hslider bounds( 80, 25,140, 35), colour(150,150,50,255), tracker(150,150,50,255), fontcolour("silver"),    channel("wguidemin"), range(22, 100, 50,1,1)
hslider bounds( 80, 50,140, 35), colour(150,150,50,255), tracker(150,150,50,255), fontcolour("silver"),    channel("wguidemax"), range(22, 100, 70,1,1)
label   bounds(108, 77, 85, 12), text("Pitch Range"), FontColour("white")
}

groupbox bounds(220,245,220,100), text("SQUARE MOD."), plant("sqmod"), FontColour(silver),colour(40,40,40){
rslider bounds( 10, 25, 70, 70), colour(200,150,200,255), tracker(200,150,200,255), fontcolour("silver"), text("Chance"),    channel("SqModChnc"), range(0, 1.00, 0.2)
hslider bounds( 80, 25,140, 35), colour(200,150,200,255), tracker(200,150,200,255), fontcolour("silver"),    channel("sqmodmin"), range(1, 14.0,  6)
hslider bounds( 80, 50,140, 35), colour(200,150,200,255), tracker(200,150,200,255), fontcolour("silver"),    channel("sqmodmax"), range(1, 14.0, 12)
label   bounds(108, 77, 85, 12), text("Freq.Range"), FontColour("white")
}

groupbox bounds(0,345,220,100), text("FREQUENCY SHIFT"), plant("fshift"), FontColour(silver),colour(40,40,40){
rslider bounds( 10, 25, 70, 70), colour(250,110,250,255), tracker(250,110,250,255), fontcolour("silver"), text("Chance"),    channel("FshiftChnc"), range(0, 1.00, 0.2)
hslider bounds( 80, 25,140, 35), colour(250,110,250,255), tracker(250,110,250,255), fontcolour("silver"),    channel("fshiftmin"), range(-4000, 4000,-1000)
hslider bounds( 80, 50,140, 35), colour(250,110,250,255), tracker(250,110,250,255), fontcolour("silver"),    channel("fshiftmax"), range(-4000, 4000, 1000)
label   bounds(108, 77, 85, 12), text("Freq.Range"), FontColour("white")
}

groupbox bounds(220,345,220,100), text("OUTPUT"), plant("output"), FontColour(silver),colour(40,40,40){
rslider bounds( 10, 25, 70, 70), colour("Tan"), tracker("Tan"), fontcolour("silver"), text("Gain"),        channel("gain"), range(0, 1, 0.75)
rslider bounds( 80, 25, 70, 70), colour("Tan"), tracker("Tan"), fontcolour("silver"), text("Dry/Wet"),        channel("DryWet"), range(0, 1, 0.6)
rslider bounds(150, 25, 70, 70), colour("Tan"), tracker("Tan"), fontcolour("silver"), text("Layers"),        channel("layers"), range(1, 20, 1,1,1)
}
label   bounds( 5,470, 200, 12), text("Author: Iain McCurdy |2013|"), FontColour("grey")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1
seed	0

gisine	ftgen	0,0,131072,10,1

opcode	BBCutIteration,aa,aaiiiiiiiii
 aL,aR,iBPS, isubdiv,  ibarlen,  iphrase, irepeats, istutspd, istutchnc, icount, ilayers	xin
 abbcutL		bbcutm	aL,   iBPS, isubdiv,  ibarlen,  iphrase, irepeats, istutspd, istutchnc
 abbcutR		bbcutm	aR,   iBPS, isubdiv,  ibarlen,  iphrase, irepeats, istutspd, istutchnc
 amixL	=	0
 amixR	=	0
 if icount<ilayers then
   amixL,amixR	BBCutIteration	aL,aR, iBPS, isubdiv,  ibarlen,  iphrase, irepeats, istutspd, istutchnc, icount+1, ilayers
 endif
 xout	abbcutL+amixL,abbcutR+amixL
endop

opcode	FreqShifter,a,aki
	ain,kfshift,ifn	xin					;READ IN INPUT ARGUMENTS
	areal, aimag hilbert ain				;HILBERT OPCODE OUTPUTS TWO PHASE SHIFTED SIGNALS, EACH 90 OUT OF PHASE WITH EACH OTHER
	asin 	oscili       1,    kfshift,     ifn,          0
	acos 	oscili       1,    kfshift,     ifn,          0.25	
	;RING MODULATE EACH SIGNAL USING THE QUADRATURE OSCILLATORS AS MODULATORS
	amod1	=		areal * acos
	amod2	=		aimag * asin	
	;UPSHIFTING OUTPUT
	aFS	= (amod1 - amod2)
		xout	aFS				;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1	; read widgets
 gksubdiv 	chnget	"subdiv"
 gkbarlen 	chnget	"barlen"
 gkphrase 	chnget	"phrase"
 gkrepeats	chnget	"repeats"
 gkstutspd	chnget	"stutspd"
 gkstutchnc	chnget	"stutchnc"
 gkBPM    	chnget	"BPM"
 gkfltdiv	chnget	"fltdiv"
 gkDryWet    	chnget	"DryWet"
 gkFltMix 	chnget	"FltMix"
 gkbw     	chnget	"bw"
 gkcfmin  	chnget	"cfmin"
 gkcfmax  	chnget	"cfmax"
 gki_h    	chnget	"i_h"
 gklayers    	chnget	"layers"
 gkgain		chnget	"gain"
 konoff		chnget	"onoff"
endin

instr	2
 aL,aR ins
 ;aL	diskin	"loop.wav",1,0,1
 ;aR	=	aL
 
 kSwitch	changed		gkBPM, gkrepeats, gkphrase, gkstutspd, gkstutchnc, gkbarlen, gksubdiv, gkfltdiv, gklayers	;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
 if	kSwitch=1	then		;IF I-RATE VARIABLE CHANGE TRIGGER IS '1'...
 	reinit	UPDATE			;BEGIN A REINITIALISATION PASS FROM LABEL 'UPDATE'
 endif
 UPDATE:

 kmetro	metro	i(gkBPM)/60
 
 ;OUTPUT		OPCODE	        INPUT |   BPM      | SUBDIVISION | BAR_LENGTH | PHRASE_LENGTH | NUM.OF_REPEATS | STUTTER_SPEED | STUTTER_CHANCE	
 abbcutL,abbcutR	BBCutIteration	aL,aR, i(gkBPM)/60, i(gksubdiv),  i(gkbarlen),   i(gkphrase),    i(gkrepeats),   i(gkstutspd),   i(gkstutchnc),  1, i(gklayers)
 
 ;FILTER=================================================================================================================================================================
 kfreq	=	(gkfltdiv)	;FREQUENCY WITH WHICH NEW FILTER CUTOFF VALUES ARE GENERATED			;LEFT CHANNE 
 rireturn	;RETURN FROM REINITIALISATION PASS TO PERFORMANCE TIME PASSES
 
 kcf1h	randomh	gkcfmin, gkcfmax, kfreq		;SAMPLE AND HOLD RANDOM FREQUENCY VALUES					;LEFT CHANNEL
 kcf1i	lineto		kcf1h, 1/kfreq		;INTERPOLATE VALUES								;LEFT CHANNEL
 kcf1	ntrpol		kcf1i, kcf1h, gki_h   	;CROSSFADE BETWEEN INTERPOLATING AND SAMPLE AND HOLD TYPE RANDOM VALUES		;LEFT CHANNEL
 abbFltL	resonz	abbcutL, kcf1, kcf1*gkbw, 2	;BAND-PASS FILTER								;LEFT CHANNEL
 abbMixL	ntrpol	abbcutL, abbFltL, gkFltMix	;CROSSFADE BETWEEN UNFILTERED AND FILTER AUDIO SIGNAL				;LEFT CHANNEL
 kcf2h	randomh	gkcfmin, gkcfmax, kfreq		;SAMPLE AND HOLD RANDOM FREQUENCY VALUES					;RIGHT CHANNEL
 kcf2i	lineto		kcf2h, 1/kfreq		;INTERPOLATE VALUES                                                             ;RIGHT CHANNEL
 kcf2	ntrpol		kcf2i, kcf2h, gki_h   	;CROSSFADE BETWEEN INTERPOLATING AND SAMPLE AND HOLD TYPE RANDOM VALUES		;RIGHT CHANNEL
 abbFltR	resonz	abbcutR, kcf2, kcf2*gkbw, 2	;BAND-PASS FILTER								;RIGHT CHANNEL
 abbMixR	ntrpol	abbcutR, abbFltR, gkFltMix	;CROSSFADE BETWEEN UNFILTERED AND FILTER AUDIO SIGNAL				;RIGHT CHANNEL
 ;=======================================================================================================================================================================

 ;WGUIDE1================================================================================================================================================================
 kchance	chnget	"WguideChnc"
 kdice		trandom	kmetro,0,1
 if kdice<kchance then
  kwguidemin	chnget	"wguidemin"
  kwguidemax	chnget	"wguidemax"
  knum		randomh	kwguidemin,kwguidemax,gkBPM/60
  afrq		interp	cpsmidinn(int(knum))
  kfb		randomi	0.8,0.99,gkBPM/(60*4)
  kcf		randomi	800,4000,gkBPM/60
  abbMixL	wguide1	abbMixL*0.7,afrq,kcf,kfb
  abbMixR	wguide1	abbMixR*0.7,afrq,kcf,kfb
 endif  
 ;=======================================================================================================================================================================

 ;SQUARE MOD==============================================================================================================================================================
 kchance	chnget	"SqModChnc"
 ksqmodmin	chnget	"sqmodmin"
 ksqmodmax	chnget	"sqmodmax"
 kDiceRoll	trandom	kmetro,0,1
 if kDiceRoll<kchance then
  kratei	randomi	ksqmodmin,ksqmodmax,gkBPM/60
  krateh	randomh	ksqmodmin,ksqmodmax,gkBPM/60
  kcross	randomi	0,1,gkBPM
  kcross	=	kcross>0.5?1:0
  krate		ntrpol	kratei,krateh,kcross
  amod		lfo	1,cpsoct(krate),2
  abbMixL	=	abbMixL*amod
  abbMixR	=	abbMixR*amod
 endif
 ;=======================================================================================================================================================================

 ;FSHIFT=================================================================================================================================================================
 kchance	chnget	"FshiftChnc"
 kdice		trandom	kmetro,0,1
 if kdice<kchance then
  kfshiftmin	chnget	"fshiftmin"
  kfshiftmax	chnget	"fshiftmax"
;  kfsfrq	randomh	kfshiftmin,kfshiftmax,gkBPM/60
  kfsfrqi	randomi	kfshiftmin,kfshiftmax,gkBPM/60
  kfsfrqh	randomh	kfshiftmin,kfshiftmax,gkBPM/60
  kcross	randomi	0,1,gkBPM/2
  kcross	=	kcross>0.5?1:0
  kfsfrq	ntrpol	kfsfrqi,kfsfrqh,kcross
  abbMixL	FreqShifter	abbMixL,kfsfrq,gisine
  abbMixR	FreqShifter	abbMixR,kfsfrq,gisine
 endif   
 ;=======================================================================================================================================================================
 
 amixL	sum	aL*(1-gkDryWet), abbMixL*gkDryWet	;SUM AND MIX DRY SIGNAL AND BBCUT SIGNAL (LEFT CHANNEL)
 amixR	sum	aR*(1-gkDryWet), abbMixR*gkDryWet	;SUM AND MIX DRY SIGNAL AND BBCUT SIGNAL (RIGHT CHANNEL)




 outs	amixL*gkgain, amixR*gkgain			;SEND AUDIO TO OUTPUTS
endin

</CsInstruments>  
<CsScore>
i1 0 36000
i2 0 36000
</CsScore>
</CsoundSynthesizer>