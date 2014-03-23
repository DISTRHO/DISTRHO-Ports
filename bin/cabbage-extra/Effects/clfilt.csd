<Cabbage>
form caption("clfilt - Multi-Mode Filter") size(600, 90), pluginID("clfl")
image                              bounds(0, 0, 600, 90), colour( 40,40,100), shape("rounded"), outline("white"), line(4) 
rslider bounds( 10, 11, 70, 70), text("Freq."),       colour( 40,40,100), 	fontcolour("white"), 	channel("cf"), 		range(20, 20000, 2000, 0.333), trackercolour("white")
rslider bounds( 75, 11, 70, 70), text("N.Poles"),     colour( 40,40,100), 	fontcolour("white"), 	channel("npol"),	range(2,80,24,1,2),            trackercolour("white")
rslider bounds(140, 11, 70, 70), text("Ripple"),      colour( 40,40,100), 	fontcolour("white"), 	channel("pbr"),		range(0.1,50,14),              trackercolour("white")
rslider bounds(205, 11, 70, 70), text("Attenuation"), colour( 40,40,100), 	fontcolour("white"), 	channel("sba"),		range(-120,-1,-60),            trackercolour("white")
combobox channel("type"),  bounds(275, 12, 120, 20), value(1), text("Lowpass","Highpass")
label bounds(320, 32,110, 12), text("Type"), fontcolour("white")
combobox channel("kind"),  bounds(275, 46, 120, 20), value(2), text("Butterworth","Chebychev type I","Chebychev type II")
label bounds(313, 66,120, 12), text("Method"), fontcolour("white")
rslider bounds(400, 11, 70, 70), text("Mix"),	      colour( 40,40,100),	fontcolour("white"), 	channel("mix"), 	range(0,1.00,1),   trackercolour("white")
rslider bounds(465, 11, 70, 70), text("Level"),	      colour( 40,40,100),	fontcolour("white"), 	channel("level"), 	range(0,1.00,0.3), trackercolour("white")
checkbox channel("test"), pos(540, 35), text("TEST"), colour("yellow"), fontcolour("white"), size(100, 15), value(0)
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

;Author: Iain McCurdy (2013)

instr	1	; widgets input
	gkcf		chnget	"cf"				;
	gknpol		chnget	"npol"				;
	gknpol		init	2
	gkpbr		chnget	"pbr"
	gksba		chnget	"sba"
	gktype		chnget	"type"
	gktype		=	gktype - 1
	gktype		init	0
	gkkind		chnget	"kind"
	gkkind		=	gkkind - 1
	gkkind		init	0
	gkmix		chnget	"mix"				;
	gklevel		chnget	"level"				;
	gktest		chnget	"test"
endin

instr	2	; clfilt - multimode filter
	kporttime	linseg	0,0.001,0.02
	kcf	portk	gkcf,kporttime
	kmix	portk	gkmix,kporttime
	klevel	portk	gklevel,kporttime
	if gktest=0 then
	 aL,aR	ins
	else
	 aL	pinkish	0.5
	 aR	=	aL
	endif
	
	ktrig	changed	gktype, gknpol, gkkind, gkpbr, gksba
	if ktrig==1 then
	 reinit	UPDATE
	endif
	UPDATE:
	aFiltL	clfilt	aL, kcf, i(gktype), i(gknpol), i(gkkind), i(gkpbr), i(gksba)
	aFiltR	clfilt	aR, kcf, i(gktype), i(gknpol), i(gkkind), i(gkpbr), i(gksba)
	rireturn

	aL	ntrpol	aL,aFiltL,kmix
	aR	ntrpol	aR,aFiltR,kmix
		outs	aL*klevel,aR*klevel
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
i 2 0.01 [3600*24*7]
</CsScore>

</CsoundSynthesizer>
