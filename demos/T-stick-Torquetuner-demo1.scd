(
// Start libmapper
s.waitForBoot({
	Mapper.enable;
});
)


////////////// Output signals  //////////////
(
~mode = Bus.control;
{
	MapperOutput.kr(In.kr(~mode), 'Modeselektor', 0, 7);
}.play;

~lfo = Bus.control;
{
	MapperOutput.kr(In.kr(~lfo), 'LFO', -1, 1);
}.play;


////////////// Input signals  //////////////

~freq = Bus.control;
{
	Out.kr(~freq, MapperInput.kr('Frequency', 20, 20000));
}.play;


~cutoff = Bus.control;
{
	Out.kr(~cutoff, MapperInput.kr('FilterCutoff', 0, 1));
}.play;

~delayTime = Bus.control;
{
	Out.kr(~delayTime, MapperInput.kr('DelayTime', 0, 1));
}.play;


~trigger = Bus.control;
{
	Out.kr(~trigger, MapperInput.kr('Excitation', 0, 1));
}.play;

~resonance = Bus.control;
{
	Out.kr(~resonance,MapperInput.kr('Resonance', 0, 1));
}.play;


~playbackSpeed = Bus.control;
{
	Out.kr(~playbackSpeed,MapperInput.kr('PlaybackSpeed',-1,1));
}.play;

~pan = Bus.control;
{
	Out.kr(~pan, MapperInput.kr('Balance', -1, 1));
}.play;


// Modeselektor

Ndef(\modePlay,{ arg mode = 5;
	Out.kr(~mode,mode)
}).play;

Ndef(\lfoPlay,{ arg amount = 0.1, freq  = 2, offset = 0.5;
	var sig = SinOsc.kr(freq).scope;
	Out.kr(~lfo,sig*amount + offset);
}).play;

~sample = Buffer.read(s,"/Users/mathiaskirkegaard/Documents/Music/Samples/looperman-l-2852656-0192348-jaybird-drums-80bpm.wav");

)

Ndef(\lfoPlay).set(\amount, 0.5, \freq, 0.5, \offset, 0.4);


// Filter
(
Ndef(\modePlay).set(\mode,4);
Ndef(\filter,{
	var ctrl_in = In.kr(~cutoff);
	var sig = BLowPass4.ar(LFSaw.ar([99.5,100.5]),ctrl_in.linlin(0,1,20,3000)**[0.99,1.01],In.kr(~resonance).lincurve(0,1,1,0.02,-4));
	var sig2 = BLowPass4.ar(LFSaw.ar([99.5,100.5]*2,0.2),ctrl_in.linlin(0,1,100,5000)**[0.99,1.01],In.kr(~resonance).lincurve(0,1,1,0.02,-4));
	Out.ar(0,(sig + (ctrl_in.lincurve(0,1,0,1,4)*sig2))/2);
}).play;
)

// Filter - pan

(
Ndef(\filter).clear;
Ndef(\modePlay).set(\mode,1);
Ndef(\filter_balance,{
	var ctrl_in = 0.3;
	var sig = BLowPass4.ar(LFSaw.ar([99.5,100.5]),ctrl_in.linlin(0,1,20,3000)**[0.99,1.01],In.kr(~resonance).lincurve(0,1,1,0.02,-4));
	var sig2 = BLowPass4.ar(LFSaw.ar([99.5,100.5]*2,0.2),ctrl_in.linlin(0,1,100,5000)**[0.99,1.01],In.kr(~resonance).lincurve(0,1,1,0.02,-4));
	sig = Mix.new((sig + (ctrl_in.lincurve(0,1,0,1,4)*sig2))/2);
	Out.ar(0,Pan2.ar(sig,In.kr(~pan)));
}).play;
)

// String - trigger mode
(
Ndef(\filter_balance).clear;
Ndef(\modePlay).set(\mode,0);
Ndef(\string,{
	var trig = Changed.kr(In.kr(~trigger));
	var sig = Pluck.ar(WhiteNoise.ar(0.1),trig,0.1,(In.kr(~freq)).lincurve(20,20000,0.0005,0.05,8),15,0.2).dup;
	Out.ar(0,sig);
}).play;
)

// Turntable velocity mode
(
Ndef(\modePlay).set(\mode,7);
Ndef(\exciter).clear;
Ndef(\vinyl, {
	var buf, speed, env;
/*	env = EnvGen.kr(
	Env([0,1,0], [0.1, 0.1], \sin, 1),
		gate, doneAction: 20);*/
	speed = In.kr(~playbackSpeed).linlin(-1,1,-3,3);
	// speed = 1.2;
	buf = PlayBuf.ar(2, ~sample, speed * BufRateScale.kr(~sample), loop: 1);

	buf = CombC.ar(buf,0.2,Lag.kr(In.kr(~delayTime).lincurve(0,0.03,0.08,0.004,8),0.05),In.kr(~delayTime).lincurve(0,0.05,0.5,10,8))*0.14+(buf*0.2);
	Out.ar(0, buf);
}).play;
)

(
Ndef(\vinyl).clear;
Ndef(\modePlay).set(\mode,5);
)

// Wall
Ndef(\modePlay).set(\mode,2);

// Detent
Ndef(\modePlay).set(\mode,0);

// Magnet
Ndef(\modePlay).set(\mode,1);

// Linspring
Ndef(\modePlay).set(\mode,4);

// Expspring
Ndef(\modePlay).set(\mode,4);

// Free
Ndef(\modePlay).set(\mode,6);



