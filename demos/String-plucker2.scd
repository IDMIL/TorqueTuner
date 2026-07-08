(
/*
////////////// String-Plucker Demo for TorqueTuner //////////////
Run this demo by placing cursor below this comment and press CMD+return.
Stop it by pressing CMD+.
///////////////////////////////////////////////////////////
*/

// Start libmapper
s.waitForBoot({

	var numDevices = 3;
	var win, layout, deviceBoxes;
	var modeNames = ["Click", "Wall", "Linspring"];
	var modeValues = [0, 2, 4];

	Mapper.enable;
	10.wait;

	////////////// Output signals  //////////////
	// Initialize arrays to hold buses and Ndefs
    ~modeBuses = Array.fill(numDevices, { Bus.control(s) });
    ~modeNdefs = Array.fill(numDevices, { |i|
        Ndef("modePlay_%".format(i).asSymbol, { arg mode = 0; Out.kr(~modeBuses[i], mode) });
    });

    // Setup Mapping for each device
    numDevices.do { |i|
        {
            // Map to 'Modeselektor_1', 'Modeselektor_2', etc.
            MapOut.kr(In.kr(~modeBuses[i]), "Modeselektor_%".format(i+1), 0, 9);
        }.play;
    };

    // GUI Stuff
    defer {
        win = Window("TorqueTuner Multi-Controller", Rect(100, 100, 600, 300));
        layout = HLayout(); // Arrange devices horizontally

        numDevices.do { |i|
            var deviceLayout = VLayout();
            deviceLayout.add(StaticText().string_("Device %".format(i+1)).align_(\center));

            modeNames.do { |name, mIdx|
                deviceLayout.add(
                    Button()
                    .states_([[name]])
                    .action_({
                        ~modeNdefs[i].set(\mode, modeValues[mIdx]);
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

	~freq = Bus.control;
	{
		Out.kr(~freq, MapIn.kr('Frequency', 20, 20000));
	}.play;

	~trigger = Bus.control;
	{
		Out.kr(~trigger, MapIn.kr('Excitation', 0, 1));
	}.play;

	// String-plucker synth
	Ndef(\modePlay).set(\mode,0);
	Ndef(\string,{
		var rawTrigger = In.kr(~trigger);
		var trig = Changed.kr(rawTrigger);

		// Impulse.kr(0) to sample frequency on startup
		var initTrig = Impulse.kr(0) + trig;
		var rawFreq = In.kr(~freq);

		// Sample the frequency exactly when the string is plucked
		var latchedFreq = Latch.kr(rawFreq, initTrig);

		// Define the deadband in Hz
		var deadbandHz = 1000;

		// Calculate distance from the locked/latched frequency
		var freqDiff = abs(rawFreq - latchedFreq);
		var inDeadband = freqDiff < deadbandHz;

		// If inside the deadband lock to latchedFreq
		// If outside the deadband track rawFreq continuously.
		var finalFreq = Select.kr(inDeadband, [rawFreq, latchedFreq]);

		// Calculate delay time using the stabilized frequency
		var delayTime = finalFreq.lincurve(20, 20000, 0.0005, 0.05, 8);

		var sig = Pluck.ar(WhiteNoise.ar(0.1), trig, 0.1, delayTime, 15, 0.2).dup;
		Out.ar(0, sig);
	}).play;
});
)