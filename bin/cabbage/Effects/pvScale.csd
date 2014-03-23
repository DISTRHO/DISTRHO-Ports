<Cabbage>
form caption("pvscale Pitch Shifter") size(510,  90), pluginID("scal")
image bounds(0, 0, 510, 90), colour("SlateGrey"), outline("silver"), line(4)
image bounds(  6, 23,498, 30), colour("silver"), shape("ellipse"), line(3)
image bounds( 15,  9, 60, 60), colour(30,30,30), shape("ellipse"), line(0)	; circles around rsliders
image bounds( 85,  9, 60, 60), colour(30,30,30), shape("ellipse"), line(0)
image bounds(155,  9, 60, 60), colour(30,30,30), shape("ellipse"), line(0)
image bounds(295,  9, 60, 60), colour(30,30,30), shape("ellipse"), line(0)
image bounds(365,  9, 60, 60), colour(30,30,30), shape("ellipse"), line(0)
image bounds(435,  9, 60, 60), colour(30,30,30), shape("ellipse"), line(0)
rslider bounds(10, 10, 70, 70),  text("Semitones"), channel("semis"), range(-24, 24, 7, 1, 1),   fontcolour("black"),colour("DarkSlateGrey"), trackercolour("LightBlue")
rslider bounds(80, 10, 70, 70),  text("Cents"),     channel("cents"), range(-100, 100, 0, 1, 1), fontcolour("black"),colour("DarkSlateGrey"), trackercolour("LightBlue")
rslider bounds(150, 10, 70, 70), text("Feedback"),  channel("FB"), range(0.00, 0.99, 0),         fontcolour("black"),colour("DarkSlateGrey"), trackercolour("LightBlue")
combobox bounds(220, 28,  70,20), channel("FB_mode"), value(0), text("F Sig.", "Audio")
label   bounds(220, 54, 70, 12), text("F.back Mode"), FontColour("black")
rslider bounds(290, 10, 70, 70), text("FFT Size"),  channel("att_table"), range(1, 8, 5, 1,1),      fontcolour("black"),colour("DarkSlateGrey"), trackercolour("LightBlue")
rslider bounds(360, 10, 70, 70), text("Mix"),       channel("mix"),       range(0, 1.00, 0.5),      fontcolour("black"),colour("DarkSlateGrey"), trackercolour("LightBlue")
rslider bounds(430, 10, 70, 70), text("Level"),     channel("lev"),       range(0, 1.00, 0.5, 0.5), fontcolour("black"),colour("DarkSlateGrey"), trackercolour("LightBlue")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>

sr 		= 	44100
ksmps 		= 	32
nchnls 		= 	2
0dbfs		=	1	;MAXIMUM AMPLITUDE

;Iain McCurdy
;http://iainmccurdy.org/csound.html
;Pitch shifting effect using pvs scale opcode.

/* FFT attribute tables */
giFFTattributes1	ftgen	0, 0, 4, -2,   64,  32,   64, 1
giFFTattributes2	ftgen	0, 0, 4, -2,  128,  64,  128, 1
giFFTattributes3	ftgen	0, 0, 4, -2,  256, 128,  256, 1
giFFTattributes4	ftgen	0, 0, 4, -2,  512, 128,  512, 1
giFFTattributes5	ftgen	0, 0, 4, -2, 1024, 256, 1024, 1
giFFTattributes6	ftgen	0, 0, 4, -2, 2048, 512, 2048, 1
giFFTattributes7	ftgen	0, 0, 4, -2, 4096,1024, 4096, 1
giFFTattributes8	ftgen	0, 0, 4, -2, 8192,2048, 8192, 1

opcode	pvscale_module,a,akkkkkiiii
	ain,kscale,kfeedback,kFB_mode,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	if(kFB_mode==0) then
	 f_FB		pvsinit iFFTsize,ioverlap,iwinsize,iwintype, 0			;INITIALISE FEEDBACK FSIG
	 f_anal  	pvsanal	ain, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	 f_mix		pvsmix	f_anal, f_FB						;MIX AUDIO INPUT WITH FEEDBACK SIGNAL
	 f_scale		pvscale f_mix, kscale					;RESCALE FREQUENCIES
	 f_FB		pvsgain f_scale, kfeedback 					;CREATE FEEDBACK F-SIGNAL FOR NEXT PASS
	 aout		pvsynth f_scale                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
	else
	 aFB		init	0
	 f_anal  	pvsanal	ain+aFB, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	 f_scale		pvscale f_anal, kscale					;RESCALE FREQUENCIES
	 aout		pvsynth f_scale                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
	 aFB		=	aout*kfeedback
	endif	
	amix		ntrpol		ain, aout, kmix					;CREATE DRY/WET MIX
			xout		amix*klev	
endop

instr	1
	ainL,ainR	ins
	;ainL,ainR	diskin2	"SynthPad.wav",1,0,1
	kmix	chnget	"mix"
	kFB	chnget	"FB"
	kFB_mode	chnget	"FB_mode"

	/* SET FFT ATTRIBUTES */
	katt_table	chnget	"att_table"	; FFT atribute table
	katt_table	init	5
	ktrig		changed	katt_table
	if ktrig==1 then
	 reinit update
	endif
	update:
	iFFTsize	table	0, giFFTattributes1 + i(katt_table) - 1
	ioverlap	table	1, giFFTattributes1 + i(katt_table) - 1
	iwinsize	table	2, giFFTattributes1 + i(katt_table) - 1
	iwintype	table	3, giFFTattributes1 + i(katt_table) - 1
	/*-------------------*/
	
	kfeedback	chnget	"FB"
	ksemis		chnget	"semis"
	kcents		chnget	"cents"
	kmix		chnget	"mix"
	klev		chnget	"lev"
	kporttime	linseg	0,0.001,0.02
	kscale		=	semitone(ksemis)*cent(kcents)
	aoutL		pvscale_module	ainL,kscale,kfeedback,kFB_mode,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvscale_module	ainR,kscale,kfeedback,kFB_mode,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutR,aoutR
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>