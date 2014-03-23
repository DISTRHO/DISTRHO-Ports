<Cabbage>
form caption("Ping Pong Delay") size(350, 90), pluginID("ppdl")
image                    bounds(0, 0,350, 90), colour("Maroon"), shape("rounded"), outline("white"), line(4) 
rslider bounds(10, 11, 70, 70),  text("Time"),     channel("time"),     range(0.001, 10, 0.4, 0.5), colour(135, 30, 30) trackercolour(255,255,150) fontcolour(silver)
rslider bounds(75, 11, 70, 70),  text("Cutoff"),   channel("cutoff"),   range(20,20000,20000,0.5),  colour(135, 30, 30) trackercolour(255,255,150) fontcolour(silver)
rslider bounds(140, 11, 70, 70), text("Feedback"), channel("feedback"), range(0, 1.00, 0.5),        colour(135, 30, 30) trackercolour(255,255,150) fontcolour(silver)
rslider bounds(205, 11, 70, 70), text("Mix"),      channel("mix"),      range(0, 1.00, 0.5),        colour(135, 30, 30) trackercolour(255,255,150) fontcolour(silver)
rslider bounds(270, 11, 70, 70), text("Level"),    channel("level"),    range(0, 1.00, 0.7),        colour(135, 30, 30) trackercolour(255,255,150) fontcolour(silver)

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
	ktime		chnget	"time"					;READ WIDGETS...
	kcutoff		chnget	"cutoff"				;
	kfeedback	chnget	"feedback"				;
	kmix		chnget	"mix"					;
	klevel		chnget	"level"					;
	asigL, asigR	ins
	kporttime	linseg	0,0.01,0.03				;CREATE A VARIABLE THAT WILL BE USED FOR PORTAMENTO TIME
	ktime		portk	ktime,kporttime				;PORTAMENTO SMOOTHING OF DELAT TIME
	atime		interp	ktime					;INTERPOLATED A-RATE VERSION OF DELAY TIME
	aL_OS		vdelay	asigL,(atime*1000)/2,(10*1000)/2	;DELAYED OFFSET OF LEFT CHANNEL (FIRST 'PING')
	;LEFT CHANNEL
	abuf		delayr	10					;ESTABLISH DELAY BUFFER
	aDelL		deltapi	atime					;TAP BUFFER
	aDelL		tone	aDelL,kcutoff				;LOWPASS FILTER DELAY TAP 
			delayw	aL_OS+(aDelL*kfeedback)			;WRITE INPUT AUDIO INTO BUFFER
	;RIGHT CHANNEL
	abuf		delayr	10					;ESTABLISH DELAY BUFFER
	aDelR		deltapi	atime					;TAP BUFFER
	aDelR		tone	aDelR,kcutoff				;LOWPASS FILTER DELAY TAP
			delayw	asigR+(aDelR*kfeedback)			;WRITE INPUT AUDIO INTO BUFFER
	amixL		ntrpol	asigL,aDelL+aL_OS,kmix			;MIX DRY AND WET SIGNALS (LEFT CHANNEL)
	amixR		ntrpol	asigR,aDelR,kmix			;MIX DRY AND WET SIGNALS (RIGHT CHANNEL)
			outs	amixL*klevel, amixR*klevel		;PING PONG DELAY OUTPUTS ARE SENT OUT
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>