GaussianDust.csd
Iain McCurdy [2013]

<Cabbage>
form caption("Gaussian Dust"), size(325, 350), pluginID("gaus")
image                  bounds(0, 0, 325, 350), colour("DarkSlateGrey"), shape("rounded"), outline("white"), line(4) 
checkbox bounds( 15, 10, 80, 15), text("On/Off"), channel("onoff"), value(1), fontcolour("white")
combobox bounds( 10, 40, 70, 20), channel("mode"), value(2), text("Held", "Reinit")
rslider  bounds(90, 10, 60, 60), text("Amplitude"), channel("amp"), range(0, 1, 0.5, 0.5, 0.001), fontcolour("white")
rslider  bounds(150, 10, 60, 60), text("Freq."), channel("freq"), range(0.1, 10000, 10, 0.25, 0.01), fontcolour("white")
rslider  bounds(205, 10, 60, 60), text("Deviation"), channel("dev"), range(0, 10, 1), fontcolour("white")
rslider  bounds(260, 10, 60, 60), text("Spread"), channel("spread"), range(0, 1, 1), fontcolour("white")
xypad bounds( 40, 78, 245, 260), text("Freq./Deviation"), channel("freq", "dev"), rangex(0.1, 10000, 10), rangey(0, 10.00, 0)
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100
ksmps 		= 	64
nchnls 		= 	2
0dbfs		=	1

instr	1
	konoff	chnget	"onoff"		;read in on/off switch widget value
	if konoff==0 goto SKIP		;if on/off switch is off jump to skip label
	kamp	chnget	"amp"
	kfreq	chnget	"freq"
	kdev	chnget	"dev"
	kporttime	linseg	0,0.01,0.1
	kdev	portk	kdev,kporttime
	kmode	chnget	"mode"
	kmode	=	kmode - 1
	kspread	chnget	"spread"
	ktrig	changed		kmode		;IF gkmode COUNTER IS CHANGED GENERATE A MOMENTARY '1' IMPULSE
	if ktrig==1 then			;THEREFORE IF gkmode HAS BEEN CHANGED...
	 reinit	UPDATE				;BEGIN A REINITIALISATION PASS AT LABEL 'UPDATE'
	endif					;END OF CONDITIONAL BRANCH
	UPDATE:					;LABEL 'UPDATE'. REINITIALISATION BEGINS FROM HERE.
	asig	gausstrig	kamp, kfreq, kdev, i(kmode)	;GENERATE GAUSSIAM TRIGGERS
	kpan	random	0.5-(kspread*0.5), 0.5+(kspread*0.5)
	asigL,asigR	pan2	asig,kpan
	rireturn				;RETURN FROM REINITIALISATION PASS
		outs	asigL,asigR		;SEND AUDIO SIGNAL TO OUTPUT
	SKIP:					;A label. Skip to here is on/off switch is off 
endin


</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]	;instrument plays for a week
</CsScore>

</CsoundSynthesizer>