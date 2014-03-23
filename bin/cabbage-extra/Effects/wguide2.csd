<Cabbage>
form caption("wguide2") size(545, 90)
image pos(0, 0), size(545, 90), colour(25,0,25), shape("rounded"), outline("white"), line(4) 
rslider bounds(10, 11, 70, 70), text("Freq. 1"),   channel("freq1"),     range(20, 8000, 160, 0.25), colour(150,110,110), trackercolour(white)
rslider bounds(75, 11, 70, 70), text("Freq. 2"),   channel("freq2"),     range(20, 8000, 160, 0.25), colour(150,110,110), trackercolour(white)
rslider bounds(140, 11, 70, 70), text("Cutoff 1"), channel("cutoff1"),   range(20,20000,8000,0.25),  colour(150,110,110), trackercolour(white)
rslider bounds(205, 11, 70, 70), text("Cutoff 1"), channel("cutoff2"),   range(20,20000,8000,0.25),  colour(150,110,110), trackercolour(white)
rslider bounds(270, 11, 70, 70), text("F.back 1"), channel("feedback1"), range(-0.999, 0.999, 0.2),  colour(150,110,110), trackercolour(white)
rslider bounds(335, 11, 70, 70), text("F.back 2"), channel("feedback2"), range(-0.999, 0.999, 0.2),  colour(150,110,110), trackercolour(white)
rslider bounds(400, 11, 70, 70), text("Mix"),      channel("mix"),       range(0, 1.00, 0.7),        colour(150,110,110), trackercolour(white)
rslider bounds(465, 11, 70, 70), text("Level"),    channel("level"),     range(0, 1.00, 0.7),        colour(150,110,110), trackercolour(white)
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
	kporttime	linseg	0,0.001,0.05
	gkfreq1		chnget	"freq1"					;READ WIDGETS...
	gkfreq1		portk	gkfreq1,kporttime
	afreq1		interp	gkfreq1
	gkfreq2		chnget	"freq2"					;READ WIDGETS...
	gkfreq2		portk	gkfreq2,kporttime
	afreq2		interp	gkfreq2
	gkcutoff1	chnget	"cutoff1"				;
	gkcutoff2	chnget	"cutoff2"				;
	gkfeedback1	chnget	"feedback1"				;
	gkfeedback2	chnget	"feedback2"				;
	gkmix		chnget	"mix"					;
	gklevel		chnget	"level"					;
	;asigL, asigR	diskin2	"Seashore.wav",1,0,1			;USE SOUND FILE FOR TESTING
	asigL, asigR	ins
	kporttime	linseg	0,0.01,0.03				;CREATE A VARIABLE THAT WILL BE USED FOR PORTAMENTO TIME
	aresL	wguide2 asigL, afreq1, afreq2, gkcutoff1, gkcutoff2, gkfeedback1, gkfeedback2
	aresR	wguide2 asigR, afreq1, afreq2, gkcutoff1, gkcutoff2, gkfeedback1, gkfeedback2
	aresL	dcblock2	aresL	;BLOCK DC OFFSET
	aresR	dcblock2	aresR	;BLOCK DC OFFSET           
	amixL		ntrpol	asigL,aresL,gkmix
	amixR		ntrpol	asigR,aresR,gkmix
			outs	amixL*gklevel, amixR*gklevel		;WGUIDE1 OUTPUTS ARE SENT OUT
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>


</CsoundSynthesizer>



























