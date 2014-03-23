; pvsBandr
; seems to crash a bit on Windows...

<Cabbage>
form caption("pvs Bandreject Filter"), size(390, 310) colour( 55,165,79,150), pluginID("bndp")
vslider bounds( 10,  0,  60,200), text("Low Cut"),   channel("lowcut"),   range(20, 20000, 100,0.5),  colour("black"), FontColour("black"), trackercolour("yellow")
vslider bounds( 70,  0,  60,200), text("Low Full"),  channel("lowfull"),  range(20, 20000, 150,0.5),  colour("black"), trackercolour("yellow")
vslider bounds(130,  0,  60,200), text("High Full"), channel("highfull"), range(20, 20000,2800,0.5),  colour("black"), trackercolour("yellow")
vslider bounds(190,  0,  60,200), text("High Cut"),  channel("highcut"),  range(20, 20000, 2900,0.5), colour("black"), trackercolour("yellow")
vslider bounds(250,  0,  60,200), text("Curve"),     channel("type"),     range(-5.00, 5.00, 0),      colour("black"), trackercolour("yellow")
rslider bounds(310, 30,  70, 70), text("FFT Size"),  channel("att_table"),range(1,10, 5, 1,1),        colour("black"), trackercolour("yellow")
rslider bounds(310,110,  70, 70), text("Feedback"),  channel("feedback"), range(0,0.99, 0),           colour("black"), trackercolour("yellow")
line bounds(  0, 210, 390,100), colour("Grey")
hslider bounds(  5,205, 380, 60), text("Mix"), channel("mix"), fontcolour("black"), range(0, 1.00, 1), colour("black"), trackercolour("yellow")
hslider bounds(  5,250, 380, 60), text("Level"), channel("lev"), fontcolour("black"), range(0, 10.00, 0.5, 0.5), colour("black"), trackercolour("yellow")
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

;Author: Iain McCurdy (2013)
;http://iainmccurdy.org/csound.html

/* FFT attribute tables */
giFFTattributes1	ftgen	0, 0, 4, -2,   64,  32,   64, 1
giFFTattributes2	ftgen	0, 0, 4, -2,  128,  64,  128, 1
giFFTattributes3	ftgen	0, 0, 4, -2,  256, 128,  256, 1
giFFTattributes4	ftgen	0, 0, 4, -2,  512, 128,  512, 1
giFFTattributes5	ftgen	0, 0, 4, -2, 1024, 128, 1024, 1
giFFTattributes6	ftgen	0, 0, 4, -2, 2048, 256, 2048, 1
giFFTattributes7	ftgen	0, 0, 4, -2, 2048,1024, 2048, 1
giFFTattributes8	ftgen	0, 0, 4, -2, 4096,1024, 4096, 1
giFFTattributes9	ftgen	0, 0, 4, -2, 8192,2048, 8192, 1
giFFTattributes10	ftgen	0, 0, 4, -2,16384,4096,16384, 1

opcode	pvsbandr_module,a,akkkkkkkkiiii
	ain,klowcut,klowfull,khighfull,khighcut,ktype,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin

	f_FB		pvsinit iFFTsize,ioverlap,iwinsize,iwintype, 0			;INITIALISE FEEDBACK FSIG
	f_anal  	pvsanal	ain, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG	
	f_mix		pvsmix	f_anal, f_FB						;MIX AUDIO INPUT WITH FEEDBACK SIGNAL
	f_bandr		pvsbandr	f_mix, klowcut, klowfull, khighfull, khighcut, ktype
	f_FB		pvsgain f_bandr, kfeedback 					;CREATE FEEDBACK F-SIGNAL FOR NEXT PASS
	aout		pvsynth f_bandr                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
	ain		delay		ain,iwinsize/sr
	amix		ntrpol		ain, aout, kmix					;CREATE DRY/WET MIX
			xout		amix*klev	
endop

instr	1
	kmix		chnget	"mix"
	klev		chnget	"lev"
	klowcut		chnget	"lowcut"
	klowfull	chnget	"lowfull"
	khighfull	chnget	"highfull"
	khighcut	chnget	"highcut"
	ktype		chnget	"type"	
	kfeedback	chnget	"feedback"	

	ainL,ainR	ins
	;ainL,ainR	diskin	"808loop.wav",1,0,1	;USE FOR TESTING

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

	aoutL		pvsbandr_module	ainL,klowcut,klowfull,khighfull,khighcut,ktype,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvsbandr_module	ainR,klowcut,klowfull,khighfull,khighcut,ktype,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype

			outs	aoutL, aoutR
	;klev		chnget	"lev"
	;	outs	ainL*klev,ainR*klev
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>