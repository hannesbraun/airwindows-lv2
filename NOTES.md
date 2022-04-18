This is a collection of notes for some plugins.

### Pressure5

The Audio Unit and the LV2 version sound identical. There is a difference to the VST version. When starting playback with a rather loud input signal, the initial loud "blip" is shorter for the VST version. I don't know why. This ignored for now. Maybe, I'll find out the reason for that in the future. (Tested on macOS)

### SubsOnly

The initialization of `fpdL` and `fpdR` is missing in the original VST source code. I'll add it here too, if it get's fixed in the original code.
