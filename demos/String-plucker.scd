(
/*
////////////// String-Plucker Demo for TorqueTuner //////////////
Run this demo by placing cursor below this comment and press CMD+return.
Stop it by pressing CMD+.
///////////////////////////////////////////////////////////
*/

// Start libmapper
s.waitForBoot({
	Mapper.enable;
	1.wait;
	////////////// Output signals  //////////////
	~mode = Bus.control;
	{
		MapOut.kr(In.kr(~mode), 'Modeselektor', 0, 7);
	}.play;


	////////////// Input signals  //////////////

	~freq = Bus.control;
	{
		Out.kr(~freq, MapIn.kr('Frequency', 20, 20000));
	}.play;

	~trigger = Bus.control;
	{
		Out.kr(~trigger, MapIn.kr('Excitation', 0, 1));
	}.play;

	////////////// Synth Defs  //////////////

	// Modeselektor

	Ndef(\modePlay,{ arg mode = 5;
		Out.kr(~mode,mode)
	}).play;

	// String-plucker synth
	Ndef(\modePlay).set(\mode,0);
	Ndef(\string,{
		var trig = Changed.kr(In.kr(~trigger));
		var sig = Pluck.ar(WhiteNoise.ar(0.1),trig,0.1,(In.kr(~freq)).lincurve(20,20000,0.0005,0.05,8),15,0.2).dup;
		Out.ar(0,sig);
	}).play;
});
)


