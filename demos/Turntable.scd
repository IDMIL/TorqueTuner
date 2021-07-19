(
/*
////////////// Turntable Demo for TorqueTuner //////////////
Run this demo by placing cursor below this comment and press CMD+return.
Stop it by pressing CMD+.
///////////////////////////////////////////////////////////
*/

// Start libmapper
s.waitForBoot({
	Mapper.enable;

	////////////// Load buffer  //////////////
	~samplePath = thisProcess.nowExecutingPath.dirname +/+ "drums-80bpm.wav";
	~sample = Buffer.read(s,~samplePath);
	1.wait;


	////////////// Output signals  //////////////

	~mode = Bus.control;
	{
		MapOut.kr(In.kr(~mode), 'Modeselektor', 0, 7);
	}.play;

	~lfo = Bus.control;
	{
		MapOut.kr(In.kr(~lfo), 'LFO', -1, 1);
	}.play;


	////////////// Input signals  //////////////


	~delayTime = Bus.control;
	{
		Out.kr(~delayTime, MapIn.kr('DelayTime', 0, 1));
	}.play;
	~delayTime.set(0);


	~playbackSpeed = Bus.control;
	{
		Out.kr(~playbackSpeed,MapIn.kr('PlaybackSpeed',-1,1));
	}.play;
	~playbackSpeed.set(0);

	// Modeselektor

	Ndef(\modePlay,{ arg mode = 5;
		Out.kr(~mode,mode)
	}).play;

	Ndef(\lfoPlay,{ arg amount = 0, freq  = 2, offset = 0.5;
		var sig = SinOsc.kr(freq);
		Out.kr(~lfo,sig*amount + offset);
	}).play;

	// Turntable velocity mode
	Ndef(\modePlay).set(\mode,7);
	Ndef(\vinyl, {
		var buf, speed, env;
		/*	env = EnvGen.kr(
		Env([0,1,0], [0.1, 0.1], \sin, 1),
		gate, doneAction: 20);*/
		speed = In.kr(~playbackSpeed).linlin(-1,1,-3,3);
		buf = PlayBuf.ar(2, ~sample, speed * BufRateScale.kr(~sample), loop: 1);
		buf = CombC.ar(buf,0.2,Lag.kr(In.kr(~delayTime).lincurve(0,0.03,0.08,0.004,8),0.05),In.kr(~delayTime).lincurve(0,0.05,0.5,10,8))*0.14+(buf*0.2);
		Out.ar(0, buf);
	}).play;
});
)


(
/*
////////////// LFO modulation //////////////
To apply lfo do:
 1. Run this section by placing cursor below this comment and press CMD+return
 2. map LFO -> "Target velocity" to modulate the velocity of TorqueTuner.
 3. Try entering new values for amount, freq or offset and 1.
*/
Ndef(\lfoPlay).set(\amount, 0.5, \freq, 0.5, \offset, 0.4);

)



