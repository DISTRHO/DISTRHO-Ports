StutterFilter.csd
Written by Iain McCurdy |2013|

<Cabbage>
form caption("Stutter Filter") size(800, 510), pluginID("stfl")

image            bounds(290, 475,225, 30), colour(100,100,100), shape("rounded"), outline("white"), line(4) 
snapshot bounds(305,480,200, 20), preset("StutterFilter"), master(1), items("Preset 1", "Preset 2", "Preset 3", "Preset 4", "Preset 5", "Preset 6", "Preset 7", "Preset 8", "Preset 9", "Preset 10")
label   bounds(  8, 496, 200, 12), text("Iain McCurdy [2013]"), FontColour("grey")


; METRONOME
groupbox bounds(  0,  0, 80, 60), text("INPUT"), plant("input"), FontColour(silver),colour(50,50,50){
checkbox bounds(  5, 30, 80, 13), text("Test Tone") channel("TestTone"), FontColour("White"), colour(255,100,100)  value(1)
}


; METRONOME
groupbox bounds(  0, 60, 80,410), text("TEMPO"), plant("metronome"), FontColour(silver),colour(50,50,50){
checkbox bounds( 10, 23, 80, 13), text("Start") channel("MetroStart"), FontColour("White"), colour(lightblue)  value(1)
rslider  bounds(  5, 40, 70, 70), colour(200,200,250,220), text("BPM"),       channel("tempo"), range(1, 500, 60)
rslider  bounds(  5,120, 70, 70), colour(200,200,250,220), text("Jitter"), channel("ratejit"), range(0, 4.00, 1.4)
}

; AMPLITUDE ENVELOPE
groupbox bounds( 80, 0,  80,470), text("AMP ENV"), plant("amplitudeenvelope"), FontColour(silver),colour(60,60,60){
rslider  bounds(  5, 25, 70, 70), colour(200,200,250,220), text("Jitter"), channel("ampjit"), range(0, 1.00, 0)
rslider  bounds(  5,105, 70, 70), colour(200,200,250,220), text("Max.Dur."), channel("maxdur"), range(0.01, 5.00, 0.1, 0.5, 0.01)
rslider  bounds(  5,185, 70, 70), colour(200,200,250,220), text("Smoothing"), channel("smoothing"), range(0.005, 0.01, 0.005, 1, 0.0001)
combobox bounds(  5,265, 70, 20), channel("shape"), value(1), text("Square", "Inv.Sq.","Smooth","Lin.","Exp.")
rslider  bounds(  5,295, 70, 70), colour(200,200,250,220), text("Layer Spread"), channel("LayerSpread"), range(0, 1.00, 0)
}

; FILTER ENVELOPE
groupbox bounds(160, 0, 80, 470), text("FILT ENV"), plant("filterenvelope"), FontColour(silver),colour(30,30,30){
checkbox bounds( 10, 23, 70, 13), text("On/Off") channel("EnvFlt"), FontColour("White"), colour("lime")  value(0)
rslider  bounds(  5, 40, 70, 70), colour(200,200,250,220), text("Start"), channel("EnvStart"), range(0, 1.00, 1)
rslider  bounds(  5,120, 70, 70), colour(200,200,250,220), text("End"), channel("EnvEnd"), range(0, 1.00,0)
}

; FILTER 
groupbox bounds(240, 0, 80, 470), text("FILTER"), plant("filter"), FontColour(silver),colour(40,40,40){
checkbox bounds( 10, 23, 70, 13), text("On/Off") channel("FiltOnOff"), FontColour("White"), colour(lightblue)  value(1)
rslider  bounds(  5, 40, 70, 70), colour(200,200,250,220), text("Filt.Freq."), channel("cfoct"), range(4.00, 14, 8)
rslider  bounds(  5,120, 70, 70), colour(200,200,250,220), text("Filt.Jitter"), channel("cfjit"), range(0, 7.00, 2.83)
rslider  bounds(  5,200, 70, 70), colour(200,200,250,220), text("Q"), channel("bw"), range(0.01, 5, 0.5, 0.5,0.001)
rslider  bounds(  5,280, 70, 70), colour(200,200,250,220), text("Q jit"), channel("bwjit"), range(0, 4.00, 0)
rslider  bounds(  5,360, 70, 70), colour(200,200,250,220), text("Filt.Spline"), channel("fltspline"), range(0, 2.00, 0)
combobox bounds(  5,440, 70, 20), channel("FiltType"), value(1), text("resonz", "reson","Butlp","Moog")
}



; PITCH SHIFTER
groupbox bounds(320, 0, 80, 470), text("PITCH"), plant("pitchshifter"), FontColour(silver),colour(70,70,70){
checkbox bounds( 10, 23, 70, 13), text("On/Off") channel("PitchOnOff"), FontColour("White"), colour("lightblue")  value(0)
rslider  bounds(  5, 40, 70, 70), colour(200,200,250,220), text("Min"), channel("pitchmin"), range(-96.00, 96, -6)
rslider  bounds(  5,120, 70, 70), colour(200,200,250,220), text("Max"), channel("pitchmax"), range(-96.00, 96, 12)
checkbox bounds(  5,200, 70, 15), text("Quantize") channel("pchquant"), FontColour("White"), colour("lime")  value(0)
rslider  bounds(  5,220, 70, 70), colour(200,200,250,220), text("Granularity"), channel("dlt"), range(0.001, 5, 0.2, 0.375, 0.0001)
}

; WAVESET
groupbox bounds(400, 0, 80, 470), text("WAVESET"), plant("waveset"), FontColour(silver),colour(45,45,45){
checkbox bounds( 10, 23, 70, 13), text("On/Off") channel("WsetOnOff"), FontColour("White"), colour("lightblue")  value(0)
rslider  bounds(  5, 40, 70, 70), colour(200,200,250,220), text("Chance"), channel("wsetchance"), range(0, 1.00, 0.5)
rslider  bounds(  5,120, 70, 70), colour(200,200,250,220), text("Min"), channel("wsetmin"), range(1, 200, 1, 1,1)
rslider  bounds(  5,200, 70, 70), colour(200,200,250,220), text("Max"), channel("wsetmax"), range(1, 200,10, 1,1)
}

; FREQUENCY SHIFTER
groupbox bounds(480, 0, 82, 470), text("F.SHIFT"), plant("frequencyshifter"), FontColour(silver),colour(55,55,55){
checkbox bounds( 10, 23, 70, 13), text("On/Off") channel("FshiftOnOff"), FontColour("White"), colour("lightblue")  value(0)
rslider  bounds(  5, 40, 70, 70), colour(200,200,250,220), text("Chance"), channel("fshiftchance"), range(0, 1.00, 0.5)
rslider  bounds(  5,120, 70, 70), colour(200,200,250,220), text("Shift"), channel("fshift"), range(-4000, 4000, 0)
rslider  bounds(  5,200, 70, 70), colour(200,200,250,220), text("Jitter"), channel("fshiftjit"), range( 0, 4000, 2000)
}

; WAVEGUIDE
groupbox bounds(560,  0,160, 470), text("WAVEGUIDE"), plant("waveguide"), FontColour(silver),colour(35,35,35){
checkbox bounds( 10, 23, 70, 13), text("On/Off") channel("WguideOnOff"), FontColour("White"), colour("lightblue")  value(0)
checkbox bounds( 10, 80, 70, 15), text("Quantize") channel("wgquant"), FontColour("White"), colour("lime")  value(0)
rslider  bounds( 80, 25, 70, 70), colour(200,200,250,220), text("Chance"), channel("wgchance"), range(0, 1.00, 0.5)
rslider  bounds(  5,105, 70, 70), colour(200,200,250,220), text("Freq.Min"), channel("wgfreqmin"), range(0.00,127, 60)
rslider  bounds( 80,105, 70, 70), colour(200,200,250,220), text("Freq.Max"), channel("wgfreqmax"), range(0.00,127, 100)
rslider  bounds(  5,185, 70, 70), colour(200,200,250,220), text("F.back.Min"), channel("wgFBmin"), range(0, 0.999, 0.9, 2, 0.0001)
rslider  bounds( 80,185, 70, 70), colour(200,200,250,220), text("F.back.Max"), channel("wgFBmax"), range(0, 0.999, 0.95, 2, 0.0001)
rslider  bounds(  5,265, 70, 70), colour(200,200,250,220), text("Damp.Min"), channel("wgCFmin"), range(4, 14, 8)
rslider  bounds( 80,265, 70, 70), colour(200,200,250,220), text("Damp.Max"), channel("wgCFmax"), range(4, 14, 13)
}

; PANNING
groupbox bounds(720, 0, 80, 185), text("PANNING"), plant("panning"), FontColour(silver),colour(65,65,65){
rslider  bounds(  5,  25, 70, 70), colour(200,200,250,220), text("L-R"), channel("pan"), range(0.00, 1, 0.5)
rslider  bounds(  5,105, 70, 70), colour(200,200,250,220), text("Spread"), channel("panjit"), range(0.00, 0.5, 0.25)
}

; OUTPUT
groupbox bounds(720,185, 80, 285), text("OUTPUT"), plant("output"), FontColour(silver),colour(50,50,50){
rslider  bounds(  5, 25, 70, 70), colour(200,200,250,220), text("Gain"), channel("gain"), range(0, 50.00, 1, 0.5, 0.001)
rslider  bounds(  5,105, 70, 70), colour(200,200,250,220), text("Layers"), channel("layers"), range(1, 20, 1, 1, 1)
}

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n
</CsOptions>

<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1
seed	0

;Author: Iain McCurdy (2012)


gisq	ftgen	0,0,32768,7, 0,1,0, 1,1, 32768-3,1, 1,0
gisine	ftgen	0,0,32768,10,1
gihanning	ftgen	0,0,32768,20,2

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

opcode	PitchShifterDelaySt,aa,aakkki
	ainL,ainR,ktrans,kdlt,kFB,imaxdlt	xin
	adlt	interp	kdlt					;INTERPOLATE DELAY TIME CONTROL TO CREATE A-RATE VERSION
	ihalfsine	ftgen	0, 0, 1025, 9, 0.5, 1, 0	;HALF SINE  WINDOW FUNCTION USED FOR AMPLITUDE ENVELOPING
	koctfract	=	ktrans/12			;TRANSPOSITION AS FRACTION OF AN OCTAVE
	kratio		=	octave(koctfract)		;RATIO TO PRODUCE PITCH SHIFT
	krate		=	(kratio-1)/kdlt			;SUBTRACT 1/1 SPEED
	
	aphase1		phasor	-krate				;MOVING PHASE 1-0
	aphase2		phasor	-krate, .5			;MOVING PHASE 1-0 - PHASE OFFSET BY 180 DEGREES (.5 RADIANS)
	
	agate1		tablei	aphase1, ihalfsine, 1, 0, 1	;WINDOW FUNC =HALF SINE
	agate2		tablei	aphase2, ihalfsine, 1, 0, 1	;WINDOW FUNC =HALF SINE

	aoutL,aoutR	init	0	;INITIALISE OUTPUT SIGNAL (NEEDED IN ORDER TO CREATE FEEDBACK SIGNAL)
	iws		=	4	;WINDOW SIZE FOR INTERPOLATION - INCREASE THIS FOR IMPROVED QUALITY AT SOME CPU COST 
	aout1L, aout1R	vdelayxs ainL+(aoutL*kFB), ainR+(aoutR*kFB), aphase1*adlt, imaxdlt, iws
	aout2L, aout2R	vdelayxs ainL+(aoutL*kFB), ainR+(aoutR*kFB), aphase2*adlt, imaxdlt, iws
	
	aoutL		=	((aout1L * agate1)+(aout2L * agate2))*0.5
	aoutR		=	((aout1R * agate1)+(aout2R * agate2))*0.5
			
			xout	aoutL, aoutR		;SUM AND RESCALE PITCH SHIFTER OUTPUTS (LEFT CHANNEL)
endop


opcode	StutterFilter,aa,aakkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkii
ain1,ain2,kMetroStart,ktempo,kratejit1,kampjit1,kmaxdur,ksmoothing,kEnvFlt,kshape,kLayerSpread,kEnvStart,kEnvEnd,kFiltOnOff,kcfoct1,kcfjit1,kbw1,kbwjit1,kfltspline1,kFiltType,kpan1,\
					kpanjit1,kPitchOnOff,kpitchmin,kpitchmax,kpchquant,kdlt,kWsetOnOff,kwsetchance,kwsetmin1,kwsetmax1,kFshiftOnOff,kfshiftchance,kfshift1,kfshiftjit1,\
					kWguideOnOff,kwgchance,kwgfreqmin1,kwgfreqmax1,kwgquant,kwgFBmin1,kwgFBmax1,kwgCFmin1,kwgCFmax1,icount,ilayers	xin
a1	=	ain1
a2	=	ain2

krate		=	ktempo/15		; krate (assuming 4 beats per bar)



/* PORTAMENTO */
kporttime	linseg	0,0.001,0.01
kbw	portk	kbw1,kporttime





/* TRIGGER RANDOM VALUES */
kmetro   init    1
kratejit	trandom	kmetro, -kratejit1, kratejit1
kampjit		trandom	kmetro, 0, kampjit1
ktrans		trandom	kmetro, kpitchmin, kpitchmax
kwsetrep	trandom	kmetro, kwsetmin1+0.999999, kwsetmax1+0.999999
kwsetrep	=	int(kwsetrep)
kfshiftjit	trandom	kmetro, -octcps(kfshiftjit1), octcps(kfshiftjit1)
ksign		=	(kfshiftjit<0?-1:1)
kfshiftjit	=	cpsoct(abs(kfshiftjit))*ksign
kwgfreq		trandom	kmetro, kwgfreqmin1, kwgfreqmax1
kwgFB		trandom	kmetro, kwgFBmin1, kwgFBmax1
kwgCF		trandom	kmetro, kwgCFmin1, kwgCFmax1




/* FILTER */
if kFiltOnOff=0 kgoto SKIP_FILTER
 kcfoct	portk	kcfoct1,kporttime
if kfltspline1==0 then							; if filt.spline switch is at zero...
 kcfjit		trandom	kmetro, 0, kcfjit1				; ...
 kbwjit		trandom	kmetro, 0, kbwjit1
 kcfoct	limit	kcfoct+kcfjit,4,14
 kcf	=	cpsoct(kcfoct)
 kbw	=	kcf*(kbw*octave(kbwjit))
else									; otherwise (filt.spline is greater than zero)...
 kfltspline	limit	kfltspline1,0.1,10				; ...
 kcfoct	randomi	kcfoct-kcfjit,kcfoct+kcfjit,kfltspline			;
 kcf	limit	cpsoct(kcfoct),50,sr/3
 kbwjit	rspline	0,kbwjit,kfltspline*0.5,kfltspline
 kbw	=	kcf*(kbw*octave(kbwjit))
endif
 if kFiltType=1 then
  a1	resonr	a1,kcf,kbw,1
  a2	resonr	a2,kcf,kbw,1
 elseif kFiltType=2 then
  a1	reson	a1,kcf,kbw,1
  a2	reson	a2,kcf,kbw,1
 elseif kFiltType=3 then
  a1	clfilt	a1,kcf,0,2
  a2	clfilt	a2,kcf,0,2
 elseif kFiltType=4 then
  kbw chnget "bw"
  kbw	limit	(kbw-0.01)/(5-0.01),0,0.99
  a1	moogladder	a1,kcf,kbw
  a2	moogladder	a2,kcf,kbw
 endif
SKIP_FILTER:
 
 
 
/* METRONOME */
krate	limit	krate * octave(kratejit),0,50
kmetro	metro	krate*kMetroStart



/* PITCH SHIFTER */
if kPitchOnOff=0 kgoto SKIP_PITCH
kFB	=	0
imaxdlt	=	5
if kpchquant=1 then
 ktrans	=	int(ktrans)
endif
a1,a2	PitchShifterDelaySt	a1,a2,ktrans,kdlt,kFB,imaxdlt
SKIP_PITCH:




/* FREQUENCY SHIFTER */
if kFshiftOnOff=0 kgoto SKIP_FSHIFT
kdice	trandom	kmetro,0,1
if kdice<kfshiftchance then
 kfshift	=	kfshift1+kfshiftjit
 if kshape=2 then
  kfshift	portk	kfshift,kporttime
 endif
 a1	FreqShifter	a1,kfshift,gisine
 a2	FreqShifter	a2,kfshift,gisine
endif
SKIP_FSHIFT:




/* WAVESET */
if kWsetOnOff=0 kgoto SKIP_WAVESET
if kmetro=1 then
 reinit RESET_WAVESET
endif
RESET_WAVESET:
awset1 	waveset a1,    kwsetrep, 3600
awset2 	waveset a2,    kwsetrep, 3600
rireturn
kdice	trandom	kmetro,0,1
kamp	init	0
if kwsetrep>1&&kdice<kwsetchance then
 a1	=	awset1*0.7
 a2	=	awset2*0.7 
endif
SKIP_WAVESET:


/* AMPLITUDE ENVELOPE */
if kmetro=1 then
 reinit	RESTART_ENV
endif
RESTART_ENV:
idur	limit	1/i(krate),0,i(kmaxdur)
iEnvAmp	=	(1-i(kampjit))^2
if i(kshape)=1 then												; square
 kamp	linseg	0,i(ksmoothing),iEnvAmp, (idur*0.5)-(i(ksmoothing)*2), iEnvAmp, i(ksmoothing), 0
elseif i(kshape)=2 then												; inverse square
 kamp	linseg	1,i(ksmoothing),1-iEnvAmp, (idur*0.5)-(2*i(ksmoothing)), 1-iEnvAmp, i(ksmoothing), 1		
elseif i(kshape)=3 then												; hanning window
 kamp	oscil1i 0, iEnvAmp, idur, gihanning
elseif i(kshape)=4 then												; linear decay
 kamp	linseg	0,i(ksmoothing),iEnvAmp, (idur*0.5)-i(ksmoothing), 0
elseif i(kshape)=5 then												; exponential decay
 kamp	transeg	0,i(ksmoothing),2,iEnvAmp, (idur*0.5)-i(ksmoothing),-2,0
endif
kamp	portk	kamp,kporttime *0.01
aamp	interp	kamp					; INTERPOLATE K-RATE LFO AND CREATE A-RATE VARIABLE


/* FILTER ENVELOPE */
if kEnvFlt=1 then					
 kenv	=	((kamp*kEnvStart)*(kEnvStart-kEnvEnd))+kEnvEnd
 a1	clfilt	a1,cpsoct(4+(kenv*10)),0,2
 a2	clfilt	a2,cpsoct(4+(kenv*10)),0,2
endif
rireturn


/* PANNING */
kpanjit    trandom    kmetro, -kpanjit1, kpanjit1
kpan	limit	kpan1+kpanjit,0,1
a1	=	a1*aamp*kpan				
a2	=	a2*aamp*(1-kpan)			


/* WGUIDE */
if kWguideOnOff=0 kgoto SKIP_WAVEGUIDE
kchanceval	trandom	kmetro, 0, 1
if kchanceval<kwgchance then
 if kwgquant=1 then
  kwgfreq	=	int(kwgfreq)
 endif
 kwgfreq	portk	cpsmidinn(kwgfreq),kporttime*0.01
 awgfreq	interp	kwgfreq
 a1	wguide1	a1, awgfreq,cpsoct(kwgCF),kwgFB
 a2	wguide1	a2, awgfreq,cpsoct(kwgCF),kwgFB
 a1	dcblock2	a1
 a2	dcblock2	a2
endif
SKIP_WAVEGUIDE:


kDelTim	=	((icount-1) * ((1/krate)/ilayers))*(kLayerSpread*2)

;kDelTim	portk	kDelTim, kporttime*8

if kDelTim>0 then
 aDelTim	interp	kDelTim
 a1, a2 vdelayxs a1, a2, aDelTim, 10, 4
endif


amix1	=	0
amix2	=	0

if icount<ilayers then
 amix1,amix2	StutterFilter	ain1,ain2,kMetroStart,ktempo,kratejit1,kampjit1,kmaxdur,ksmoothing,kEnvFlt,kshape,kLayerSpread,kEnvStart,kEnvEnd,kFiltOnOff,kcfoct1,kcfjit1,kbw1,kbwjit1,kfltspline1,kFiltType,kpan1,\
					kpanjit1,kPitchOnOff,kpitchmin,kpitchmax,kpchquant,kdlt,kWsetOnOff,kwsetchance,kwsetmin1,kwsetmax1,kFshiftOnOff,kfshiftchance,kfshift1,kfshiftjit1,\
					kWguideOnOff,kwgchance,kwgfreqmin1,kwgfreqmax1,kwgquant,kwgFBmin1,kwgFBmax1,kwgCFmin1,kwgCFmax1,icount+1,ilayers
endif

	xout	a1+amix1,a2+amix2


endop


















instr	1
klayers	chnget	"layers"		;
kTestTone	chnget	"TestTone"

/* METRONOME */
kMetroStart	chnget	"MetroStart"		; read in widgets
ktempo		chnget	"tempo"			; 
kratejit	chnget	"ratejit"		;

/* AMPLITUDE ENVELOPE */
kampjit 	chnget	"ampjit"		;
kmaxdur 	chnget	"maxdur"		;
ksmoothing 	chnget	"smoothing"		;
kEnvFlt		chnget	"EnvFlt"		;
kshape		chnget	"shape"			;
kshape		init	1			; ensure not zero at i-time
kLayerSpread	chnget	"LayerSpread"		;
/* FILTER ENVELOPE */
kEnvStart	chnget	"EnvStart"		;
kEnvEnd		chnget	"EnvEnd"		;
/* FILTER */
kFiltOnOff	chnget	"FiltOnOff"		;
kcfoct		chnget	"cfoct"			;
kcfjit		chnget	"cfjit"			;
kbw		chnget	"bw"			;
kbwjit		chnget	"bwjit"			;
kfltspline	chnget	"fltspline"		;
kFiltType	chnget	"FiltType"		;
kFiltType	init	1			; ensure not zero at i-time
/* PANNING */
kpan		chnget	"pan"
kpanjit		chnget	"panjit"
/* PITCH SHIFTER */
kPitchOnOff	chnget	"PitchOnOff"
kpitchmin	chnget	"pitchmin"
kpitchmax	chnget	"pitchmax"
kpchquant	chnget	"pchquant"
kdlt		chnget	"dlt"
/* WAVESET */
kWsetOnOff	chnget	"WsetOnOff"
kwsetchance	chnget	"wsetchance"
kwsetmin	chnget	"wsetmin"
kwsetmax	chnget	"wsetmax"


/* FREQUENCY SHIFTER */
kFshiftOnOff	chnget	"FshiftOnOff"
kfshiftchance	chnget	"fshiftchance"
kfshift		chnget	"fshift"
kfshiftjit	chnget	"fshiftjit"
/* WAVEGUIDE */
kWguideOnOff	chnget	"WguideOnOff"
kwgchance	chnget	"wgchance"
kwgfreqmin	chnget	"wgfreqmin"
kwgfreqmax	chnget	"wgfreqmax"
kwgquant	chnget	"wgquant"
kwgFBmin	chnget	"wgFBmin"
kwgFBmax	chnget	"wgFBmax"
kwgCFmin	chnget	"wgCFmin"
kwgCFmax	chnget	"wgCFmax"
/* OUTPUT */
kgain		chnget	"gain"



/* SOUND INPUT */
if kTestTone=1 then
 a1	vco2	1,100
 a2	=	a1
else
 a1,a2	ins
 a1,a2	diskin2	"Songpan.wav",1,0,1
endif 

ktrig	changed	klayers
if ktrig==1 then
 reinit RESTART_LAYERS
endif
RESTART_LAYERS:
iGainScale	=	i(klayers)^0.3
print	iGainScale

a1,a2	StutterFilter	a1,a2,kMetroStart,ktempo,kratejit,kampjit,kmaxdur,ksmoothing,kEnvFlt,kshape,kLayerSpread,kEnvStart,kEnvEnd,kFiltOnOff,kcfoct,kcfjit,kbw,kbwjit,kfltspline,kFiltType,kpan,\
					kpanjit,kPitchOnOff,kpitchmin,kpitchmax,kpchquant,kdlt,kWsetOnOff,kwsetchance,kwsetmin,kwsetmax,kFshiftOnOff,kfshiftchance,kfshift,kfshiftjit,\
					kWguideOnOff,kwgchance,kwgfreqmin,kwgfreqmax,kwgquant,kwgFBmin,kwgFBmax,kwgCFmin,kwgCFmax,1,i(klayers)
rireturn

	outs		(a1*kgain)/iGainScale, (a2*kgain)/iGainScale	; scale amplitude according to the number of layers present
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]	; scan for changes to number of layers control
</CsScore>

</CsoundSynthesizer>