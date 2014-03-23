<Cabbage>
form caption("Autopan / Tremolo") size(440, 102), pluginID("aptr")
image pos( 0,  0),                size(440, 102),     colour("Maroon"), shape("rounded"), outline("white"), line(4) 
rslider  bounds(  5,  6, 90, 90), text("Freq.[Hz]"),  channel("rate"),  range(0.1, 50, 0.5, 0.5),   textBox(1), trackercolour("tomato")
rslider  bounds( 80,  6, 90, 90), text("Tempo[BPM]"), channel("tempo"), range(6, 3000, 30, 0.5, 1), textBox(1), trackercolour("tomato")
rslider  bounds(175,  6, 90, 90), text("Depth"),      channel("depth"), range(0, 1.00, 1, 0.5),     textBox(1), trackercolour("tomato")
rslider  bounds(345,  6, 90, 90), text("Level"),      channel("level"), range(0, 1.00, 1),          textBox(1), trackercolour("tomato")
checkbox bounds(160, 15, 25, 25), colour("yellow"), channel("indicator"),  value(0), shape("rounded")
combobox bounds(260, 13,  90,20), channel("mode"), value(1), text("Autopan", "Tremolo")
combobox bounds(260, 38,  90,20), channel("wave"), value(1), text("Sine", "Triangle", "Square", "Randomi", "Randomh")
checkbox bounds(260, 63, 90, 15), text("TEST TONE"), colour("lime"), channel("test"),  value(0)


</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Author: Iain McCurdy (2012)

opcode	PanTrem,aa,aakkkK
	ainL,ainR,krate,kdepth,kmode,kwave	xin	;READ IN INPUT ARGUMENTS
	ktrig	changed	kwave				;IF LFO WAVEFORM TYPE IS CHANGED GENERATE A MOMENTARY '1' (BANG)
	if ktrig=1 then					;IF A 'BANG' HAS BEEN GENERATED IN THE ABOVE LINE
		reinit	UPDATE				;BEGIN A REINITIALIZATION PASS FROM LABEL 'UPDATE' SO THAT LFO WAVEFORM TYPE CAN BE UPDATED
	endif						;END OF THIS CONDITIONAL BRANCH
	UPDATE:						;LABEL CALLED UPDATE
	iwave	init		i(kwave)
	iwave	limit	iwave,	0, 4			;
	if iwave==3 then				;if randomi chosen...
	 klfo	randomi	-kdepth,kdepth,krate,1
	elseif iwave==4 then				;of randomh chosen...
	 klfo	randomh	-kdepth,kdepth,krate,1		
	else
	 klfo	lfo	kdepth, krate, iwave		;CREATE AN LFO
	endif
	rireturn					;RETURN FROM REINITIALIZATION PASS
	klfo	=	(klfo*0.5)+0.5			;RESCALE AND OFFSET LFO SO IT STAY WITHIN THE RANGE 0 - 1 ABOUT THE VALUE 0.5
	if iwave=2||iwave==4 then					;IF SQUARE WAVE MODULATION HAS BEEN CHOSEN...
		klfo	portk	klfo, 0.001		;SMOOTH THE SQUARE WAVE A TINY BIT TO PREVENT CLICKS
	endif						;END OF THIS CONDITIONAL BRANCH	
	if kmode=0 then	;PAN				;IF PANNING MODE IS CHOSEN FROM BUTTON BANK...
		alfo	interp	klfo			;INTERPOLATE K-RATE LFO AND CREATE A-RATE VARIABLE
		aoutL	=	ainL*sqrt(alfo)		;REDEFINE GLOBAL AUDIO LEFT CHANNEL SIGNAL WITH AUTO-PANNING
		aoutR	=	ainR*(1-sqrt(alfo))	;REDEFINE GLOBAL AUDIO RIGHT CHANNEL SIGNAL WITH AUTO-PANNING
		kindicator	=	(klfo>0.5?1:0)
		chnset	kindicator,"indicator"
	elseif kmode=1 then	;TREM			;IF TREMELO MODE IS CHOSEN FROM BUTTON BANK...
		kindicator	=	(klfo>0.5?1:0)
		chnset	kindicator,"indicator"
		klfo	=	klfo+(0.5-(kdepth*0.5))	;MODIFY LFO AT ZERO DEPTH VALUE IS 1 AND AT MAX DEPTH CENTRE OF MODULATION IS 0.5
		alfo	interp	klfo			;INTERPOLATE K-RATE LFO AND CREATE A-RATE VARIABLE
		aoutL	=	ainL*(alfo^2)		;REDEFINE GLOBAL AUDIO LEFT CHANNEL SIGNAL WITH TREMELO
		aoutR	=	ainR*(alfo^2)		;REDEFINE GLOBAL AUDIO RIGHT CHANNEL SIGNAL WITH TREMELO
	endif						;END OF THIS CONDITIONAL BRANCH
		xout	aoutL,aoutR			;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
krate chnget "rate"
ktempo chnget "tempo"
kdepth chnget "depth"
kmode chnget "mode"
kwave chnget "wave"
klevel chnget "level"
ktest	chnget	"test"

ktrig	changed	krate
ktrig2	changed	ktempo
if ktrig=1 then
 chnset	krate*60,"tempo"
elseif ktrig2=1 then
 chnset	ktempo/60,"rate"
endif

if ktest=1 then
 a1	vco2	0.2,300,4,0.5
 a2	=	a1
else
 a1,a2	ins
endif

a1,a2	PanTrem	a1,a2,krate,kdepth,kmode-1,kwave-1	
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>