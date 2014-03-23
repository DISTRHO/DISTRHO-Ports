<Cabbage>
form caption("phaser2") size(565, 90), pluginID("phs2")
image pos(0, 0), size(565, 90), colour(0,0,25), shape("rounded"), outline("white"), line(4) 
rslider bounds(10, 11, 70, 70),  text("Frequency"),  channel("freq"),     range(20.0, 5000, 160, 0.25), colour(100,100,200), trackercolour(silver)
rslider bounds(75, 11, 70, 70),  text("Q"),          channel("q"),        range(0.0001,4,3),            colour(100,100,200), trackercolour(silver)
rslider bounds(140, 11, 70, 70), text("N.Ords."),    channel("ord"),      range(1, 256, 8, 0.5,1),      colour(100,100,200), trackercolour(silver)
label   bounds(220, 20, 60,12),  text("Sep. Mode")
rslider bounds(290, 11, 70, 70), text("Separation"), channel("sep"),      range(-3, 3.00, 0.9),         colour(100,100,200), trackercolour(silver)
rslider bounds(355, 11, 70, 70), text("Feedback"),   channel("feedback"), range(-0.99, 0.99, 0.9),      colour(100,100,200), trackercolour(silver)
rslider bounds(420, 11, 70, 70), text("Mix"),        channel("mix"),      range(0, 1.00, 1),            colour(100,100,200), trackercolour(silver)
rslider bounds(485, 11, 70, 70), text("Level"),      channel("level"),    range(0, 1.00, 0.7),          colour(100,100,200), trackercolour(silver)
combobox bounds(210, 34, 80,25), channel("mode"), size(100,50), value(1), text("Equal", "Power")
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

instr	1
	gkfreq		chnget	"freq"					;READ WIDGETS...
	gkq		chnget	"q"					;
	gkmode		chnget	"mode"					;
	gkmode		init	1
	gkmode		init	i(gkmode)-1
	gksep		chnget	"sep"					;
	gkfeedback	chnget	"feedback"				;
	gkord		chnget	"ord"					;
	gkmix		chnget	"mix"					;
	gklevel		chnget	"level"					;
	;asigL, asigR	diskin2	"Seashore.wav",1,0,1			;USE SOUND FILE FOR TESTING
	asigL, asigR	ins
	kporttime	linseg	0,0.01,0.03				;CREATE A VARIABLE THAT WILL BE USED FOR PORTAMENTO TIME
	kfreq		portk	gkfreq, kporttime			;PORTAMENTO IS APPLIED TO 'SMOOTH' SLIDER MOVEMENT	
	kq		portk	gkq, kporttime				;PORTAMENTO IS APPLIED TO 'SMOOTH' SLIDER MOVEMENT
	ksep		portk	gksep, kporttime				;PORTAMENTO IS APPLIED TO 'SMOOTH' SLIDER MOVEMENT
	kSwitch		changed	gkord,gkmode				;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
	if	kSwitch=1	then					;IF I-RATE VARIABLE CHANGE TRIGGER IS '1'...
		reinit	UPDATE						;BEGIN A REINITIALISATION PASS FROM LABEL 'UPDATE'
	endif								;END OF CONDITIONAL BRANCH
	UPDATE:								;BEGIN A REINITIALISATION PASS FROM HERE
	aphaserl	phaser2		asigL, kfreq, kq, gkord, gkmode, ksep, gkfeedback	; PHASER2 IS APPLIED TO THE LEFT CHANNEL
	aphaserr	phaser2		asigR, kfreq, kq, gkord, gkmode, ksep, gkfeedback	; PHASER1 IS APPLIED TO THE RIGHT CHANNEL
	rireturn							;RETURN FROM REINITIALISATION PASS TO PERFORMANCE TIME PASSES
	amixL		ntrpol	asigL,aphaserl,gkmix
	amixR		ntrpol	asigR,aphaserr,gkmix
			outs	amixL*gklevel, amixR*gklevel		;PHASER OUTPUTS ARE SENT OUT
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>


</CsoundSynthesizer>



























