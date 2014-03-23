<Cabbage>
form caption("Bassline") size(615, 250), pluginID("basl")

snapshot bounds( 80,185,200, 20), preset("bassline"), items("Preset 1", "Preset 2", "Preset 3", "Preset 4", "Preset 5", "Preset 6", "Preset 7", "Preset 8", "Preset 9", "Preset 10")

image    bounds(  0,  0,615,220), colour("grey"), shape("rounded"), outline("white"), line(4)
checkbox bounds( 64,  8,  4,  4), channel("Act1"),  value(0), colour("yellow"), shape("ellipse")
checkbox bounds( 99,  8,  4,  4), channel("Act2"),  value(0), colour("yellow"), shape("ellipse")
checkbox bounds(134,  8,  4,  4), channel("Act3"),  value(0), colour("yellow"), shape("ellipse")
checkbox bounds(169,  8,  4,  4), channel("Act4"),  value(0), colour("yellow"), shape("ellipse")
checkbox bounds(204,  8,  4,  4), channel("Act5"),  value(0), colour("yellow"), shape("ellipse")
checkbox bounds(239,  8,  4,  4), channel("Act6"),  value(0), colour("yellow"), shape("ellipse")
checkbox bounds(274,  8,  4,  4), channel("Act7"),  value(0), colour("yellow"), shape("ellipse")
checkbox bounds(309,  8,  4,  4), channel("Act8"),  value(0), colour("yellow"), shape("ellipse")
checkbox bounds(344,  8,  4,  4), channel("Act9"),  value(0), colour("yellow"), shape("ellipse")
checkbox bounds(379,  8,  4,  4), channel("Act10"), value(0), colour("yellow"), shape("ellipse")
checkbox bounds(414,  8,  4,  4), channel("Act11"), value(0), colour("yellow"), shape("ellipse")
checkbox bounds(449,  8,  4,  4), channel("Act12"), value(0), colour("yellow"), shape("ellipse")
checkbox bounds(484,  8,  4,  4), channel("Act13"), value(0), colour("yellow"), shape("ellipse")
checkbox bounds(519,  8,  4,  4), channel("Act14"), value(0), colour("yellow"), shape("ellipse")
checkbox bounds(554,  8,  4,  4), channel("Act15"), value(0), colour("yellow"), shape("ellipse")
checkbox bounds(589,  8,  4,  4), channel("Act16"), value(0), colour("yellow"), shape("ellipse")

label    bounds( 10, 23, 40, 12), text("Note"), FontColour("black"), colour("black")
rslider  bounds( 45, 15, 45, 45), text("1"), FontColour("black"),  channel("Note1"),  range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds( 80, 15, 45, 45), text("2"),  channel("Note2"), FontColour("black"),  range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(115, 15, 45, 45), text("3"),  channel("Note3"), FontColour("black"),  range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(150, 15, 45, 45), text("4"),  channel("Note4"), FontColour("black"),  range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(185, 15, 45, 45), text("5"),  channel("Note5"), FontColour("black"),  range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(220, 15, 45, 45), text("6"),  channel("Note6"), FontColour("black"),  range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(255, 15, 45, 45), text("7"),  channel("Note7"), FontColour("black"),  range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(290, 15, 45, 45), text("8"),  channel("Note8"), FontColour("black"),  range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(325, 15, 45, 45), text("9"),  channel("Note9"), FontColour("black"),  range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(360, 15, 45, 45), text("10"), channel("Note10"), FontColour("black"), range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(395, 15, 45, 45), text("11"), channel("Note11"), FontColour("black"), range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(430, 15, 45, 45), text("12"), channel("Note12"), FontColour("black"), range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(465, 15, 45, 45), text("13"), channel("Note13"), FontColour("black"), range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(500, 15, 45, 45), text("14"), channel("Note14"), FontColour("black"), range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(535, 15, 45, 45), text("15"), channel("Note15"), FontColour("black"), range(24, 96, 60, 1, 1), preset("bassline")
rslider  bounds(570, 15, 45, 45), text("16"), channel("Note16"), FontColour("black"), range(24, 96, 60, 1, 1), preset("bassline")

label    bounds( 10, 62, 40, 12), text("On/Off"), colour("black"), FontColour("black")
checkbox bounds( 61, 62, 12, 12), channel("On1"),  value(0), preset("bassline")
checkbox bounds( 96, 62, 12, 12), channel("On2"),  value(0), preset("bassline")
checkbox bounds(131, 62, 12, 12), channel("On3"),  value(0), preset("bassline")
checkbox bounds(166, 62, 12, 12), channel("On4"),  value(0), preset("bassline")
checkbox bounds(201, 62, 12, 12), channel("On5"),  value(0), preset("bassline")
checkbox bounds(236, 62, 12, 12), channel("On6"),  value(0), preset("bassline")
checkbox bounds(271, 62, 12, 12), channel("On7"),  value(0), preset("bassline")
checkbox bounds(306, 62, 12, 12), channel("On8"),  value(0), preset("bassline")
checkbox bounds(341, 62, 12, 12), channel("On9"),  value(0), preset("bassline")
checkbox bounds(376, 62, 12, 12), channel("On10"), value(0), preset("bassline")
checkbox bounds(411, 62, 12, 12), channel("On11"), value(0), preset("bassline")
checkbox bounds(446, 62, 12, 12), channel("On12"), value(0), preset("bassline")
checkbox bounds(481, 62, 12, 12), channel("On13"), value(0), preset("bassline")
checkbox bounds(516, 62, 12, 12), channel("On14"), value(0), preset("bassline")
checkbox bounds(551, 62, 12, 12), channel("On15"), value(0), preset("bassline")
checkbox bounds(586, 62, 12, 12), channel("On16"), value(0), preset("bassline")

label    bounds( 11, 80, 40, 12), text("Hold"), colour("black"), FontColour("black")
checkbox bounds( 61, 80, 12, 12), channel("Hold1"),  value(0), preset("bassline")
checkbox bounds( 96, 80, 12, 12), channel("Hold2"),  value(0), preset("bassline")
checkbox bounds(131, 80, 12, 12), channel("Hold3"),  value(0), preset("bassline")
checkbox bounds(166, 80, 12, 12), channel("Hold4"),  value(0), preset("bassline")
checkbox bounds(201, 80, 12, 12), channel("Hold5"),  value(0), preset("bassline")
checkbox bounds(236, 80, 12, 12), channel("Hold6"),  value(0), preset("bassline")
checkbox bounds(271, 80, 12, 12), channel("Hold7"),  value(0), preset("bassline")
checkbox bounds(306, 80, 12, 12), channel("Hold8"),  value(0), preset("bassline")
checkbox bounds(341, 80, 12, 12), channel("Hold9"),  value(0), preset("bassline")
checkbox bounds(376, 80, 12, 12), channel("Hold10"), value(0), preset("bassline")
checkbox bounds(411, 80, 12, 12), channel("Hold11"), value(0), preset("bassline")
checkbox bounds(446, 80, 12, 12), channel("Hold12"), value(0), preset("bassline")
checkbox bounds(481, 80, 12, 12), channel("Hold13"), value(0), preset("bassline")
checkbox bounds(516, 80, 12, 12), channel("Hold14"), value(0), preset("bassline")
checkbox bounds(551, 80, 12, 12), channel("Hold15"), value(0), preset("bassline")
checkbox bounds(586, 80, 12, 12), channel("Hold16"), value(0), preset("bassline")

label    bounds( 11, 98, 40, 12), text("Accent"), colour("black"), FontColour("black")
checkbox bounds( 61, 98, 12, 12), channel("Acc1"),  value(0), preset("bassline")
checkbox bounds( 96, 98, 12, 12), channel("Acc2"),  value(0), preset("bassline")
checkbox bounds(131, 98, 12, 12), channel("Acc3"),  value(0), preset("bassline")
checkbox bounds(166, 98, 12, 12), channel("Acc4"),  value(0), preset("bassline")
checkbox bounds(201, 98, 12, 12), channel("Acc5"),  value(0), preset("bassline")
checkbox bounds(236, 98, 12, 12), channel("Acc6"),  value(0), preset("bassline")
checkbox bounds(271, 98, 12, 12), channel("Acc7"),  value(0), preset("bassline")
checkbox bounds(306, 98, 12, 12), channel("Acc8"),  value(0), preset("bassline")
checkbox bounds(341, 98, 12, 12), channel("Acc9"),  value(0), preset("bassline")
checkbox bounds(376, 98, 12, 12), channel("Acc10"), value(0), preset("bassline")
checkbox bounds(411, 98, 12, 12), channel("Acc11"), value(0), preset("bassline")
checkbox bounds(446, 98, 12, 12), channel("Acc12"), value(0), preset("bassline")
checkbox bounds(481, 98, 12, 12), channel("Acc13"), value(0), preset("bassline")
checkbox bounds(516, 98, 12, 12), channel("Acc14"), value(0), preset("bassline")
checkbox bounds(551, 98, 12, 12), channel("Acc15"), value(0), preset("bassline")
checkbox bounds(586, 98, 12, 12), channel("Acc16"), value(0), preset("bassline")

button   bounds( 10,120, 60, 25), colour("Green"), text("Run","Stop"), channel("OnOff"), value(1)
combobox bounds( 10,150, 60, 15), channel("Waveform"), value(1), text("saw","square","triangle","PWM","noise"), preset("bassline")
rslider  bounds( 20,170, 40, 40), text("P.W."),  channel("pw"),  range(0.01,0.99, 0.25), preset("bassline"), FontColour("black")
rslider  bounds( 70,120, 60, 60), text("Vol."),  channel("Vol"),  range(0, 5.00, 1), preset("bassline"), FontColour("black")
rslider  bounds(120,120, 60, 60), text("Cutoff"),  channel("CfBase"),  range(4.00, 14, 8), preset("bassline"), FontColour("black")
rslider  bounds(170,120, 60, 60), text("Env.Mod"),  channel("CfEnv"),  range(0, 10.00, 4), preset("bassline"), FontColour("black")
rslider  bounds(220,120, 60, 60), text("Res."),  channel("Res"),  range(0,0.98, .4), preset("bassline"), FontColour("black")
rslider  bounds(270,120, 60, 60), text("Dist."),  channel("Dist"),  range(0,1.00, 0), preset("bassline"), FontColour("black")
rslider  bounds(320,120, 60, 60), text("Decay"),  channel("Decay"),  range(0.10, 5, .3, 0.25), preset("bassline"), FontColour("black")
rslider  bounds(370,120, 60, 60), text("Accent"),  channel("Accent"),  range(0, 1.00, .5), preset("bassline"), FontColour("black")
rslider  bounds(420,120, 60, 60), text("Tempo"),  channel("Tempo"),  range(1, 500, 110, 1, 1), FontColour("black"), FontColour("black")
button   bounds(480,120, 60, 15), text("Reset"), channel("Reset"), value(0)
button   bounds(540,120, 60, 15), text("Random"), channel("Rnd"), value(0)
button   bounds(480,140, 60, 15), text("-semi"), channel("SemiDn"), value(0)
button   bounds(540,140, 60, 15), text("+semi"), channel("SemiUp"), value(0)
button   bounds(480,160, 60, 15), text("-oct"), channel("OctDn"), value(0)
button   bounds(540,160, 60, 15), text("+oct"), channel("OctUp"), value(0)

button bounds(280, 185, 80, 20), text("Internal","External"), channel("ClockSource"), value(0)
label  bounds(285, 205, 80, 12), text("Clock Source"), FontColour("black")

image bounds(5, 225, 330, 20), colour(75, 85, 90, 100), plant("credit"){
label bounds(0.03, 0.1, .6, .7), text("Author: Iain McCurdy |2012|"), colour("white")
}

hostbpm channel("bpm")
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
		seed	0
		
;Author: Iain McCurdy (2012)

opcode	moogladdersr,a,akk
	asig,kcf,kres	xin
		setksmps	1
	acf	interp	kcf
	kcf	downsamp	acf
	asig      moogladder   asig, kcf, kres
		xout	asig
endop

opcode	resonsr,a,akki
	asig,kcf,kbw,iscal	xin
		setksmps	1
	acf	interp	kcf
	kcf	downsamp	acf
	abw	interp	kbw
	kbw	downsamp	abw
	asig      reson   asig, kcf, kbw,iscal
		xout	asig
endop

instr	1	;BASSLINE INSTRUMENT
	kporttime	linseg	0,0.001,0.01	;PORTAMENTO TIME RAMPS UP QUICKLY FROM ZERO TO A HELD VALUE
	;READ IN WIDGETS...
	gkOnOff		chnget	"OnOff"
	gkNote1		chnget	"Note1"
	gkNote2		chnget	"Note2"
	gkNote3		chnget	"Note3"
	gkNote4		chnget	"Note4"
	gkNote5		chnget	"Note5"
	gkNote6		chnget	"Note6"
	gkNote7		chnget	"Note7"
	gkNote8		chnget	"Note8"
	gkNote9		chnget	"Note9"
	gkNote10	chnget	"Note10"
	gkNote11	chnget	"Note11"
	gkNote12	chnget	"Note12"
	gkNote13	chnget	"Note13"
	gkNote14	chnget	"Note14"
	gkNote15	chnget	"Note15"
	gkNote16	chnget	"Note16"
	gkOn1		chnget	"On1"
	gkOn2		chnget	"On2"
	gkOn3		chnget	"On3"
	gkOn4		chnget	"On4"
	gkOn5		chnget	"On5"
	gkOn6		chnget	"On6"
	gkOn7		chnget	"On7"
	gkOn8		chnget	"On8"
	gkOn9		chnget	"On9"
	gkOn10		chnget	"On10"
	gkOn11		chnget	"On11"
	gkOn12		chnget	"On12"
	gkOn13		chnget	"On13"
	gkOn14		chnget	"On14"
	gkOn15		chnget	"On15"
	gkOn16		chnget	"On16"
	gkHold1		chnget	"Hold1"
	gkHold2		chnget	"Hold2"
	gkHold3		chnget	"Hold3"
	gkHold4		chnget	"Hold4"
	gkHold5		chnget	"Hold5"
	gkHold6		chnget	"Hold6"
	gkHold7		chnget	"Hold7"
	gkHold8		chnget	"Hold8"
	gkHold9		chnget	"Hold9"
	gkHold10	chnget	"Hold10"
	gkHold11	chnget	"Hold11"
	gkHold12	chnget	"Hold12"
	gkHold13	chnget	"Hold13"
	gkHold14	chnget	"Hold14"
	gkHold15	chnget	"Hold15"
	gkHold16	chnget	"Hold16"
	gkAcc1	chnget	"Acc1"
	gkAcc2	chnget	"Acc2"
	gkAcc3	chnget	"Acc3"
	gkAcc4	chnget	"Acc4"
	gkAcc5	chnget	"Acc5"
	gkAcc6	chnget	"Acc6"
	gkAcc7	chnget	"Acc7"
	gkAcc8	chnget	"Acc8"
	gkAcc9	chnget	"Acc9"
	gkAcc10	chnget	"Acc10"
	gkAcc11	chnget	"Acc11"
	gkAcc12	chnget	"Acc12"
	gkAcc13	chnget	"Acc13"
	gkAcc14	chnget	"Acc14"
	gkAcc15	chnget	"Acc15"
	gkAcc16	chnget	"Acc16"
	gkVol	chnget	"Vol"
	gkCfBase	chnget	"CfBase"
	gkCfBase	portk	gkCfBase,kporttime*5
	gkCfEnv	chnget	"CfEnv"
	gkCfEnv	portk	gkCfEnv,kporttime*5
	gkRes	chnget	"Res"
	gkDist	chnget	"Dist"
	gkDecay	chnget	"Decay"
	gkAccent	chnget	"Accent"
	gkWaveform	chnget	"Waveform"
	kpw	chnget	"pw"
	gkReset	chnget	"Reset"
	gkRnd	chnget	"Rnd"
	kSemiDn	chnget	"SemiDn"
	kSemiUp	chnget	"SemiUp"
	kOctDn	chnget	"OctDn"
	kOctUp	chnget	"OctUp"
	kClockSource	chnget	"ClockSource"
	if kClockSource==0 then
	 gkTempo	chnget	"Tempo"
	else
	 gkTempo	chnget	"bpm"
	 gkTempo	limit	gkTempo,1,2000
	endif
	
	kNonActive	init	0	
	kStop	trigger	gkOnOff,0.5,1	; IS LOOP PLAYING TURNED OFF...
	if kStop==1 then
	 chnset kNonActive,"Act1"
	 chnset kNonActive,"Act2"
	 chnset kNonActive,"Act3"
	 chnset kNonActive,"Act4"
	 chnset kNonActive,"Act5"
	 chnset kNonActive,"Act6"
	 chnset kNonActive,"Act7"
	 chnset kNonActive,"Act8"
	 chnset kNonActive,"Act9"
	 chnset kNonActive,"Act10"
	 chnset kNonActive,"Act11"
	 chnset kNonActive,"Act12"
	 chnset kNonActive,"Act13"
	 chnset kNonActive,"Act14"
	 chnset kNonActive,"Act15"
	 chnset kNonActive,"Act16"
	 reinit RESET_LOOP
	endif
	RESET_LOOP:

	if gkOnOff==0 kgoto SKIP
	kPhFreq   =            gkTempo/240	;FREQUENCY WITH WHICH TO REPEAT THE ENTIRE PHRASE
	kBtFreq   =            gkTempo/15	;FREQUENCY OF EACH 1/16TH NOTE
	kNewNote	metro	kBtFreq		;GENERATES A TRIGGER FOR EACH BEAT
	;ENVELOPES WITH HELD SEGMENTS
	;                      freq     trig      ph  NOTE:1 (dur) 2            3            4            5            6            7            8            9            10            11            12            13            14            15            16             DUMMY
	kNoteLoop lpshold      kPhFreq, 0,        0,  gkNote1, 1,  gkNote2, 1,  gkNote3, 1,  gkNote4, 1,  gkNote5, 1,  gkNote6, 1,  gkNote7, 1,  gkNote8, 1,  gkNote9, 1,  gkNote10, 1,  gkNote11, 1,  gkNote12, 1,  gkNote13, 1,  gkNote4,  1,  gkNote15, 1,  gkNote16,   1  ;,gkNote1	;NEED AN EXTRA 'DUMMY' VALUE
	kOn       lpshold      kPhFreq, 0,        0,  gkOn1,   1,  gkOn2,   1,  gkOn3,   1,  gkOn4,   1,  gkOn5,   1,  gkOn6,   1,  gkOn7,   1,  gkOn8,   1,  gkOn9,   1,  gkOn10,   1,  gkOn11,   1,  gkOn12,   1,  gkOn13,   1,  gkOn14,   1,  gkOn15,   1,  gkOn16,     1  ;,1		;NEED AN EXTRA 'DUMMY' VALUE
	kAcc      lpshold      kPhFreq, 0,        0,  gkAcc1,  1,  gkAcc2,  1,  gkAcc3,  1,  gkAcc4,  1,  gkAcc5,  1,  gkAcc6,  1,  gkAcc7,  1,  gkAcc8,  1,  gkAcc9,  1,  gkAcc10,  1,  gkAcc11,  1,  gkAcc12,  1,  gkAcc13,  1,  gkAcc14,  1,  gkAcc15,  1,  gkAcc16,    1  ;,1		;NEED AN EXTRA 'DUMMY' VALUE
	kHoldLoop lpshold      kPhFreq, 0,        0,  gkHold1, 1,  gkHold2, 1,  gkHold3, 1,  gkHold4, 1,  gkHold5, 1,  gkHold6, 1,  gkHold7, 1,  gkHold8, 1,  gkHold9, 1,  gkHold10, 1,  gkHold11, 1,  gkHold12, 1,  gkHold13, 1,  gkHold14, 1,  gkHold15, 1,  gkHold16,   1  ;,0		;NEED AN EXTRA 'DUMMY' VALUE
	kStep 	  lpshold      kPhFreq, 0,        0,  1,       1,  2,       1,  3,       1,  4,       1,  5,       1,  6,       1,  7,       1,  8,       1,  9,       1,  10,       1,  11,       1,  12,       1,  13,       1,  14,       1,  15,       1,  16,         1  ;,0		;NEED AN EXTRA 'DUMMY' VALUE
	
	kNewOn	trigger	kOn,0.5,0	;SENSE A NOTE ON FOLLOWING A NOTE OFF
	kAcc	portk	kAcc*gkAccent,kporttime	;SCALE ACCENT USING ON-SCREEN KNOB AND SMOOTH CHANGES IN ACCENT VALUE TO PREVENT CLICKS
	
	;IGNORE PITCH VALUES AND HOLD VALUES FOR 'OFF' NOTES
	kNote	init	i(gkNote1)
	if kOn==1 then			;IF THIS IS AN 'ON' NOTE...
	 kNote = kNoteLoop		;USE NOTE LOOP VALUE FOR PCH
	 kHold = kHoldLoop		;USE HOLD LOOP VALUE FOR HOLD
	endif
	
	kHoldDel  vdel_k       kHold, 1/(kBtFreq*2), 1	;OFFSET HOLD SO THAT ITS VALUE WILL OVERLAP THE NEXT NOTE
	kNote      portk        kNote, 0.01*kHoldDel*kOn	;APPLY PORTAMENTO TO PITCH CHANGES - IF NOTE IS NOT HELD, NO PORTAMENTO WILL BE APPLIED
		
	;FILTER ENVELOPE
	kFiltRetrig	=	(1-kHoldDel) * kNewNote * kOn ; + kOnTrig
	;CREATE A FILTER CUTOFF FREQUENCY ENVELOPE. FILTER ATTACK LEVEL IS A SUM OF BASE FREQUENCY, ENVELOPE DEPTH AND ACCENT
	;                       freq    trig                ph v                          t        v         t             v         t(?)
	kCfOct	  loopseg	1/3600, kFiltRetrig+kNewOn, 0, gkCfBase+gkCfEnv+(kAcc*2), gkDecay, gkCfBase, 3600-gkDecay, gkCfBase, 0
	kCfOct    limit        kCfOct, 4, 14	;LIMIT THE CUTOFF FREQUENCY TO BE WITHIN SENSIBLE LIMITS
	;AMPLITUDE ENVELOPE - SEPARATES NOTES THAT ARE NOT HELD
	katt	=	0.02 * (60/gkTempo)
	kdec	=	0.02 * (60/gkTempo)
	;                      freq     trig  ph   v   tim   v   t                      v   t    v  t(?)
	kAmpEnv   loopseg      kBtFreq, 0,    0,   0,  katt, 1,  (1/kBtFreq)-katt-kdec, 1,  kdec,0, 0	;SUSTAIN SEGMENT DURATION (AND THEREFORE ATTACK AND DECAY SEGMENT DURATIONS) ARE DEPENDENT UPON TEMPO
	kAmp	=	(kHoldDel==1?1:kAmpEnv)
	
	;AUDIO OSCILLATOR
	kWavTrig  changed      gkWaveform		;GENERATE A 'BANG' IF WAVEFORM SELECTOR CHANGES
	if kWavTrig=1 then				;IF A 'BANG' HAS BEEN GENERATED...
	 reinit REINIT_VCO				;BEGIN A REINITIALIZATION PASS FROM THE LABEL 'REINIT_VCO'
	endif
	REINIT_VCO:							;A LABEL
	if     i(gkWaveform)==1 then	;SAWTOOTH
	 imode	=	0
	elseif i(gkWaveform)==2 then	;SQUARE
	 imode	=	10
	elseif i(gkWaveform)==3 then	;TRIANGLE
	 imode	=	12
	elseif i(gkWaveform)==4 then	;PWM
	 imode	=	2
	elseif i(gkWaveform)==5 then	;NOISE
	 anoise	pinkish	1
	 aSig	resonsr	anoise,cpsmidinn(kNote),cpsmidinn(kNote)*0.005,1
	 aSig	=	aSig*20
	 goto SKIP_VCO2
	endif
	kpw	portk	kpw,kporttime
	aSig      vco2         0.2, cpsmidinn(kNote), imode, kpw	;GENERATE AUDIO USING VCO OSCILLATOR
	SKIP_VCO2:
	rireturn							;RETURN FROM INITIALIZATION PASS TO PERFORMANCE PASSES

	;FILTER (CALLS UDO: A VERSION OF moogladder IN WHICH CUTOFF FREQUENCY IS MODULATED AT kr=sr) IN ORDER TO PREVENT QUANTISATION NOISE)
	kres	limit	gkRes+(kAcc*0.4),0,0.98				;PREVENT EXCESSIVE RESONANCE THAT COULD RESULT FROM THE COMBINATION OF RESONANCE SETTING AND ACCENTING
	
	/*
	aFilt      moogladdersr   aSig, cpsoct(kCfOct), kres		;FILTER AUDIO 
	aSig       balance        aFilt,aSig
	;DISTORTION
	if gkDist==0 kgoto SKIP_DISTORTION 
	iSclLimit ftgentmp     0, 0, 1024, -16, 1, 1024,  -8, 0.01	;RESCALING CURVE FOR CLIP 'LIMIT' PARAMETER
	iSclGain  ftgentmp     0, 0, 1024, -16, 1, 1024,   4, 10	;RESCALING CURVE FOR GAIN COMPENSATION
	kLimit    table        gkDist, iSclLimit, 1			;READ LIMIT VALUE FROM RESCALING CURVE
	kGain     table        gkDist, iSclGain, 1			;READ GAIN VALUE FROM RESCALING CURVE
	kTrigDist changed      kLimit					;IF LIMIT VALUE CHANGES GENERATE A 'BANG'
	if kTrigDist=1 then						;IF A 'BANG' HAS BEEN GENERATED...
		reinit REINIT_CLIP					;BEGIN A REINITIALIZATION PASS FROM LABEL 'REINIT_CLIP'
	endif
	REINIT_CLIP:
	aSig      clip         aSig, 0, i(kLimit)			;CLIP DISTORT AUDIO SIGNAL
	rireturn							;
	aSig      =            aSig * kGain				;COMPENSATE FOR GAIN LOSS FROM 'CLIP' PROCESSING
	*/
	
	iSclGain  ftgentmp     0, 0, 1024, -16, 1, 1024,   -6, 0.1	;RESCALING CURVE FOR GAIN COMPENSATION
	kGain     table        gkDist, iSclGain, 1			;READ GAIN VALUE FROM RESCALING CURVE
	aSig	lpf18	aSig,cpsoct(kCfOct),kres, (gkDist^2)*20		;LOWPASS
	;aSig	tone	aSig,cpsoct(kCfOct)
	af1	resonz	aSig,150,150,1
	aSig	=	aSig+af1
	aSig	butlp	aSig,4000
	aSig      =            aSig * kGain				;COMPENSATE FOR GAIN LOSS FROM 'CLIP' PROCESSING
	
	SKIP_DISTORTION:
		
	kOn       port         kOn, 0.006				;SMOOTH CHANGES IN ON OFF SWITCHING
	aAmp	interp	((kAmp*kOn)*((kAcc*0.7)+1)*gkVol)		;COMBINE ALL FACTORS THAT CONTRIBUTE TO AMPLITUDE AND INTERPOLATE AND CREATE AN A-RATE VERSION OF THIS RESULT (TO PREVENT QUANTISATION NOISE) 
	aSig	=	aSig * aAmp					;SCALE AUDIO USING AMPLITUDE CONTROL FUNCTION
		outs	aSig, aSig     					;AUDIO SENT TO OUTPUT, APPLY AMP. ENVELOPE, VOLUME CONTROL AND NOTE ON/OFF STATUS

	rireturn
	SKIP:
	
	;RESET SEQUENCE TO BLANK
	ktrig	changed	gkReset
	if ktrig==1 then
	 kval	=	60
	 chnset	kval,"Note1"
	 chnset	kval,"Note2"
	 chnset	kval,"Note3"
	 chnset	kval,"Note4"
	 chnset	kval,"Note5"
	 chnset	kval,"Note6"
	 chnset	kval,"Note7"
	 chnset	kval,"Note8"
	 chnset	kval,"Note9"
	 chnset	kval,"Note10"
	 chnset	kval,"Note11"
	 chnset	kval,"Note12"
	 chnset	kval,"Note13"
	 chnset	kval,"Note14"
	 chnset	kval,"Note15"
	 chnset	kval,"Note16"
	 kval	=	0
	 chnset	kval,"On1"
	 chnset	kval,"On2"
	 chnset	kval,"On3"
	 chnset	kval,"On4"
	 chnset	kval,"On5"
	 chnset	kval,"On6"
	 chnset	kval,"On7"
	 chnset	kval,"On8"
	 chnset	kval,"On9"
	 chnset	kval,"On10"
	 chnset	kval,"On11"
	 chnset	kval,"On12"
	 chnset	kval,"On13"
	 chnset	kval,"On14"
	 chnset	kval,"On15"
	 chnset	kval,"On16"
	 chnset	kval,"Hold1"
	 chnset	kval,"Hold2"
	 chnset	kval,"Hold3"
	 chnset	kval,"Hold4"
	 chnset	kval,"Hold5"
	 chnset	kval,"Hold6"
	 chnset	kval,"Hold7"
	 chnset	kval,"Hold8"
	 chnset	kval,"Hold9"
	 chnset	kval,"Hold10"
	 chnset	kval,"Hold11"
	 chnset	kval,"Hold12"
	 chnset	kval,"Hold13"
	 chnset	kval,"Hold14"
	 chnset	kval,"Hold15"
	 chnset	kval,"Hold16"
	 chnset	kval,"Acc1"
	 chnset	kval,"Acc2"
	 chnset	kval,"Acc3"
	 chnset	kval,"Acc4"
	 chnset	kval,"Acc5"
	 chnset	kval,"Acc6"
	 chnset	kval,"Acc7"
	 chnset	kval,"Acc8"
	 chnset	kval,"Acc9"
	 chnset	kval,"Acc10"
	 chnset	kval,"Acc11"
	 chnset	kval,"Acc12"
	 chnset	kval,"Acc13"
	 chnset	kval,"Acc14"
	 chnset	kval,"Acc15"
	 chnset	kval,"Acc16"
	endif

	;RANDOMISE SEQUENCE
	ktrig	changed	gkRnd
	if ktrig==1 then
#define	RANDOM_STEP(N)
	 #
	 kval	random	48,72.999
	 chnset	int(kval),"Note$N"
	 kval	random	0,1.999
	 chnset	int(kval),"On$N"
	 kval	random	0,1.999
	 chnset	int(kval),"Hold$N"
	 kval	random	0,1.999
	 chnset	int(kval),"Acc$N"
	 #
	 $RANDOM_STEP(1)
	 $RANDOM_STEP(2)
	 $RANDOM_STEP(3)
	 $RANDOM_STEP(4)
	 $RANDOM_STEP(5)
	 $RANDOM_STEP(6)
	 $RANDOM_STEP(7)
	 $RANDOM_STEP(8)
	 $RANDOM_STEP(9)
	 $RANDOM_STEP(10)
	 $RANDOM_STEP(11)
	 $RANDOM_STEP(12)
	 $RANDOM_STEP(13)
	 $RANDOM_STEP(14)
	 $RANDOM_STEP(15)
	 $RANDOM_STEP(16)
	 
	endif
	
	;PROGRESS INDICATORS
	kStepChange	changed	kStep
	kActive	=	1
	kNonActive	=	0
	if kStepChange==1 then
	 if kStep==1 then
	  chnset kActive,"Act1"
	  chnset kNonActive,"Act16"
	 elseif kStep==2 then
	  chnset kActive,"Act2"
	  chnset kNonActive,"Act1"
	 elseif kStep==3 then
	  chnset kActive,"Act3"
	  chnset kNonActive,"Act2"
	 elseif kStep==4 then
	  chnset kActive,"Act4"
	  chnset kNonActive,"Act3"
	 elseif kStep==5 then
	  chnset kActive,"Act5"
	  chnset kNonActive,"Act4"
	 elseif kStep==6 then
	  chnset kActive,"Act6"
	  chnset kNonActive,"Act5"
	 elseif kStep==7 then
	  chnset kActive,"Act7"
	  chnset kNonActive,"Act6"
	 elseif kStep==8 then
	  chnset kActive,"Act8"
	  chnset kNonActive,"Act7"
	 elseif kStep==9 then
	  chnset kActive,"Act9"
	  chnset kNonActive,"Act8"
	 elseif kStep==10 then
	  chnset kActive,"Act10"
	  chnset kNonActive,"Act9"
	 elseif kStep==11 then
	  chnset kActive,"Act11"
	  chnset kNonActive,"Act10"
	 elseif kStep==12 then
	  chnset kActive,"Act12"
	  chnset kNonActive,"Act11"
	 elseif kStep==13 then
	  chnset kActive,"Act13"
	  chnset kNonActive,"Act12"
	 elseif kStep==14 then
	  chnset kActive,"Act14"
	  chnset kNonActive,"Act13"
	 elseif kStep==15 then
	  chnset kActive,"Act15"
	  chnset kNonActive,"Act14"
	 elseif kStep==16 then
	  chnset kActive,"Act16"
	  chnset kNonActive,"Act15"
	 endif
	endif	

	;TRANSPOSE
	ktrig1	changed	kSemiDn
	ktrig2	changed	kSemiUp
	ktrig3	changed	kOctDn
	ktrig4	changed	kOctUp
	if ktrig1+ktrig2+ktrig3+ktrig4==1 then
#define	TRANSPOSE(N)
	 #
	 kval	limit	gkNote$N+kInt,24,96
	 chnset	kval,"Note$N"
	 #
	 if ktrig1==1 then
	  kInt	=	-1
	 elseif ktrig2==1 then
	  kInt	=	1
	 elseif ktrig3==1 then
	  kInt	=	-12
	 elseif ktrig4==1 then
	  kInt	=	12
	 endif	  
	 $TRANSPOSE(1)
	 $TRANSPOSE(2)
	 $TRANSPOSE(3)
	 $TRANSPOSE(4)
	 $TRANSPOSE(5)
	 $TRANSPOSE(6)
	 $TRANSPOSE(7)
	 $TRANSPOSE(8)
	 $TRANSPOSE(9)
	 $TRANSPOSE(10)
	 $TRANSPOSE(11)
	 $TRANSPOSE(12)
	 $TRANSPOSE(13)
	 $TRANSPOSE(14)
	 $TRANSPOSE(15)
	 $TRANSPOSE(16)
	endif
endin
	
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>



























