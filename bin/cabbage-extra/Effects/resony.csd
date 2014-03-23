resony.csd

Resony is an implementation of a stack of second-order bandpass filters whose centre frequencies are arithmetically related. 
The 'bandwidth' and 'scaling mode' parameters are as they are in the reson opcode. 
'inum' (number of filters) defines the number of filters in the stack. 
'ksep' (separation) normally defines the separation between the lowest and highest filter in the stack in octaves. How this relates to what the actual centre frequencies of filters will be differs depending upon which separation mode has been selected. This is explained below. Note that in this example the operation of 'ksep' has been modified slightly to allow the opcode to be better controlled from the GUI. These modifications are clarified below. Separation can be defined in octaves using the knob 'Sep.oct.' or in semitones using the 'Sep.semi.' knob.
'kbf' (base frequency) defines the centre frequency of the first filter. In 'oct.total' separation mode the pitch interval between the base frequency and (base frequency + separation) is divided into equal intervals according to the number of filters that have been selected. Note that no filter is created at the frequency of (base frequency + separation). For example: if separation=1 and num.filters=2, filters will be created at the base frequency and a tritone above the base frequency (i.e. an interval of 1/2 and an octave). I suspect this is a mistake in the opcode implementation so in this example I rescale the separation interval before passing it to the resony opcode so that the interval between the lowest and highest filter in this mode will always be the interval defined in the GUI. A further option I have provided allows the defined interval to be the interval between adjacent filters rather than the interval from lowest to highest. If 'hertz' separation mode is selected behaviour is somewhat curious. I have made some modifications to the values passed to the opcode to make this mode more controllable. Without these modifications, if number of filters is '1' no filters would be created. The frequency relationship between filters in the stack always follows the harmonic series. Both 'Base Frequency' and 'Separation' normally shift this harmonic stack of filters up or down, for this reason I have disabled user control of 'Separation' in this example, instead a value equal to the 'Number of Filters' will always be used for 'Separation'. This ensures that a harmonic stack will always be created built upon 'Base Frequency' as the fundamental. Negative values for 'separation' are allowed whenever 'separation mode' is 'octaves' (if this is the case, the stack of frequencies will extend below the base frequency). Negative values for 'separation' when 'separation mode' is 'hertz' will cause filters to 'explode'. As 'Separation' is fixed at 'Number of Filters' in this example this explosion will not occur.

<Cabbage>
form caption("resony") size(810, 90), pluginID("rsny"), colour(255,100,0)
image    bounds(  0,  0,810, 90), colour("black"), shape("rounded"), outline(255,100,0), line(2) 
button   bounds( 10, 20, 80, 50), text("Live","Noise"), channel("input"), value(0), fontcolour(yellow)
rslider  bounds( 90, 10, 70, 70), text("BF."),             fontcolour("white"), channel("bf"),   range(20, 20000, 909, 0.5), colour(255,100,0,255), trackercolour(255,250,100)
rslider  bounds(160, 10, 70, 70), text("BW."),             fontcolour("white"), channel("bw"),   range(0.01, 1000, 13, 0.5), colour(255,100,0,255), trackercolour(255,250,100)
rslider  bounds(230, 10, 70, 70), text("Num."),            fontcolour("white"), channel("num"),  range(1, 80, 10, 1,1),      colour(255,100,0,255), trackercolour(255,250,100)
rslider  bounds(300, 10, 70, 70), text("Sep.oct."),        fontcolour("white"), channel("sep"),  range(-11, 11, 2),          colour(255,100,0,255), trackercolour(255,250,100)
rslider  bounds(370, 10, 70, 70), text("Sep.semi."),       fontcolour("white"), channel("sep2"), range(-48, 48, 24,1,1),     colour(255,100,0,255), trackercolour(255,250,100)
combobox bounds(450, 10,130, 70), caption("Scaling Mode"), channel("scl"), value(2), text("none", "peak response", "RMS")
combobox bounds(590, 10,130, 70), caption("Separation Mode"), channel("sepmode"), value(1), text("octs.total", "hertz", "octs.adjacent")
rslider  bounds(730, 10, 70, 70), text("Level"),           fontcolour("white"), channel("gain"), range(0.0001, 1, 30, 0.5),  colour(255,100,0,255), trackercolour(255,250,100)
}
image bounds(5, 95, 205, 20), colour(100,100,100,80), plant("credit"), line(0){
label bounds(0.03, 0.1, .95, .7), text("Author: Iain McCurdy |2012|"), fontcolour("white")
}
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	32	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (2=STEREO)
0dbfs		=	1

;Author: Iain McCurdy (2012)

;A UDO IS CREATED WHICH ENCAPSULATES THE MODIFICATIONS TO THE resony OPCODE DISCUSSED IN THIS EXAMPLE 
opcode	resony2,a,akkikii
	ain, kbf, kbw, inum, ksep , isepmode, iscl	xin

	;IF 'Octaves (Total)' MODE SELECTED...
	if isepmode==0 then
	 irescale	divz	inum,inum-1,1	;PREVENT ERROR IS NUMBER OF FILTERS = ZERO
	 ksep = ksep * irescale			;RESCALE SEPARATION
	
	;IF 'Hertz' MODE SELECTED...	
	elseif isepmode==1 then
	 inum	=	inum + 1
	 ksep	=	inum

	;IF 'Octaves (Adjacent)' MODE SELECTED...
	elseif isepmode==2 then 
	 irescale	divz	inum,inum-1,1	;PREVENT ERROR IS NUMBER OF FILTERS = ZERO
	 ksep = ksep * irescale			;RESCALE SEPARATION
	 ksep = ksep * (inum-1)			;RESCALE SEPARATION INTERVAL ACCORDING TO THE NUMBER OF FILTERS CHOSEN
	 isepmode	=	0
	endif

	aout 		resony 	ain, kbf, kbw, inum, ksep , isepmode, iscl, 0
			xout	aout
endop


instr	1	
	kporttime	linseg	0,0.001,0.005,1,0.005	;CREATE A VARIABLE FUNCTION THAT RAPIDLY RAMPS UP TO A SET VALUE	
	
	gkbf	chnget	"bf"
	gkbw	chnget	"bw"
	gkgain	chnget	"gain"
	gknum	chnget	"num"
	gksep	chnget	"sep"
	gksep2	chnget	"sep2"
	gksepmode	chnget	"sepmode"
	gksepmode	=	gksepmode - 1
	gksepmode	init	1
	gkscl	chnget	"scl"
	gkscl	=	gkscl - 1
	gkscl	init	1
	gkinput		chnget	"input"

	kbf	portk	gkbf, kporttime		;SMOOTH MOVEMENT OF SLIDER VARIABLE TO CREATE BASE FREQUENCY

	ksep	portk	gksep, kporttime	;SMOOTH MOVEMENT OF SLIDER VARIABLE

	if gkinput==0 then
	 asigL,asigR	ins
	else
	 asigL	pinkish	1
	 asigR	pinkish	1
	endif

	kSwitch		changed	gkscl, gknum, gksepmode		;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
	if	kSwitch=1	then		;IF I-RATE VARIABLE CHANGE TRIGGER IS '1'...
		reinit	START			;BEGIN A REINITIALISATION PASS FROM LABEL 'START'
	endif
	START:
	
	isepmode	init	i(gksepmode)
	inum		init	i(gknum)	
	iscl		init	i(gkscl)
	
	;CALL resony2 UDO
	aresL 		resony2 asigL, kbf, gkbw, inum, ksep , isepmode, iscl
	aresR 		resony2	asigR, kbf, gkbw, inum, ksep , isepmode, iscl

	rireturn	;RETURN FROM REINITIALISATION PASS TO PERFORMANCE TIME PASSES
			outs	aresL * gkgain, aresR * gkgain	;SEND FILTER OUTPUT TO THE AUDIO OUTPUTS AND SCALE USING THE FLTK SLIDER VARIABLE gkgain
endin

instr	UpdateWidgets
	ksep	chnget	"sep"
	ksep2	chnget	"sep2"
	ktrig1	changed	ksep
	ktrig2	changed	ksep2
	if ktrig1==1 then
	 chnset	ksep*12, "sep2"
	elseif  ktrig2==1 then
	 chnset	ksep2/12, "sep"
	endif
endin

</CsInstruments>

<CsScore>
i 1 0.1 [3600*24*7]
i "UpdateWidgets" 0 3600	;UPDATE SEPARATION DISPLAY BOX
</CsScore>

</CsoundSynthesizer>
