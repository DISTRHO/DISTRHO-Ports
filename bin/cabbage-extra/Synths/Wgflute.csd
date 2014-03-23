<Cabbage>
form caption("Waveguide flute ") size(335, 165), colour("black"), pluginID("flu")
rslider bounds(0, 0, 80, 80) channel("jet"), range(0.08,0.56,0.08), caption("Air jet"), colour("yellow"), midictrl(1, 1)
rslider bounds(85, 0, 80, 80) channel("vib"), range(0,12,1), caption("Vib rate"), colour("yellow"), midictrl(1, 2)
rslider bounds(170, 0, 80, 80) channel("vibamp"), range(0,1,0), caption("Vib amp"), colour("yellow"), midictrl(1, 3)
rslider bounds(255, 0, 80, 80) channel("noise"), range(0,0.6,0.1), caption("Noise"), colour("yellow"), midictrl(1, 3)
keyboard bounds(0, 85, 330, 50)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=NULL -M0 
;-n -d -+rtmidi=NULL -M0
</CsOptions>
<CsInstruments>

;Author: Giorgio Zucco (2012)


sr = 44100
ksmps = 10
nchnls = 2
0dbfs = 1


instr	1


iamp	ampmidi	1
ifreq	cpsmidi
kjet 	chnget	"jet"
kvibf	chnget	"vib"
kngain	chnget	"noise"
kstiff	chnget		"stiff"
kvamp	chnget		"vibamp"	


iatt = 0.1
idetk = 0.1

aflute	wgflute iamp, ifreq, kjet, iatt, idetk, kngain, kvibf, kvamp, 1

aout	clip	aflute,0,0dbfs

kenv	linsegr	1,.1,1,.3,.5,.2,0	;inviluppo d'ampiezza

outs	aout*kenv,aout*kenv


endin



</CsInstruments>
<CsScore>
f1	0	16384	10	1



f0	36000

</CsScore>
</CsoundSynthesizer>