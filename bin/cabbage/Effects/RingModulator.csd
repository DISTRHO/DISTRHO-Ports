<Cabbage>
form caption("Ring Modulator") size(435, 90), pluginID("rmod")
;label   bounds(7, 3, 200, 11), text("Iain McCurdy [2013]"), FontColour("grey")
image pos(0, 0), size(435, 90), colour("black"), shape("rounded"), outline("lime"), line(5) 
rslider bounds(  5, 12, 70, 70), text("Freq."), channel("freq"), range(0, 15000, 800, 0.25), colour("yellow")
rslider bounds( 75, 12, 70, 70), text("Sine/Sq."), channel("wave"), range(0, 1.00, 0),       colour("yellow")
line    bounds(150,  2, 3, 86), colour("lime")
rslider bounds(160, 12, 70, 70), text("Env."), channel("env"), range(0, 1.00, 0.5, 0.5),     colour("orange")
rslider bounds(230,  6, 40, 40), text("Att."), channel("att"), range(0.001, 0.5, 0.01, 0.5), colour("orange")
rslider bounds(230, 46, 40, 40), text("Dec."), channel("dec"), range(0.001, 0.5, 0.1,  0.5), colour("orange")
line    bounds(275,  2, 3, 86), colour("lime")
rslider bounds(290, 12, 70, 70), text("Mix"),   channel("mix"),   range(0, 1.00, 1), colour("chocolate")
rslider bounds(360, 12, 70, 70), text("Level"), channel("level"), range(0, 1.00, 1), colour("chocolate")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Author: Iain McCurdy (2012)
;http://iainmccurdy.org/csound.html

opcode	RingModulator,a,akkkkkk
	ain,kmix,kfreq,kwave,kenv,katt,kdec	xin					;READ IN INPUT ARGUMENTS
	iWet	ftgentmp	0,0,1024,-7,0,512,1,512,1		;RESCALING FUNCTION FOR WET LEVEL CONTROL
	iDry	ftgentmp	0,0,1024,-7,1,512,1,512,0		;RESCALING FUNCTION FOR DRY LEVEL CONTROL
	isine	ftgentmp	0,0,4096,10,1				;SINE WAVE
	kWet	table	kmix, iWet, 1					;RESCALE WET LEVEL CONTROL ACCORDING TO FUNCTION TABLE iWet
	kDry	table	kmix, iDry, 1					;RESCALE DRY LEVEL CONTROL ACCORDING TO FUNCTION TABLE iDry
	kporttime	linseg	0,0.001,0.02				;PORTAMENTO VARIABLE
	kModFrq	portk	kfreq, kporttime				;SMOOTH VARIABLE CHANGES
	aFollow	follow2		ain, katt, kdec			;AMPLITUDE FOLLOWING kModFrq + (cpsoct(kFollow*kenv*30))     	;CREATE A LEFT CHANNEL MODULATING FREQUENCY BASE ON THE STATIC VALUE CREATED BY kfreq AND THE AMOUNT OF DYNAMIC ENVELOPE FOLLOWING GOVERNED BY kenv
	kFollow	downsamp	aFollow
	kFollow	logcurve	kFollow/0dbfs,2
	kModFrq	=	kModFrq + (kFollow*kenv*8000)     	;CREATE A LEFT CHANNEL MODULATING FREQUENCY BASE ON THE STATIC VALUE CREATED BY kfreq AND THE AMOUNT OF DYNAMIC ENVELOPE FOLLOWING GOVERNED BY kenv
	aModSi	poscil	1, kModFrq, isine  				;CREATE RING MODULATING SIGNAL
	aModSq	vco2	1, kModFrq, 10   				;CREATE RING MODULATING SIGNAL
	aMod	ntrpol	aModSi,aModSq,kwave
	aout	sum	ain*kDry, ain*aMod*kWet				;MIX DRY AND WET SIGNALS
		xout	aout						;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr	1
kmix 	chnget	"mix"
kfreq 	chnget	"freq"
kwave 	chnget	"wave"
kenv 	chnget	"env"
katt 	chnget	"att"
kdec 	chnget	"dec"
klevel 	chnget	"level"
a1,a2	ins
;;a1,a2	diskin2	"808loop.wav",1,0,1
a1	RingModulator	a1,kmix,kfreq,kwave,kenv,katt,kdec
a2	RingModulator	a2,kmix,kfreq,kwave,kenv,katt,kdec
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>