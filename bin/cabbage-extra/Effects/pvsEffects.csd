<Cabbage>
form caption("FFT Effects") size(450, 640), pluginID("pvfx")

; PVSFREEZE
groupbox bounds(0,  0, 450, 100), colour(  0,200,100,100) text("Freeze"), fontcolour("white"), plant("pvsfreeze"){
checkbox bounds(35, 30, 30, 30), channel("freeze_onoff"), fontcolour("black"), colour("red"), shape("rounded")
label   bounds( 30, 68, 60, 12), text("On/Off"), FontColour("white")
checkbox bounds(100, 30, 180, 25), channel("freezb"), text("Freeze Both"), fontcolour("white"), shape("rounded"), colour("red")
checkbox bounds(215, 30, 180, 25), channel("freeza"), text("Freeze Amplitudes"), fontcolour("white"), shape("rounded"), colour("yellow")
checkbox bounds(215, 65, 180, 25), channel("freezf"), text("Freeze Frequencies"), fontcolour("white"), shape("rounded"), colour("orange")
rslider bounds(370, 25, 70, 70), text("Mix"), channel("freeze_mix"), fontcolour("white"), range(0, 1.00, 1), colour(  0,200,100,150)
}

; PVSCALE
groupbox bounds(0,100, 225, 100), colour(250,250,  0,200) text("Freq. Scale"), fontcolour("white"), plant("pvscale"){
checkbox bounds(25, 30, 30, 30), channel("pvscale_onoff"), fontcolour("black"), colour("red"), shape("rounded")
label   bounds( 20, 68, 60, 12), text("On/Off"), FontColour("white")
rslider bounds( 70, 25, 70, 70), text("Ratio"), channel("pvscale_scal"), fontcolour("white"), range(0.25, 4.00, 1, 0.5, 0.01), colour(250,250,  0,150)
rslider bounds(140, 25, 70, 70), text("Mix"), channel("pvscale_mix"), fontcolour("white"), range(0, 1.00, 1), colour(250,250,  0,150)
}

; PVSHIFT
groupbox bounds(225,100, 225, 100), colour(  0,250,250,200) text("Freq. Shift"), fontcolour("white"), plant("pvshift"){
checkbox bounds(25, 30, 30, 30), channel("pvshift_onoff"), fontcolour("black"), colour("red"), shape("rounded")
label   bounds( 20, 68, 60, 12), text("On/Off"), FontColour("white")
rslider bounds( 70, 25, 70, 70), text("Freq."), channel("pvshift_freq"), fontcolour("white"), range(-4000, 4000, 0), colour(  0,250,250,150)
rslider bounds(140, 25, 70, 70), text("Mix"), channel("pvshift_mix"), fontcolour("white"), range(0, 1.00, 1), colour(  0,250,250,150)
}

; PVSBLUR
groupbox bounds(0,200, 225, 100), colour( 50,250,  0,200) text("Blur"), fontcolour("white"), plant("pvsblur"){
checkbox bounds(25, 30, 30, 30), channel("pvsblur_onoff"), fontcolour("black"), colour("red"), shape("rounded")
label   bounds( 20, 68, 60, 12), text("On/Off"), FontColour("white")
rslider bounds( 70, 25, 70, 70), text("Ratio"), channel("pvsblur_scal"), fontcolour("white"), range(0.25, 4.00, 1, 0.5, 0.01), colour( 50,250,  0,150)
rslider bounds(140, 25, 70, 70), text("Mix"), channel("pvsblur_mix"), fontcolour("white"), range(0, 1.00, 1), colour( 50,250,  0,150)
}

; PVSMOOTH
groupbox bounds(225,200, 225, 100), colour(100,250,100,200) text("Smooth"), fontcolour("white"), plant("pvsmooth"){
checkbox bounds(25, 30, 30, 30), channel("pvsmooth_onoff"), fontcolour("black"), colour("red"), shape("rounded")
label   bounds( 20, 68, 60, 12), text("On/Off"), FontColour("white")
rslider bounds( 70, 25, 70, 70), text("Freq."), channel("pvsmooth_freq"), fontcolour("white"), range(-4000, 4000, 0), colour(100,250,100,160)
rslider bounds(140, 25, 70, 70), text("Mix"), channel("pvsmooth_mix"), fontcolour("white"), range(0, 1.00, 1), colour(100,250,100,150)
}

; PVSWARP

; PVSBANDP / PVSBANDR

; PVSADSYN
groupbox bounds(0, 300, 450, 100), colour(150, 40,  40,250) text("Additive Resynthesizer"), fontcolour("white"), plant("pvsadsyn"){
checkbox bounds(35, 30, 30, 30), channel("pvsadsyn_onoff"), fontcolour("black"), colour("red"), shape("rounded")
label   bounds( 30, 68, 60, 12), text("On/Off"), FontColour("white")
rslider bounds( 90, 25, 70, 70), text("Freq.Mod."), channel("pvsadsyn_fmod"), range(0.25, 4, 1), fontcolour("white"), colour(110,  0,   0,200)
rslider bounds(160, 25, 70, 70), text("Num.Osc.s"), channel("pvsadsyn_noscs"), range(1, 1024, 256,1,1), fontcolour("white"), colour(110,  0,   0,200)
rslider bounds(230, 25, 70, 70), text("Bin Offset"), channel("pvsadsyn_binoffset"), range(0, 256, 1,1,1), fontcolour("white"), colour(110,  0,   0,200)
rslider bounds(300, 25, 70, 70), text("Bin Incr."), channel("pvsadsyn_binincr"), range(1, 32, 1,1,1), fontcolour("white"), colour(110,  0,   0,200)
rslider bounds(370, 25, 70, 70), text("Mix"), channel("pvsadsyn_mix"), range(0, 1, 1), fontcolour("white"), colour(110,  0,   0,200)
}

; OUTPUT
groupbox bounds(0, 400, 450, 100), colour(  0,250,250,100) text("Output"), fontcolour("white"), plant("output"){
rslider bounds( 10, 25, 70, 70), text("Level"), channel("level"), range(0, 1.00, 0), fontcolour("white"), colour(  0,250,250,150)
}

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

instr	1
	/* AUDIO INPUT */
	aL,aR	ins
	;aL,aR	diskin2	"SynthPad.wav",1,0,1
	
	/* FFT PARAMETERS */
	iFFTsize	=	1024
	ioverlap	=	256
	iwinsize	=	1024
	iwintype	=	1

	/* ANALYSE AUDIO SIGNAL */
	f_sigL  	pvsanal	aL, iFFTsize, ioverlap, iwinsize, iwintype	;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	f_sigR  	pvsanal	aR, iFFTsize, ioverlap, iwinsize, iwintype
	
	/* PVSFREEZE */
	kfreeze_onoff	chnget	"freeze_onoff"
	kfreeze_mix		chnget	"freeze_mix"
	kfreeza		chnget	"freeza"
	kfreezf		chnget	"freezf"
	kfreezb		chnget	"freezb"
	if(kfreeze_onoff==1) then
	 ; triggering of 'Freeze All' mode
	 kon		=	1
	 koff		=	0
	 ktrigon		trigger	kfreezb,0.5,0
	 ktrigoff	trigger	kfreezb,0.5,1
	 if(ktrigon==1) then
	  chnset		kon,"freeza"
	  chnset		kon,"freezf"
	 elseif(ktrigoff==1) then
	  chnset		koff,"freeza"
	  chnset		koff,"freezf"
	 endif
	 f_freezeL	pvsfreeze f_sigL, kfreeza, kfreezf	
	 f_freezeR	pvsfreeze f_sigR, kfreeza, kfreezf
	 f_drymixL	pvsgain	f_sigL, 1-kfreeze_mix           ;WET/DRY MIX
	 f_drymixR	pvsgain	f_sigR, 1-kfreeze_mix           ;
	 f_wetmixL	pvsgain	f_freezeL, kfreeze_mix          ;
	 f_wetmixR	pvsgain	f_freezeR, kfreeze_mix          ;
	 f_sigL		pvsmix	f_drymixL, f_wetmixL            ;
	 f_sigR		pvsmix	f_drymixR, f_wetmixR            ;
	endif



	/* PVSADSYN */
	kpvsadsyn_onoff		chnget	"pvsadsyn_onoff"						;READ IN WIDGETS
	if(kpvsadsyn_onoff==1) then
	 kpvsadsyn_mix		chnget	"pvsadsyn_mix"							;READ IN WIDGETS
 	 kpvsadsyn_fmod		chnget	"pvsadsyn_fmod"
	 kpvsadsyn_noscs	chnget	"pvsadsyn_noscs"
	 kpvsadsyn_binoffset	chnget	"pvsadsyn_binoffset"
	 kpvsadsyn_binincr	chnget	"pvsadsyn_binincr"
	 kpvsadsyn_lev		chnget	"pvsadsyn_lev"
	 knossc	init	256
	 ktrig		changed	kpvsadsyn_noscs,kpvsadsyn_binoffset,kpvsadsyn_binincr			;GENERATE TRIGGER FOR I-RATE PARAMETERS
	 if ktrig==1 then
	  reinit	UPDATE
	 endif
	 UPDATE:
	 inoscs	init	i(kpvsadsyn_noscs)	
 	 ibinoffset	init	i(kpvsadsyn_binoffset)
	 ibinincr	init	i(kpvsadsyn_binincr)
	 inoscs	limit	inoscs, 1, (((iFFTsize*0.5)+1)-ibinoffset)/ibinincr				;LIMIT NUMBER OF OSCILLATORS USED TO PREVENT CRASHES
	 aresynL		pvsadsyn f_sigL, inoscs, kpvsadsyn_fmod , ibinoffset, ibinincr		;RESYNTHESIZE FROM THE fsig USING pvsadsyn
	 aresynR		pvsadsyn f_sigR, inoscs, kpvsadsyn_fmod , ibinoffset, ibinincr
	 rireturn
	 aresynL	ntrpol		aL, aresynL, kpvsadsyn_mix					;CREATE DRY/WET MIX
	 aresynR	ntrpol		aR, aresynR, kpvsadsyn_mix
	else
	
	/* PVSYNTH */
  	 aresynL	pvsynth	f_sigL		;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
  	 aresynR	pvsynth	f_sigR		;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
	endif
	
	/* AUDIO OUTPUT */
	klevel		chnget	"level"							;READ IN WIDGETS
			outs		aresynL*klevel, aresynR*klevel

endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>