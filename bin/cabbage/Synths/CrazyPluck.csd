<Cabbage>
form caption("CrazyPluck"), size(300, 220), pluginID("cra1") 
groupbox bounds(0, 0, 300, 100), text("Parameters"), plant("control1"){ 
rslider bounds(.05, .3, .6, .6), text("ratio"), colour("white"), channel("ratio"), range(1, 1.2,1)
rslider bounds(.28, .3, .6, .6), text("time"), colour("white"), channel("time"), range(.001,.1,.1)
rslider bounds(.52, .3, .6, .6), text("random"), colour("white"), channel("random"), range(.001,.1,0)
rslider bounds(.75, .3, .6, .6), text("Event number"), colour("white"), channel("number"), range(1,50,10)
}
keyboard bounds(0, 110, 300, 70)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -m0d -+rtmidi=NULL -M0 
</CsOptions>
<CsInstruments>

;Author: Giorgio Zucco (2012)


sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1
		


instr	1

giamp	ampmidi	1	;controllo midi
gifreq	cpsmidi

instrument	=	10	;strumento da controllare
inum=1	;per attivare la sezione loop
idelay = 0
inumber	chnget	"number"	;numero di eventi da generare	
idur	=	2	;durata evento	
iratio	chnget	"ratio"	;1,1,1,1.2	;slider che controlla il pitch transpose
itime	chnget	"time"	;1,2,.001,.1	;slider che controlla il delay tra un evento e il successivo
irndtime	chnget	"random"	;1,3,.001,.1 ;slider che controlla la variazione random del ritardo

loop:	;inizia la sezione da reiterare

gifreq	=	gifreq*iratio	;pitch controllato da slider 1
idelay	=	idelay+itime+rnd(irndtime)	;delay controllato da slider 2 e 3

event_i "i",instrument,idelay,idur,giamp,gifreq,abs(inumber)

loop_le inum,1,abs(inumber),loop	;ripete il processo per 20 istanze (inumber)

endin


instr	10	

;a1	pluck	(p4*1-p6)/p6,p5,p5,0,1
ival	=	(p4*1-p6)
a1	pluck	ival/p6,p5,p5,0,1

; Or using the new csound 6 function syntax
;a1	=	pluck((p4*1-p6)/p6,p5,p5,0,1)


aout	clip	a1,0,0dbfs
	
kenv	adsr	0.01,.5,.2,.1	;inviluppo

outs	aout*kenv,aout*kenv

endin

</CsInstruments> 
<CsScore>

;i1 0	36000
f0	36000	



</CsScore>
 </CsoundSynthesizer>
