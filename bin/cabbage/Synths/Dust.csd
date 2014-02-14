<Cabbage>
form caption("Dust"), size(300, 80), pluginID("dust")
image bounds(  0,  0,300, 80), colour("brown"), shape("rounded"), outline("white"), line(4) 
checkbox bounds( 10, 10, 80, 15), text("On/Off"), channel("onoff"), value(1), fontcolour("white")
rslider  bounds( 70, 10, 60, 60), text("Amplitude"), channel("amp"), range(0, 1.00, 0.5, 0.5, 0.001), fontcolour("white")
rslider  bounds(125, 10, 60, 60), text("Freq."), channel("freq"), range(0.1, 10000, 500, 0.5, 0.01), fontcolour("white")
rslider  bounds(180, 10, 60, 60), text("Spread"), channel("spread"), range(0, 1.00, 1), fontcolour("white")
rslider  bounds(235, 10, 60, 60), text("Tone Var."), channel("ToneVar"), range(0, 1.00, 0), fontcolour("white")
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100
ksmps 		= 	64
nchnls 		= 	2
0dbfs		=	1	;MAXIMUM AMPLITUDE
massign	0,0

instr	1
	konoff	chnget	"onoff"		;read in on/off switch widget value
	if konoff==0 goto SKIP		;if on/off switch is off jump to skip label
	kamp	chnget	"amp"
	kfreq	chnget	"freq"
	kspread	chnget	"spread"
	asig	dust2	kamp, kfreq	;GENERATE 'dust2' IMPULSES

	; tone variation
	kToneVar	chnget	"ToneVar"
	if(kToneVar>0) then
 	 kcfoct	random		14-(kToneVar*10),14
	 asig	tonex		asig,cpsoct(kcfoct),1
	endif


	kpan	random	0.5-(kspread*0.5), 0.5+(kspread*0.5)
	asigL,asigR	pan2	asig,kpan
		outs	asigL,asigR	;SEND AUDIO SIGNAL TO OUTPUT
	SKIP:				;A label. Skip to here is on/off switch is off 
endin


</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]	;instrument that reads in widget data
</CsScore>

</CsoundSynthesizer>