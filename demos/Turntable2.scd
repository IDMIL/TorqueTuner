(
/*
////////////// Turntable Demo for TorqueTuner //////////////
Run this demo by placing cursor below this comment and press CMD+return.
Stop it by pressing CMD+.
///////////////////////////////////////////////////////////
*/

// Start libmapper
s.waitForBoot({

	var numDevices = 3;
	var win, layout;
	var modeNames = ["Click", "Wall", "Linspring", "Spin"];
	var modeValues = [0, 2, 4, 7];

	Mapper.enable;

	////////////// Load buffer  //////////////
	~samplePath = thisProcess.nowExecutingPath.dirname +/+ "drums-80bpm.wav";
	~sample = Buffer.read(s, ~samplePath);
	10.wait;

	////////////// Output signals  //////////////

	~modeBuses = Array.fill(numDevices, { Bus.control(s, 1) });
	~modeBuses.do({ |bus| bus.set(7) }); // Default to Spin

	numDevices.do { |i|
		{
			MapOut.kr(In.kr(~modeBuses[i]), "Modeselektor_%".format(i+1), 0, 9);
		}.play;
	};

	~lfo = Bus.control(s, 1);
	{
		MapOut.kr(In.kr(~lfo), 'LFO', -30, 30);
	}.play(target: s, addAction: \addToTail);

	// GUI Stuff
	defer {
		win = Window("TorqueTuner Multi-Controller", Rect(100, 100, 600, 300));
		layout = HLayout();

		numDevices.do { |i|
			var deviceLayout = VLayout();
			deviceLayout.add(StaticText().string_("Device %".format(i+1)).align_(\center));

			modeNames.do { |name, mIdx|
				deviceLayout.add(
					Button()
					.states_([[name]])
					.action_({
						~modeBuses[i].set(modeValues[mIdx]);
						"Device % set to %".format(i+1, name).postln;
					})
				);
			};
			layout.add(deviceLayout);
		};

		win.layout = layout;
		win.front;
	};

	////////////// Input signals  //////////////

	~delayTime = Bus.control(s, 1);
	{
		Out.kr(~delayTime, MapIn.kr('DelayTime', 0, 1));
	}.play;
	~delayTime.set(0);

	~playbackSpeed = Bus.control(s, 1);
	{
		Out.kr(~playbackSpeed, MapIn.kr('PlaybackSpeed', -1, 1));
	}.play;
	~playbackSpeed.set(0);

	////////////// Synth Defs  //////////////

	Ndef(\lfoPlay, { arg amount = 0, freq = 2, offset = 5;
		var sig = SinOsc.kr(freq);
		Out.kr(~lfo, sig * amount + offset);
	}).play;

	// Turntable velocity mode with delay
	Ndef(\vinyl, {
		var buf, speed, echo, delayParam, dTime, dMix;

		// Turntable playback
		speed = In.kr(~playbackSpeed).linlin(-1, 1, -4, 4);
		buf = PlayBuf.ar(2, ~sample, speed * BufRateScale.kr(~sample), loop: 1);

		// Delay logic, delayParam maps 0 to 1. Lag to filter jittery control
		delayParam = Lag.kr(In.kr(~delayTime), 0.1);

		// Map 0 to 1 to delay times from 10ms to 500ms
		dTime = delayParam.linlin(0, 1, 0.01, 0.50);

		// CombL (linear-interpolating delay line) with 1.0 s tail
		echo = CombL.ar(buf, 2.0, dTime, 1.0);

		// Filter the echo slightly
		echo = LPF.ar(echo, 3500);

		// Map wet/dry mix where 0 = completely dry. Turns up to a 50/50 blend.
		dMix = delayParam.linlin(0, 0.1, 0, 0.5).clip(0, 0.5);

		// Blend dry and wet signals
		buf = (buf * (1 - dMix)) + (echo * dMix);

		Out.ar(0, buf * 0.4);
	}).play;
});
)