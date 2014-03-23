Clavinet.csd - an emulation of the Hohner Clavinet D6
-----------------------------------------------------

Pickup Selection
----------------
CA - neck pickup only (pickup A)
CB - bridge pickup only (pickup B)
DA - both pickups in phase
DB - bridge pickup (pickup B) 180 degrees out of phase w.r.t the neck pickup 

Setup
-----
Pick A - position of the first pickup (range 0 - 1). Typically referred to as the 'neck' pickup
Pick B - position of the second pickup (range 0 - 1). Typically referred to as the 'bridge' pickup
 (NB. the relevant pickup has to be 'active' according to the CD AB switches in order for you to be able to here changes to it pickup position.)
Mod.A - the pickup position can also modulated by an LFO. This control controls the depth of this modulation
Mod.B - Depth of this modulation for the second pickup
Rate A - Rate of modulation for the first pickup
Rate B - Rate of modulation for the second pickup
Env.A - The pickup position can be modulated by a simple single segment envelope. This is typically used to add an particualar attack nuance to the sound. This control controls the initial amplitude of this envelope (can be positive or negative). The envelope always ends at zero.
Env.B - Envelope amplitude for the second pickup.
Time A - Duration of the envelope segment for the first pickup
Time B - Duration of the envelope segment for the second pickup
Pluck - location of the 'pluck' (range 0 - 1). Should be close to zero for a typical clavinet behaviour
Damp - string vibration damping that is applied while a note is being held. String damping is also dependent upon the note played using a 'key tracking' function table 'girefl'. In general lower notes should experience greater damping.
HPF - this controls the cutoff frequency of a highpass filter that the signal passes through. 
      The value supplied here is multiplied by the fundemental frequency of the note played to derive the cutoff frequency.
      Therefore if a value of '2' is supplied the cutoff will always by twice the fundemental, i.e. the frequency of the first harmonic/second partial.
      This control can be used to remove the emphasis of the fundemental partial.
      If this control is at its minimum setting the filter is bypassed.
LPF - behaves in a similar way to HPF but in respect to a lowpass filter. This control, if reduced, can be used to soften the brightness of a note's attack.
      If this control is at its maximum setting the filter is bypassed.
Inharm - applies inharmonicity to the spectrum produced by repluck. Negative values compress the spectrum, positive values expand the spectrum and zero leaves the spectrum unaffected. 

Release
-------
 * Poorly maintained clavinets exhibit a release 'pluck' as the rubber pad which strikes and frets is lifted from the string when the key is released. The is on account of the rubber pad wearing out.
Ampl - amplitude of the release 'pluck'
Tone - a tone control applied to the release 'pluck' (this will also affect amplitude)
D.Time - time it takes for full damping to be applied (this control has a subtle effect)
Damp - amount of damping that is applied to string vibrations whenever a key is
released (this amount of damping is achieved after 'D.Time' seconds). Note that the 'Damp'
control in the 'Setup' pop-up panel also has an impact upon the release damping.
A.Time - amplitude release time. If this is too short, vibration damping may not be heard and the above controls may not appear to do anything 

Filters
-------
 The D6 Clavinet design incorporates four bandpass filters which can be switched in or out of the circuit.
 Emphasising four different frequency bands from high to low they are named 'brilliant', 'treble', 'medium' and 'soft'
 Cutoff frequencies (fco), levels and Q values for each of the four filters can be adjusted.
 Obviously the relevant filter has to be activated in order to be able to hear changes made to its settings.
 The four filters are in a parallel arrangement as opposed to being in series.

<Cabbage>
form caption("Clavinet"), size(550, 195), pluginID("clav"), colour(228,156,28)

snapshot bounds(260, 46,200, 25), master(1), items("Clavinet 1", "Clavinet 2", "Clavinet 3", "Clavinet 4", "Clavinet 5", "Clavinet 6", "Clavinet 7", "Clavinet 8", "Clavinet 9", "Clavinet 10")

; filter switches
checkbox bounds( 10,  5, 70, 15), text("Brilliant") channel("brilliant"), FontColour(50,50,50), colour("lime")
checkbox bounds( 10, 25, 70, 15), text("Treble") channel("treble"), FontColour(50,50,50), colour("lime"), value(1)
checkbox bounds( 10, 45, 70, 15), text("Medium") channel("medium"), FontColour(50,50,50), colour("lime")
checkbox bounds( 10, 65, 70, 15), text("Soft") channel("soft"), FontColour(50,50,50), colour("lime")

; pickup configuration switches
checkbox bounds( 80,  5, 70, 15), text("C/D") channel("C_D"), FontColour(50,50,50), colour("lime"), value(1)
checkbox bounds( 80, 25, 70, 15), text("A/B") channel("A_B"), FontColour(50,50,50), colour("lime"), value(1)

; controls pertaining to the setup of the instrument (pickup positions, pluck position, damping etc.) accessed in a pop-up panel
groupbox bounds(150,  8, 172, 305),  colour(228,156,28), plant("Setup), line(0), popup(1)
{
rslider bounds(  5,  8, 55, 55), text("Pick A"), colour(195,126,  0), FontColour(50,50,50), channel("pickA"),   range(0, 1.00, 0.25, 1, 0.001)
rslider bounds( 55,  8, 55, 55), text("Pick B"), colour(195,126,  0), FontColour(50,50,50), channel("pickB"),   range(0, 1.00, 0.07, 1, 0.001)
rslider bounds(  5, 68, 55, 55), text("Mod A"),  colour(225,156, 28),  FontColour(50,50,50), channel("ModA"),   range(0,0.50, 0.03, 0.5, 0.001)
rslider bounds( 55, 68, 55, 55), text("Mod B"),  colour(225,156, 28),  FontColour(50,50,50), channel("ModB"),   range(0,0.50, 0.03, 0.5, 0.001)
rslider bounds(  5,128, 55, 55), text("Rate A"), colour(255,186, 58), FontColour(50,50,50), channel("RateA"),   range(0,14.00, 0.2, 0.5, 0.001)
rslider bounds( 55,128, 55, 55), text("Rate B"), colour(255,186, 58), FontColour(50,50,50), channel("RateB"),   range(0,14.00, 0.15, 0.5, 0.001)
rslider bounds(  5,188, 55, 55), text("Env.A"),  colour(255,216, 88), FontColour(50,50,50), channel("EnvA"),   range(-1, 1.00, 0)
rslider bounds( 55,188, 55, 55), text("Env.B"),  colour(255,216, 88), FontColour(50,50,50), channel("EnvB"),   range(-1, 1.00, 0)
rslider bounds(  5,248, 55, 55), text("Time A"), colour(255,246,118), FontColour(50,50,50), channel("TimeA"),   range(0, 1.00, 0.05, 0.5, 0.001)
rslider bounds( 55,248, 55, 55), text("Time B"), colour(255,246,118), FontColour(50,50,50), channel("TimeB"),   range(0, 1.00, 0.05, 0.5, 0.001)
line bounds(110, 25,  1, 260), colour( 50, 50, 50)
rslider bounds(112,  8, 55, 55), text("Pluck"),  colour(195,126,  0), FontColour(50,50,50), channel("plk"),     range(0, 1.00, 0.005, 0.5, 0.0001)
rslider bounds(112, 68, 55, 55), text("Damp"),   colour(225,156, 28), FontColour(50,50,50), channel("SusDamp"), range(-1.00, 1.00, 0)
rslider bounds(112,128, 55, 55), text("HPF"),    colour(255,186, 58), FontColour(50,50,50), channel("HPF"),     range(0,16.0, 0)
rslider bounds(112,188, 55, 55), text("LPF"),    colour(255,216, 88), FontColour(50,50,50), channel("LPF"),     range(1.0,72, 72, 0.375, 0.0001)
rslider bounds(112,248, 55, 55), text("Inharm."),colour(255,216, 88), FontColour(50,50,50), channel("inharm"),  range(-0.99,0.99, 0)
}

; controls pertaining to the release stage of a note accessed in a pop-up panel
groupbox bounds(260,  8, 265, 65),  colour(228,156,28), plant("Release"), line(0), popup(1)
{
rslider bounds(  5,  8, 55, 55), text("Ampl."),  colour(255,186,58), FontColour(50,50,50), channel("RelAmpl"),  range(0, 20.00, 2)
rslider bounds( 55,  8, 55, 55), text("Tone"),   colour(255,186,58), FontColour(50,50,50), channel("RelTone"),  range(100, 8000, 1000)
rslider bounds(105,  8, 55, 55), text("D.Time"), colour(255,186,58), FontColour(50,50,50), channel("RelDTime"), range(0.001, 1, 0.01, 0.5, 0.001)
rslider bounds(155,  8, 55, 55), text("Damp"),   colour(255,186,58), FontColour(50,50,50), channel("RelDamp"),  range(0.10, 0.99, 0.95)
rslider bounds(205,  8, 55, 55), text("A.Time"), colour(255,186,58), FontColour(50,50,50), channel("RelATime"), range(0.01,20, 0.05, 0.5, 0.01)
}

; controls pertaining to the setup of the instrument's bandpass filters accessed in a pop-up panel
groupbox bounds(150, 46, 230, 195),  colour(228,156,28), plant("Filters"), line(0), popup(1)
{
label    bounds( 10,  5, 60, 12), text("Brilliant"), colour("white"), FontColour(50,50,50)
label    bounds( 69,  5, 60, 12), text("Treble"),    colour("white"), FontColour(50,50,50)
label    bounds(121,  5, 60, 12), text("Medium"),    colour("white"), FontColour(50,50,50)
label    bounds(185,  5, 60, 12), text("Soft"),      colour("white"), FontColour(50,50,50)
checkbox bounds( 26, 18, 10, 10), channel("brilliant"), colour("lime"), value(0), shape("ellipse")
checkbox bounds( 81, 18, 10, 10), channel("treble"),    colour("lime"), value(0), shape("ellipse")
checkbox bounds(136, 18, 10, 10), channel("medium"),    colour("lime"), value(0), shape("ellipse")
checkbox bounds(191, 18, 10, 10), channel("soft"),      colour("lime"), value(0), shape("ellipse")
rslider  bounds(  5, 30, 55, 55), channel("fco1"),  text("fco.1"),  colour(235,166,38), FontColour(50,50,50), range(50,5000,2500,0.5)
rslider  bounds( 60, 30, 55, 55), channel("fco2"),  text("fco.2"),  colour(235,166,38), FontColour(50,50,50), range(50,5000,1000,0.5)
rslider  bounds(115, 30, 55, 55), channel("fco3"),  text("fco.3"),  colour(235,166,38), FontColour(50,50,50), range(50,5000, 500,0.5)
rslider  bounds(170, 30, 55, 55), channel("fco4"),  text("fco.4"),  colour(235,166,38), FontColour(50,50,50), range(50,5000, 200,0.5)
rslider  bounds(  5, 85, 55, 55), channel("gain1"), text("Gain 1"), colour(255,216,88), FontColour(50,50,50),  range(0.50,30,  3,0.5)
rslider  bounds( 60, 85, 55, 55), channel("gain2"), text("Gain 2"), colour(255,216,88), FontColour(50,50,50),  range(0.50,30,2.3,0.5)
rslider  bounds(115, 85, 55, 55), channel("gain3"), text("Gain 3"), colour(255,216,88), FontColour(50,50,50),  range(0.50,30,1.3,0.5)
rslider bounds(170, 85, 55, 55), channel("gain4"),  text("Gain 4"), colour(255,216,88),  FontColour(50,50,50),  range(0.50,30,1.4,0.5)
rslider bounds(  5,140, 55, 55), channel("Q1"),     text("Q 1"),    colour(255,255,138), FontColour(50,50,50),  range(0.50,30,1.25,0.5)
rslider bounds( 60,140, 55, 55), channel("Q2"),     text("Q 2"),    colour(255,255,138), FontColour(50,50,50),  range(0.50,30,1   ,0.5)
rslider bounds(115,140, 55, 55), channel("Q3"),     text("Q 3"),    colour(255,255,138), FontColour(50,50,50),  range(0.50,30,1   ,0.5)
rslider bounds(170,140, 55, 55), channel("Q4"),     text("Q 4"),    colour(255,255,138), FontColour(50,50,50),  range(0.50,30,2   ,0.5)
line bounds( 59, 25,  1, 150), colour(50,50,50)
line bounds(114, 25,  1, 150), colour(50,50,50)
line bounds(169, 25,  1, 150), colour(50,50,50)
}

rslider  bounds(470,  5, 75, 75), text("Level"), colour(255,186,58), FontColour(50,50,50), channel("level"), range(0, 8.00, 0.5, 0.5)

keyboard bounds(10,  85, 530,80)
image bounds(5, 170, 420, 20), colour(75, 85, 90, 50), plant("credit"){
label bounds(0.03, 0.1, .6, .7), text("Author: Iain McCurdy |2013|"), colour("white"), FontColour(50,50,50)
}

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100
ksmps 		= 	16
nchnls 		= 	2
0dbfs		=	1

; table used to map of note played to bridge relection
girefl	ftgen	0,0,128,-27, 0,0.4, 36,0.4, 72,0.01, 127,0.01
gisine	ftgen	0,0,4096,10,1
gitri	ftgen	0,0,4096,7,0,1024,1,2046,-1,1024,0

opcode	FreqShifter,a,aki
	ain,kfshift,ifn	xin					;READ IN INPUT ARGUMENTS
	areal, aimag hilbert ain				;HILBERT OPCODE OUTPUTS TWO PHASE SHIFTED SIGNALS, EACH 90 OUT OF PHASE WITH EACH OTHER
	asin 	oscili       1,    kfshift,     ifn,          0
	acos 	oscili       1,    kfshift,     ifn,          0.25	
	;RING MODULATE EACH SIGNAL USING THE QUADRATURE OSCILLATORS AS MODULATORS
	amod1	=		areal * acos
	amod2	=		aimag * asin	
	;UPSHIFTING OUTPUT
	aFS	= (amod1 - amod2)
		xout	aFS				;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr	1
ibrilliant	chnget	"brilliant"	; filter switch
itreble		chnget	"treble"	; filter switch
imedium		chnget	"medium"	; filter switch
isoft		chnget	"soft"		; filter switch
iC_D	chnget	"C_D"			; single/dual pickup selector
iA_B	chnget	"A_B"			; neck/bridge pickup selector
gklevel	chnget	"level"			; output volume control

icps	cpsmidi			; cps read from midi
inum	notnum			; note number read from midi
ivel	veloc		0,1	; velocity read from midi

; pluck position is an i-rate variable so a mechanism is created to re-initialise the instrument if it is changed in realtime
kplk	chnget	"plk"		; pluck position
ktrig	changed	kplk		; if GUI knob changes, generate a trigger
if ktrig==1 then		; if triggered...
 reinit	UPDATE			; reinitialise...
endif				
UPDATE:				; ...from here (until the end of the instrument)
iplk	=	i(kplk)		; cast k-rate pluck position to i-rate variable

; pickup positions
kpickA	chnget	"pickA"		; neck pickup position
kpickB	chnget	"pickB"		; bridge pickup position

; string vibration damping / reflectivity
irefl	table		inum,girefl			; read reflectivity for this note from function table (lower notes will experience more damping)
iRelDTime	chnget	"RelDTime"			; release damping time: time for damping state to be established
iRelDamp	chnget	"RelDamp"			; amount of damping upon release
krefl		linsegr	irefl,iRelDTime,iRelDamp	; upon note release vibrations are damped quickly and dramatically
kSusDamp	chnget	"SusDamp"			; user controlled modulation of string vibration damping
krefl		limit	krefl+kSusDamp,0.001,0.999	; consolidate reflection/damping value and limit possible values to prevent out of range values

iamp	=		ivel*0.3
kenv	expseg		1, 0.01,0.001,1,0.001	; create an amplitude envelope for the noise burst
axcite	pinkish		kenv-0.001		; create a pink noise signal
axcite	tone		axcite,icps*2
krelease	release				; sense release of note
iRelAmpl	chnget	"RelAmpl"		; release pluck amplitude
iRelTone	chnget	"RelTone"		; tone of the release pluck
if krelease==1 then				; if key is released...
 axcite	mpulse	iRelAmpl,0			; audio impulse is created as an implementation of the release pluck impulse 
 axcite	butlp	axcite,iRelTone			; lowpass filter the impulse
endif

kModA	chnget		"ModA"		; pickup position modulation depth
kModB	chnget		"ModB"
kRateA	chnget		"RateA"		; pickup position modulation rate
kRateB	chnget		"RateB"

iEnvA	chnget		"EnvA"		; pickup position envelope modulation depth
iEnvB	chnget		"EnvB"
iTimeA	chnget		"TimeA"		; pickup position envelope modulation time
iTimeB	chnget		"TimeB"

kenv1	transeg		iEnvA,iTimeA,-4,0		; envelope applied to the pickup position (pickup A)
kenv2	transeg		iEnvB,iTimeB,-4,0		; envelope applied to the pickup position (pickup B)
kmod1	oscili		kModA,kRateA,gitri		; lfo used to modulate pickup position (pickup A)
kmod2	oscili		kModB,kRateB,gitri		; lfo used to modulate pickup position (pickup B)
kpickA	limit		kpickA + kmod1 + kenv1, 0, 1	; consolidate pickup position between fixed value, lfo and envelope. Also protect against out of range values. (pickup A)
kpickB	limit		kpickB + kmod2 + kenv2, 0, 1	; consolidate pickup position between fixed value, lfo and envelope. Also protect against out of range values. (pickup B)

iinharm	chnget	"inharm"				; inharmonicity of the tone. negative values compress the spectrum, positive values expand it. Compensation will be applied to the fundemental so that it will remain consistent.
iFShift	=	icps*iinharm				; amount of frequency shift (in CPS)

a1 	repluck 	iplk, iamp, icps-iFShift, kpickA, krefl, axcite	; employ plucked string model (pickup A)
a2 	repluck 	iplk, iamp, icps-iFShift, kpickB, krefl, axcite	; employ plucked string model (pickup B)

if(iinharm!=0) then				; only apply frequency shifting if inharmonicity is anything other than zero
 a1	FreqShifter	a1,iFShift,gisine	; call frequency shifting UDO
 a2	FreqShifter	a2,iFShift,gisine
endif

if(iC_D==0&&iA_B==0) then	;CA - neck pickup only
 asig	=	a1
elseif(iC_D==0&&iA_B==1) then	;CB - bridge pickup only
 asig	=	a2
elseif(iC_D==1&&iA_B==0) then	;DA - both pickups in phase
 asig	=	a1 + a2
elseif(iC_D==1&&iA_B==1) then	;DB - both pickups. Pickup B 180 degrees out of phase
 asig	=	a1 - a2
endif

; velocity to brightness
icf	=		(ivel*6)+8		; cutoff frequency in 'oct' format
asig	butlp		asig,cpsoct(icf)	; lowpass filter audio signal

; filter switches
kfco1		chnget	"fco1"	; filter cutoffs
kfco2		chnget	"fco2"
kfco3		chnget	"fco3"
kfco4		chnget	"fco4"
kQ1		chnget	"Q1"	; filter q's
kQ2		chnget	"Q2"
kQ3		chnget	"Q3"
kQ4		chnget	"Q4"
kgain1		chnget	"gain1"	; filter gains
kgain2		chnget	"gain2"
kgain3		chnget	"gain3"
kgain4		chnget	"gain4"

; filters
amix	=	0	; audio mix of filter outputs (reset to silence upon each iteration)
if(ibrilliant==1) then ; if 'brilliant' selected...
 afilt	rbjeq 	asig, kfco1, 1, kQ1, 1, 4		; bandpass filter sound
 amix	=	amix + (afilt * kgain1)			; add a scaled amount of filtered signal to filter mix signal
endif
if(itreble==1) then ; if 'treble' selected...
 afilt	rbjeq 	asig, kfco2, 1, kQ2, 1, 4		; bandpass filter sound                                      
 amix	=	amix + (afilt * kgain2)                 ; add a scaled amount of filtered signal to filter mix signal
endif	
if(imedium==1) then ; if 'medium' selected...
 afilt	rbjeq 	asig, kfco3, 1, kQ3, 1, 4		; bandpass filter sound                                      
 amix	=	amix + (afilt * kgain3)                 ; add a scaled amount of filtered signal to filter mix signal
endif
if(isoft==1) then ; if 'soft' selected...
 afilt	rbjeq 	asig, kfco4, 1, kQ4, 1, 4		; bandpass filter sound                                      
 amix	=	amix + (afilt * kgain4)                 ; add a scaled amount of filtered signal to filter mix signal
endif
if((ibrilliant + itreble + imedium + isoft)==0) then	; if no filters selected...
 amix	=	asig					; ...just send unfiltered audio to filter section output
endif

; key following highpass filter
kHPF	chnget	"HPF"			; highpass filter multiple
if kHPF>0 then				; if HPF control is greater than 1...
 kcf	limit	icps*kHPF,20,sr/2	; prevent out of range values which would cause the filter to 'blow up'
 amix	buthp	amix,kcf		; filter tone relative to note fundemental
endif

; key following lowpass filter
kLPF	chnget	"LPF"			; lowpass filter multiple
if kLPF<72 then				; if LPF control is greater than 1...
 kcf	limit	icps*kLPF,20,sr/2	; prevent out of range values which would cause the filter to 'blow up'
 amix	butlp	amix,kcf		; filter tone relative to note fundemental
endif

; release envelope
iRelATime	chnget	"RelATime"	; amplitude release time
aenv		linsegr	1, iRelATime, 0	; amplitude release envelope (senses key release before entering final/only segment)

; output signal
aout	=	amix * aenv * gklevel	; apply release envelope and level control
	outs	aout, aout		; send audio to outputs
endin

</CsInstruments>

<CsScore>
f 0 [60*60*24*7]	; keep performance going for up to a week
</CsScore>

</CsoundSynthesizer>