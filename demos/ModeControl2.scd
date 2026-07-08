(
/*
////////////////////////////////////// Haptic effects Demo for TorqueTuner //////////////////////////////////////
This demo has no sound synthesis, and is simply and interface for demonstrating
the haptic effects onboard TorqueTuner.
To run it do:
1. Enable libmapper by placing cursor below this comment and press CMD+return.
2. Map Modeselektor -> Mode
3. Try running the different effects by placing the curser on the correspondiing line and press press CMD+return
4. Stop the demo by pressing CMD+.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/


// Start libmapper
s.waitForBoot({
	Mapper.enable;
	10.wait;
	////////////// Output signals  //////////////
	~mode = Bus.control;
	{
		MapOut.kr(In.kr(~mode), 'Modeselektor', 0, 9);
	}.play;

	Ndef(\modePlay, { arg mode = 0;
		Out.kr(~mode, mode);
	}).play;

	~ctrl = Bus.control;
	{
		Out.kr(~ctrl, MapIn.kr('Control', -1, 1));
	}.play;

});
)

(
var win, layout, buttons;
var modeNames = [
	"Click", "Magnet", "Wall", "Linspring",
	"Expspring", "Free", "Spin"
];
var modeValues = [0, 1, 2, 4, 5, 6, 7];

win = Window("TorqueTuner Controller", Rect(100, 100, 300, 400));
layout = VLayout();

win.layout = layout;

buttons = modeNames.collect { |name, i|
	Button()
	.states_([[name]])
	.action_({
		Ndef(\modePlay).set(\mode, modeValues[i]);
		"Setting mode to: %".format(name).postln;
	});
};

buttons.do { |btn| layout.add(btn) };

win.front;
win.onClose = { "GUI Closed".postln; };
)

////////////// Haptic Effects  //////////////

// Click
Ndef(\modePlay).set(\mode,0); // Place cursor here and press CMD+return

// Magnet
Ndef(\modePlay).set(\mode,1); // Place cursor here and press CMD+return

// Wall
Ndef(\modePlay).set(\mode,2); // Place cursor here and press CMD+return

// Linspring
Ndef(\modePlay).set(\mode,4); // Place cursor here and press CMD+return

// Expspring
Ndef(\modePlay).set(\mode,5); // Place cursor here and press CMD+return

// Free
Ndef(\modePlay).set(\mode,6); // Place cursor here and press CMD+return

// Spin
Ndef(\modePlay).set(\mode,7); // Place cursor here and press CMD+return

// Anti-Cogging
Ndef(\modePlay).set(\mode,9); // Place cursor here and press CMD+return
(
/*
//////////////////////////////////////   Creating a synth   //////////////////////////////////////
Try mapping angle -> Control and create a synth that uses TorqueTuner position for something cool.
Re-evaluate the block (CMD+enter below this comment) to listen to your changes
*/

	Ndef(\my_synth,{
		var input = In.kr(~ctrl);
		var sound = SinOsc.ar(220);

		// Do something with sound and input  here ...

		Out.ar(0,sound.dup*0.5);
	}).play;
)