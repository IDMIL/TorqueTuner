(
s.waitForBoot({
    var numDevices = 3;
    var modeNames = ["Click", "Wall", "Linspring"];
    var modeValues = [0, 2, 4];

    Mapper.enable;
    10.wait;

    ~modeBuses = Array.fill(numDevices, { Bus.control(s) });

    numDevices.do { |i|
        { MapOut.kr(In.kr(~modeBuses[i]), "Modeselektor_%".format(i+1), 0, 9); }.play;
    };

	// Independent mode control for each device
    ~modeNdefs = Array.fill(numDevices, { |i|
        Ndef("modePlay_%".format(i).asSymbol, { arg mode = 4; Out.kr(~modeBuses[i], mode) });
    });

    // Input Signals
    ~cutoff = Bus.control(s, 1);
    ~resonance = Bus.control(s, 1);
    ~freq = Bus.control(s, 1);

    { Out.kr(~cutoff, MapIn.kr('FilterCutoff', 0, 1)); }.play;
    { Out.kr(~resonance, MapIn.kr('Resonance', 0, 1)); }.play;
    { Out.kr(~freq, MapIn.kr('Frequency', 20, 1000)); }.play;

    // Filter Synth
    Ndef(\filter, {
        var ctrl_in = In.kr(~cutoff);
        var baseFreq = In.kr(~freq);
        var res = In.kr(~resonance);

        var sig = BLowPass4.ar(LFSaw.ar(baseFreq * [0.995, 1.005]),
            ctrl_in.linlin(0,1,20,3000)**[0.99,1.01],
            res.lincurve(0,1,1,0.02,-4));

        var sig2 = BLowPass4.ar(LFSaw.ar(baseFreq * 2 * [0.995, 1.005], 0.2),
            ctrl_in.linlin(0,1,100,5000)**[0.99,1.01],
            res.lincurve(0,1,1,0.02,-4));

        Out.ar(0,(sig + (ctrl_in.lincurve(0,1,0,1,4)*sig2))/2);
    }).play;

    // GUI Stuff
    defer {
        var win = Window("TorqueTuner Multi-Controller", Rect(100, 100, 600, 300));
        var layout = HLayout();

        numDevices.do { |i|
            var devLayout = VLayout();
            devLayout.add(StaticText().string_("Device %".format(i+1)).align_(\center));
            modeNames.do { |name, mIdx|
                devLayout.add(Button().states_([[name]]).action_({
                    ~modeNdefs[i].set(\mode, modeValues[mIdx]);
                }));
            };
            layout.add(devLayout);
        };
        win.layout = layout;
        win.front;
    };
});
)