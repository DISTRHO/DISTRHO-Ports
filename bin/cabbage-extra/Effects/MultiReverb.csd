Reverb effect that employs screverb and freeverb opcodes.
Pitch Mod. is only used by reverbsc.

<Cabbage>
form caption("Multi Reverb") size(415, 90), pluginID("Rvrb")
image  bounds(  7, 22, 73, 45), colour( 39, 31, 20), shape("rounded"), outline(white), line(1) 
image                bounds(0, 0, 415, 90), colour( 39, 31, 20), shape("rounded"), outline("white"), line(4) 
label  bounds( 29, 26, 60, 12), text("Type"), fontColour(white)
button bounds( 11, 40, 66, 20), text("screverb","freeverb"), channel("type"), value(0), fontcolour("lime")
rslider bounds( 80, 11, 70, 70), text("Size"), 		fontcolour(white), 		channel("fblvl"), 	range(0, 1.00, 0.8), colour( 79, 81, 70), trackercolour(240,210,170)
rslider bounds(145, 11, 70, 70), text("Damping"), 	fontcolour(white), 		channel("fco"), 	range(0, 1.00, 0.6), colour( 79, 81, 70), trackercolour(240,210,170)
rslider bounds(210, 11, 70, 70), text("Pitch Mod."), 	fontcolour(white), 		channel("pitchm"), 	range(0, 20.0, 1),   colour( 79, 81, 70), trackercolour(240,210,170)
rslider bounds(275, 11, 70, 70), text("Mix"), 		fontcolour(white), 		channel("mix"), 	range(0, 1.00, 1),   colour( 79, 81, 70), trackercolour(240,210,170)
rslider bounds(340, 11, 70, 70), text("Level"),	 	fontcolour(white), 		channel("amp"), 	range(0, 1.00, 1),   colour( 79, 81, 70), trackercolour(240,210,170)
}
image bounds(5, 95, 205, 20), colour(75, 85, 90, 100), plant("credit"), line(0){
label bounds(0.03, 0.1, .95, .7), text("Author: Iain McCurdy |2012|"), colour("white")
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
	ktype		chnget	"type"				;READ WIDGETS...
	kfblvl		chnget	"fblvl"				;
	kfco		chnget	"fco"				;
	kpitchm		chnget	"pitchm"	        	;
	kmix		chnget	"mix"				;
	kamp		chnget	"amp"				;

	ainL,ainR	ins
	denorm		ainL, ainR	;DENORMALIZE BOTH CHANNELS OF AUDIO SIGNAL
	if ktype==0 then
	 kfco	expcurve	kfco, 4	;CREATE A MAPPING CURVE TO GIVE A NON LINEAR RESPONSE
	 kfco	scale	kfco,20000,20	;RESCALE 0 - 1 TO 20 TO 20000
	 kSwitch		changed		kpitchm	;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
	 if	kSwitch=1	then	;IF kSwitch=1 THEN
		reinit	UPDATE		;BEGIN A REINITIALIZATION PASS FROM LABEL 'UPDATE'
	 endif				;END OF CONDITIONAL BRANCHING
	 UPDATE:				;A LABEL
	 arvbL, arvbR 	reverbsc 	ainL, ainR, kfblvl, kfco, sr, i(kpitchm)
	 rireturn			;RETURN TO PERFORMANCE TIME PASSES
	else
	 arvbL, arvbR 	freeverb 	ainL, ainR, kfblvl, 1-kfco
	endif
	amixL		ntrpol		ainL, arvbL, kmix	;CREATE A DRY/WET MIX BETWEEN THE DRY AND THE REVERBERATED SIGNAL
	amixR		ntrpol		ainR, arvbR, kmix	;CREATE A DRY/WET MIX BETWEEN THE DRY AND THE REVERBERATED SIGNAL
			outs		amixL * kamp, amixR * kamp
endin

instr	3	;workaround for failure of these widgets from intiialising properly when used as a plugin
	;chnset	12000,"fco"
	chnset	1,"pitchm"	
endin

		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
i 3 0 0
</CsScore>

</CsoundSynthesizer>
