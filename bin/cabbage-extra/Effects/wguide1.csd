; wguide1.csd
; written by Iain McCurdy 2013

; Encapsulation of the 'wguide1' opcode, used for filtering and synthesis.

; frequency of the wave guide can be determined either in hertz of as a note number

; Activating the 'Keyboard Input' switch will allow frequency control from the keyboard (internal or external) and polyphony. 
;  In this mode, 'Frequency' and 'Note Num.' controls will be disabled.

; Source input sound can be either 'live audio input', or synthetic gravel, dust or air.

<Cabbage>
form caption("wguide1") size(415,210), pluginID("WGu1")
image           bounds(0, 0, 415,125), colour(125, 95, 55), shape("sharp"), outline(225,195,155), line(2) 
label   bounds(227,  4,180, 22), text("W A V E G U I D E"), fontcolour(silver)
image   bounds(228,  8,175,  1), colour(200,170,130), shape("sharp"), outline(silver), line(1)
image   bounds(231, 22,172,  1), colour(200,170,130), shape("sharp"), outline(silver), line(1)
checkbox bounds( 20, 10,120, 17), text("Keyboard Input"), channel("input") fontcolour("white") colour(yellow) value(0)
label    bounds(153,  5, 40, 11), text("Source"), fontcolour("white")
combobox bounds(140, 16, 62, 16), channel("source"), value(1), text("Live","Gravel","Dust","Air")
rslider  bounds( 10, 41, 70, 70),  text("Frequency"),  channel("freq"),      trackercolour(225,195,155), range(8.2, 12542, 160, 0.25), colour( 85, 55,15), fontcolour(white)
rslider  bounds( 75, 41, 70, 70),  text("Note Num."),  channel("notnum"),    trackercolour(225,195,155), range(0, 127, 51, 1,1),     colour( 85, 55,15), fontcolour(white)
rslider  bounds(140, 41, 70, 70),  text("Cutoff"),     channel("cutoff"),    trackercolour(225,195,155), range(20,20000,8000,0.25),  colour( 85, 55,15), fontcolour(white)
rslider  bounds(205, 41, 70, 70),  text("Feedback"),   channel("feedback"),  trackercolour(225,195,155), range(-0.99999, 0.99999, 0.8),  colour( 85, 55,15), fontcolour(white)
rslider  bounds(270, 41, 70, 70),  text("Mix"),        channel("mix"),       trackercolour(225,195,155), range(0, 1.00, 1),        colour( 85, 55,15), fontcolour(white)
rslider  bounds(335, 41, 70, 70),  text("Level"),      channel("level"),     trackercolour(225,195,155), range(0, 1.00, 0.7),        colour( 85, 55,15), fontcolour(white)
keyboard bounds(  0,125, 415, 85)

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
;-d -n
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	32	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (2=STEREO)
0dbfs		=	1
massign	0,2

;Author: Iain McCurdy (2012)


instr	1
	gkinput	chnget	"input"
	event_i	"i",2,0,-1
	ktrig	trigger	gkinput,0.5,2
	if ktrig=1 then
	 if gkinput=0 then
	  event	"i",2,0,-1
	 else
	  turnoff2	2,0,0
	 endif
	endif

	/* SOURCE SOUND */
	ksource	chnget	"source"
	if ksource=1 then
 	 gasigL, gasigR	ins
 	elseif ksource=2 then
 	 gasigL	gausstrig	0.5, 100, 8, 0
 	 gasigR	gausstrig	0.5, 100, 8, 0
 	elseif ksource=3 then
 	 kdens	random	475,525
	 gasigL	dust2	0.5, kdens
 	 kdens	random	525,475
	 gasigR	dust2	0.5, kdens
 	elseif ksource=4 then
	 gasigL	pinkish	0.2
	 gasigR	pinkish	0.2
	endif
	
	kactive	active	2
	printk2	kactive
endin



instr	2
	gkfreq		chnget	"freq"					;READ WIDGETS...
	gknotnum	chnget	"notnum"				;
	ktrig1		changed	gkfreq
	ktrig2		changed	gknotnum

	/* MIDI AND GUI INTEROPERABILITY */
	iMIDIflag	=	0	;IF MIDI ACTIVATED = 1, NON-MIDI = 0
	mididefault	1, iMIDIflag	;IF NOTE IS MIDI ACTIVATED REPLACE iMIDIflag WITH '1'

	if iMIDIflag==1 then				;IF THIS IS A MIDI ACTIVATED NOTE...
	 inum	notnum
	 p1	=	p1 + (rnd(1000)*0.0001)
	 if gkinput=0 then
	  turnoff
	 endif
	 icps	cpsmidi					;READ MIDI PITCH VALUES - THIS VALUE CAN BE MAPPED TO GRAIN DENSITY AND/OR PITCH DEPENDING ON THE SETTING OF THE MIDI MAPPING SWITCHES
	 gkfreq	=		icps
	else						;OTHERWISE WHEN NON-MIDI
	 if ktrig1=1 then				;DUAL FREQUENCY AND NOTE NUMBER CONTROLS
	  koct	=	octcps(gkfreq)
	  chnset	(koct-3)*12,"notnum"
	 elseif ktrig2=1 then
	  chnset	cpsmidinn(gknotnum),"freq"
	 endif
	endif						;END OF THIS CONDITIONAL BRANCH

	gkcutoff	chnget	"cutoff"				;
	gkfeedback	chnget	"feedback"				;
	gkmix		chnget	"mix"					;
	gklevel		chnget	"level"					;
	kporttime	linseg	0,0.01,0.03				;CREATE A VARIABLE THAT WILL BE USED FOR PORTAMENTO TIME
	gkfreq		portk	gkfreq,kporttime
	afreq		interp	gkfreq
	aenv		linsegr	0,0.05,1,0.05,0
	aresL 		wguide1 gasigL*aenv, afreq, gkcutoff, gkfeedback
	aresR 		wguide1 gasigR*aenv, afreq, gkcutoff, gkfeedback
	aresL 		dcblock	aresL
	aresR 		dcblock	aresR
	gkmix		portk	gkmix,kporttime
	amixL		ntrpol	gasigL*aenv, aresL, gkmix
	amixR		ntrpol	gasigR*aenv, aresR, gkmix
	gklevel		portk	gklevel,kporttime
	kenv		linsegr	1,15,0
	amixL		=	amixL * gklevel * kenv
	amixR		=	amixR * gklevel * kenv
	ktime		timeinsts
	krms		rms	amixL,3
	if krms<0.00001&&ktime>0.2&&iMIDIflag==1 then
	 turnoff2	p1,4,0
	endif
			outs	amixL, amixR		;WGUIDE1 OUTPUTS ARE SENT OUT
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>


</CsoundSynthesizer>



























