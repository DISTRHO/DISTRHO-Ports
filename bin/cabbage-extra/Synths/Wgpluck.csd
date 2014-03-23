<Cabbage>
form caption("Waveguide pluck ") size(335, 170), colour("black"), pluginID("plu1")
rslider bounds(0, 0, 80, 80) channel("pick"), tracker(0), range(.1, 1,.5), caption("Pick"), colour("yellow"), midictrl(1, 1)
rslider bounds(85, 0, 80, 80) channel("plucked"), range(0,2,1), caption("Point-plucked"), colour("yellow"), midictrl(1, 2)
rslider bounds(170, 0, 80, 80) channel("damping"), range(1, 100, 10), caption("Damping"), colour("yellow"), midictrl(1, 3)
rslider bounds(255, 0, 80, 80) channel("filter"), range(10, 30000, 100), caption("Filter"), colour("yellow"), midictrl(1, 4)



keyboard pos(0, 85), size(330, 50)

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
kpick	chnget	"pick"
kpoint	chnget	"plucked"
kdamp	chnget		"damping"
kfilt	chnget		"filter"	

kpickx	portk	kpick,.02
	
axcite	oscil	1,1,1	;segnale eccitatore
apluck	wgpluck	ifreq,iamp,kpickx,i(kpoint),i(kdamp),i(kfilt),axcite

aout	clip	apluck,0,0dbfs

kenv	linsegr	1,.1,1,.3,.5,.2,0	;inviluppo d'ampiezza

outs	aout*kenv,aout*kenv


endin



</CsInstruments>
<CsScore>
f1	0	16384	10	1



f0	36000

</CsScore>
</CsoundSynthesizer>