RubbedResonators.csd

This example introduces a method of 'exciting' mode filters that creates an imitation of a resonant object being bowed or rubbed. The excitation signal consists of a stack of sine wave oscillators that match the number and frequencies of the mode filters being used. This arrangement can cause the mode filters to resonate with great intensity so the amplitudes of the sine wave oscillators should be kept low to prevent overloading. Additionally the frequency of each sine wave oscillator is individually modulated using a random 'jitter' function created using 'gaussi' (interpolating gaussian noise generator). This technique results is a shifting spectral response from the filters. Adjusting the rate and amplitude of this jitter function can imitate a variety of bowing and rubbing techniques. Increasing 'Jit.Dep.' while 'Jit.Rate' is kept low gives the impression of the resonator being rubbed with greater pressure. If both 'Jit.Dep.' and 'Jit.Rate' are high the impression is of the resonator begin brushed. 
If 'Q' is low then more of the character of the impulse sound (the sine wave oscillators) will be apparent (and therefore the jitter modulation will be more apparent). If 'Q' is higher then the character of the mode filters will dominate and modulation of the impulse sound frequencies will be less apparent. A key aspect of this instrument is that once the impulse sound is removed ('Impulse Amp.' is brought to minimum) the sound produced is solely the residual resonance of the mode filters, therefore any modulation within the sine wave oscillators becomes irrelevant.

For brightness (damping) control to work properly modal ratios in their function table will need to be in ascending order.

Chorus effect is disabled when 'Mix' is zero

Number of partials in the chosen algorithm is printed to the GUI for the user's information. Algorithms with high numbers of partials will demand more CPU and lower polyphony will be possible.

<Cabbage>
form caption("Rubbed Resonators"), size(440, 320), pluginID("RubR")
image pos(0, 0), size(440, 290), colour("Sienna"), shape("rounded"), outline("brown"), line(4)

;IMPULSE
groupbox bounds(10, 10, 260, 90), text("Impulse"), fontcolour("white"){
rslider bounds( 10, 35,60,60), text("Amp."), colour("Chocolate"), channel("ImpDB"), range(-70, 0, 0)
rslider bounds( 60, 35,60,60), text("HPF"), colour("Chocolate"), channel("HPF"), range(20, 20000, 20, 0.5)
rslider bounds(110, 35,60,60), text("LPF"), colour("Chocolate"), channel("LPF"), range(20, 20000, 20000, 0.5)
rslider bounds(160, 35,60,60), text("Jit. Dep."), channel("JitDep"), colour("Chocolate"), range(0, 5, 0.4,0.5)
rslider bounds(210, 35,60,60), text("Jit. Rate"), channel("JitRte"), colour("Chocolate"), range(0.01,100, 3,0.5)
}

;RESONATORS
groupbox bounds(270, 10,160,180), text("Resonators"), fontcolour("white"){
label    bounds(320, 40,120, 12), text("Instrument"), colour("white")
combobox bounds(280, 55,140, 25), channel("sound"), value(4), text("Single", "Dahina", "Banyan", "Xylophone", "Tibetan Bowl 180mm", "Spinel Sphere", "Pot Lid", "Red Cedar Wood Plate", "Tubular Bell", "Redwood Wood Plate", "Douglas Fir Wood Plate", "Uniform Wooden Bar", "Uniform Aluminium Bar", "Vibraphone 1", "Vibraphone 2", "Chalandi Plates", "Tibetan Bowl 152mm", "Tibetan Bowl 140mm", "Wine Glass", "Small Handbell", "Albert Clock Bell", "Wood Block","Harmonic 10","Harmonic 20","Harmonic 30","Harmonic Odd 10","Harmonic Odd 20")
rslider  bounds(315, 85, 70, 30), text("N.Partials"), TextBox(1), channel("npartials"), range(1, 10000, 1, 1, 1)
rslider  bounds(270,125, 60, 60), text("Q"), colour("orange"), channel("Q"), range(50, 10000, 2000, 0.5)
rslider  bounds(320,125, 60, 60), text("Bright"), colour("orange"), channel("bright"), range(-4, 4, 0)
rslider  bounds(370,125, 60, 60), text("Gain"), colour("orange"), channel("gain"), range(0, 8, 1)
}

;POLYPHONY
groupbox bounds( 10,100,100, 90), text("Polyphony"), fontcolour("white"){
button   bounds( 20,125, 80, 20), text("poly","mono"), channel("monopoly"), value(0), fontcolour("lime")
hslider  bounds( 15,143, 90, 38), colour("chocolate"), channel("GlissTime"), range(0.005, 0.3, 0.1, 0.25, 0.001)
label    bounds( 32,174, 90, 12), text("Gliss Time")
}

;CHORUS
groupbox bounds(110,100,160, 90), text("Chorus"), fontcolour("white"){
rslider  bounds(110,125, 60, 60), text("Mix"), channel("ChoMix"), range(0, 1, 0.5), colour("yellow")
rslider  bounds(160,125, 60, 60), text("Depth"), channel("ChoDep"), range(0, 0.1, 0.01,0.5, 0.001), colour("yellow")
rslider  bounds(210,125, 60, 60), text("Rate"), channel("ChoRte"), range(0, 20, 0.96, 0.5), colour("yellow")
}

keyboard bounds(10, 195, 420, 85)

image bounds(5, 295, 240, 22), colour(75, 85, 90, 100), plant("credit"){
label bounds(0.03, 0.15, .9, .7), text("Author: Iain McCurdy |2012|"), colour("white")
}

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100
ksmps 		= 	64
nchnls 		= 	2
0dbfs		=	1	;MAXIMUM AMPLITUDE

massign	0,2	;MIDI DATA DIRECTED TO INSTRUMENT 2

;Author: Iain McCurdy (2012)

;FUNCTION TABLES STORING MODAL FREQUENCY RATIOS------------------------------------------------------------------------------------------------------------------------------------------------
;single
girtos1		ftgen	0,0,2, -2,	1,1

;dahina
girtos2		ftgen	0,0,-6,-2,	1,	2.89,	4.95,	6.99,	8.01,	9.02

;banyan
girtos3		ftgen	0,0,-6,-2,	1, 	2.0, 	3.01, 	4.01, 	4.69, 	5.63

;xylophone
girtos4		ftgen	0,0,-6,-2,	1, 	3.932, 	9.538,	16.688,	24.566,	31.147

;tibetan bowl (180mm)
girtos5		ftgen	0,0,-7,-2,	1, 	2.77828,	5.18099, 	8.16289,	11.66063,	15.63801,	19.99

;spinel sphere with diameter of 3.6675mm
girtos6		ftgen	0,0,-24,-2,	1,	1.026513174725,	1.4224916858532,	1.4478690202098,	1.4661959580455,	1.499452545408,	1.7891839345101,	1.8768994627782,	1.9645945254541,	1.9786543873113,	2.0334612432847,	2.1452852391916,	2.1561524686621,	2.2533435661294,	2.2905090816065,	2.3331798413917,	2.4567715528268,	2.4925556408289,	2.5661806088514,	2.6055768738808,	2.6692760296751,	2.7140956766436,	2.7543617293425,	2.7710411870043 

;pot lid
girtos7		ftgen	0,0,-6,-2,	1, 	3.2, 	6.23, 	6.27, 	9.92, 	14.15

;red cedar wood plate
girtos8		ftgen	0,0,-4,-2,	1, 	1.47, 	2.09, 	2.56

;tubular bell
girtos9		ftgen	0,0,-10,-2,	272/437,	538/437,	874/437,	1281/437,	1755/437,	2264/437,	2813/437,	3389/437,	4822/437,	5255/437

;redwood wood plate
girtos10	ftgen	0,0,-4,-2,	1, 1.47, 2.11, 2.57

;douglas fir wood plate
girtos11	ftgen	0,0,-4,-2,	1, 1.42, 2.11, 2.47

;uniform wooden bar
girtos12	ftgen	0,0,-6,-2,	1, 2.572, 4.644, 6.984, 9.723, 12

;uniform aluminum bar
girtos13	ftgen	0,0,-6,-2,	1, 2.756, 5.423, 8.988, 13.448, 18.680

;vibraphone 1
girtos14	ftgen	0,0,-6,-2,	1, 3.984, 10.668, 17.979, 23.679, 33.642

;vibraphone 2
girtos15	ftgen	0,0,-6,-2,	1, 3.997, 9.469, 15.566, 20.863, 29.440

;Chalandi plates
girtos16	ftgen	0,0,-5,-2,	1, 1.72581, 5.80645, 7.41935, 13.91935

;tibetan bowl (152 mm)
girtos17	ftgen	0,0,-7,-2,	1, 2.66242, 4.83757, 7.51592, 10.64012, 14.21019, 18.14027

;tibetan bowl (140 mm)
girtos18	ftgen	0,0,-5,-2,	1, 2.76515, 5.12121, 7.80681, 10.78409

;wine glass
girtos19	ftgen	0,0,-5,-2,	1, 2.32, 4.25, 6.63, 9.38

;small handbell
girtos20	ftgen	0,0,-22,-2,	1, 1.0019054878049, 1.7936737804878, 1.8009908536585, 2.5201981707317, 2.5224085365854, 2.9907012195122, 2.9940548780488, 3.7855182926829, 3.8061737804878, 4.5689024390244, 4.5754573170732, 5.0296493902439, 5.0455030487805, 6.0759908536585, 5.9094512195122, 6.4124237804878, 6.4430640243902, 7.0826219512195, 7.0923780487805, 7.3188262195122, 7.5551829268293 

;albert clock bell belfast
;girtos21	ftgen	0,0,-22,-2, 	2.043260,1.482916,1.000000,3.328848,4.761811,1.477056,0.612007,2.661295,1.002793,4.023776,0.254139,2.043916,4.032463,2.659438,4.775560,5.500494,3.331014,0.809697,2.391301, 0.254098,1.901476,2.366563    ;,0.614968,2.046543,1.814887,3.130744,2.484426,0.558874,0.801697,0.070870,3.617036,2.782656
girtos21	ftgen	0,0,-22,-2, 	0.254098,0.254139,0.612007,0.809697,1.000000,1.002793,1.477056,1.482916,1.901476,2.043260,2.043916,3.331014,2.366563,2.391301,2.659438,2.661295,3.328848,4.023776,4.032463,4.761811,4.775560,5.500494

;wood block
girtos22	ftgen	0,0,4,-2,	915/915,1540/915,1863/915,3112/915

;harmonic 10
girtos23	ftgen	0,0,-10,-2,	1,2,3,4,5,6,7,8,9,10

;harmonic 20
girtos24	ftgen	0,0,-20,-2,	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20

;harmonic 30
girtos25	ftgen	0,0,-30,-2,	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30

;harmonic odd 10
girtos26	ftgen	0,0,-10,-2,	1,3,5,7,9,11,13,15,17,19

;harmonic odd 20
girtos27	ftgen	0,0,-20,-2,	1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39

seed	0			;random number generators seeded from the system clock
gasend	init	0		;initialise the global audio send variable
gisine	ftgen	0,0,4096,10,1	;a sine wave

;table that stores indicators for each active note. 1=active 0=inactive index_location=note_number
giNoteActive	ftgen	0,0,128,2,0
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

gkbright	init	0
gasend		init	0

;--UDOS--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
opcode	SineVoice,a,kkkiiii
	kfreq,kjitdep,kjitrte,icount,invoices,irtos,isfn	xin
	amix	=	0								;initialise audio mix variable (mixture of all sine wave oscillators)
	kjit	gaussi	kjitdep, 1, kjitrte						;jitter function (semitone deviation)
	irto	table	icount-1,irtos							;read the frequency ratio for this oscillator from the appropriate ratios table
	a1	oscili	1,semitone(kjit)*kfreq*irto,isfn				;create the oscillator
	if icount<invoices then								;if not all required oscillators have been created yet... 
	 amix	SineVoice	kfreq,kjitdep,kjitrte,icount+1,invoices,irtos,isfn	;call the udo again, with the incremented counter
	endif
	xout	amix + a1								;send mix of all oscillators back the the udo output
endop

opcode	ModeVoice,a,akkkiii						;mode udo (k-rate base frequency) - used for non-midi mode 
	ain,kfreq,kq,kdamp,icount,inmodes,irtos  xin			;name input variables
	amix	=	0						;initialise audio mix variable (mixture of all mode filters)
	asig	=	0						;initialise audio for the mode filter in this iteration of the UDO. Only needed if only 1 mode is present.
	irto	table	icount-1,irtos					;read frequency ratio for current mode according to iteration counter
	kThisFreq	=	kfreq * irto				;derive modal frequency from product of ratio and base frequency
	if sr/kThisFreq>=$M_PI then
	 asig	mode	ain, kThisFreq, kq				;create mode signal
	 if kdamp>0 then
	  kamp	=	((inmodes-icount+1)^(-kdamp))*(kdamp+1)
	 else
	  kamp	=	icount^kdamp
	 endif
	 aamp	interp	kamp
	 asig	=	asig*aamp
	 if	icount < inmodes	then						;if all the modes in this current instrument have not yet been created...
	  amix		ModeVoice	ain, kfreq,kq,kdamp,icount+1, inmodes,irtos	;...call modemodule udo again with incremented counter
	 endif										;end of conditional branching
	endif
	xout	amix+asig								;send all audio back to caller instrument
endop											;end of udo
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

instr	1	;read in widgets (always on)
	gkQ		chnget	"Q"			;read in widgets
	kImpDB		chnget	"ImpDB"
	gkHPF		chnget	"HPF"
	gkLPF		chnget	"LPF"
	gkJitDep	chnget	"JitDep"
	gkJitRte	chnget	"JitRte"
	gkbright	chnget	"bright"
	gkgain		chnget	"gain"
	gkmonopoly	chnget	"monopoly"
	gkGlissTime	chnget	"GlissTime"
	gksound		chnget	"sound"
	gkChoMix	chnget	"ChoMix"
	gkChoRte	chnget	"ChoRte"
	gkChoDep	chnget	"ChoDep"
	gkImpAmp	=	ampdbfs(kImpDB)		;convert from dB value to amp value
	gkbright	port	gkbright,0.02		;smooth changes using portamento
	gkgain		port	gkgain,0.02		;
	ktrig	changed	gksound				;if sound combobox changes...
	if ktrig==1 then				
	 event	"i",5,0,0.001				;...call instrument 5 to update 'N.Partials' number box
	endif
endin

instr	2	;MIDI TRIGGERED INSTRUMENT
	gkNoteTrig	init	1	;at the beginning of a new note set note trigger flag to '1'	
	ibase	cpsmidi							;read in midi note number as a cps value
	gkbase	=	ibase
	ivel	ampmidi	1						;read in midi note velocity as a value within the range 0 to 1
	gkvel	init	ivel
	inum	notnum							;read in midi note number as an integer (used for create a table of active notes flags)
	krelease	release						;note release flag (0 = note held, 1 = note released)
		tablew	1-krelease,inum,giNoteActive			;write note release flag to table at the location corresponding to note number
	iactive	active	p1+1
		
	if iactive==0||i(gkmonopoly)==0 then				;if polyphonic mode or if this is the first in a series of notes in monophonic mode...
	 event_i	"i",p1+1,0,3600,ibase,inum			;call instrument 3 with the appropriate p-fields
	endif
endin

instr	3				;impulse and modal resonators instrument
	ktrig	changed	gksound
	if ktrig==1 then
	 reinit RESTART_INSTRUMENT
	endif
	RESTART_INSTRUMENT:
	isound	init	i(gksound)
	if gkmonopoly==1 then
	/*monophonic*/
	 kporttime	linseg	0,0.001,1
	 kbase		portk	gkbase,kporttime*gkGlissTime	
	 kactive		active	p1-1
	 kactive		limit	kactive,0,1	
	else
	/*polyphponic*/
	 kbase		init	p4					;base frequency read from p4	
	 kactive		table	p5,giNoteActive			;check whether the midi key corresponding to this note is being held or has been released (1 = held, 0 = released). this value will be used to control whether the impulse scound should be active or not.
	endif
	
	inum		=	p5					;note number
	
	icount		=	1					;counter to count iterations for recursive udos for sine oscillators and mode filters (starts at zero)
	irtos		=	girtos1 + isound - 1			;derive actual function table number for ratios according to 'sound' chosen
	invoices	=	ftlen(irtos)				;derive the number of voices needed for the 'sound' chosen
	kactivePort	port	kactive,0.05				;smooth note active on/off switching
	ktrig changed gkNoteTrig					;if a new note is started...
	if ktrig==1 then						
	 reinit RESTART_ENVELOPE					;...reinitialise from the given label
	endif
	RESTART_ENVELOPE:						;if a new note is trigger reinitialisation from here occurs to start a new impulse buid-up envelope based on new velocity value
	ivel	init	i(gkvel)
	kenv		linseg	0,(1-ivel)+0.01,ivel			;create an amplitude envelope for the impulse sound. impulse sound will be velocity sensitive and the attack time of this envelope will be proportionate to the note velocity also. 
	rireturn

	ktrig	changed	gksound					; to fix bug where sound would not update in mono mode
	if ktrig==1 then
	 reinit RESTART_INSTRUMENT2
	endif
	RESTART_INSTRUMENT2:

	aSineMix	SineVoice	kbase,gkJitDep,gkJitRte,icount,invoices,irtos,gisine	;call sine oscillator udo (it will be recursively recalled within the udo the appropriate number of times according to invoices)
	aSineMix	=	aSineMix*0.002*kactivePort*kenv*gkImpAmp	;scale the mixture of sines audio signal
	aSineMix	buthp	aSineMix,gkHPF
	aSineMix	butlp	aSineMix,gkLPF	 
	amodes			ModeVoice	aSineMix,kbase,gkQ,gkbright,icount,invoices,irtos	;call sine oscillator udo (it will be recursively recalled within the udo the appropriate number of times according to invoices)
	amodes		=		(amodes*gkgain*0.2)/invoices					;scale the amplitude of the sound according to the number of modes in the chosen algorithm
	krms		rms		amodes								;track the amplitude of the sound as an rms value
	ithreshold	=	0.0001
	if krms<ithreshold&&kactive=0 then			;if sound has died away and key is inactive...
	 turnoff						;turn instrument off
	endif
	xtratim	0.1						;this is needed to prevent initialisation crashes, I'm not sure why 
			outs	amodes,amodes			;send mode filters output mixture to output and rescale according to the number of modes used
	gasend	=	gasend+amodes*gkChoMix
	gkNoteTrig	=	0				;reset new-note trigger (in case it was '1')
endin

instr	4	;Chorus effect
	if gkChoMix==0 goto SKIP_CHORUS
	kporttime	linseg	0,0.001,1
	kporttime	=	kporttime/gkChoRte
	kdlt1	randomi	ksmps/sr,gkChoDep,gkChoRte,1
	kdlt1	portk	kdlt1,kporttime
	adlt1	interp	kdlt1
	acho1	vdelay	gasend,adlt1*1000,1*1000
	
	kdlt2	randomi	ksmps/sr,gkChoDep,gkChoRte,1
	kdlt2	portk	kdlt2,kporttime
	adlt2	interp	kdlt2
	acho2	vdelay	gasend,adlt2*1000,1*1000
	
	kpan1	randomi	0,1,gkChoRte,1
	kpan2	randomi	0,1,gkChoRte,1
	a1L,a1R	pan2	acho1,kpan1
	a2L,a2R	pan2	acho2,kpan2
	achoL	=	a1L+a2L
	achoR	=	a1R+a2R
		outs	achoL, achoR
		clear	gasend
	SKIP_CHORUS:
endin

instr	5	;update 'Number of Partials' number box
	invoices	=	ftlen(girtos1+i(gksound)-1)	;derive the number of voices needed for the 'sound' chosen
	knvoices	init	invoices			;create krate version of invoices
	chnset		knvoices,"npartials"			;send value to number box
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
i 4 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>