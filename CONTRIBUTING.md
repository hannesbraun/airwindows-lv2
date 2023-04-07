# Contributing to airwindows-lv2

You want to help? Cool! This should help you get started with porting a plugin. If you're unsure about some things, have a look at the plugins that were already ported. That might already help in some cases. Otherwise, just ask me (Hannes).

This project uses the macOS VST plugins from the original distribution as a reference. The reason for this is simply that I am using the VST versions on a Mac, so I chose these to be the reference. Every other version should usually work too. But in the case where there are any differences, the LV2 plugins are consistent with the corresponding Mac VST plugins (instead of having a "wild" mixture of behaviours). Additionally, you don't have to guess where to look in the original source code for the origin of the ported code.

Also, these plugins are written in C, not C++. As most parts of these plugins are just basic arithmetic, there's no reason (in my opinion) to use C++.

## Preparation

Find the plugin to port in the `MacVST` directory within the [Airwindows repository](https://github.com/airwindows/airwindows).
The three files inside the `source` directory are of interest.

First off, copy the Template into the source directory and rename the directory to the plugin name.
Inside, rename both `Template.c` and `Template.ttl` in the same way.

## Writing the Turtle file

Within the Turtle file:
- Replace `template` in the URI with a lowercase version of the plugin name.
- Replace `lv2:TemplatePlugin` with a proper plugin category.
- Replace the plugin name (`doap:name`).
- Replace the plugin description (`rdfs:comment`) with the corresponding description from the `what.txt` that is bundled with the binary releases. The "<PluginName> is" part is usually omitted. Have a look at the plugins that were already ported to see some examples.
- Adjust the control ports of the plugin. There is already one control port template at the end of the file.
	- The symbol should be a camel case version of the port name. Also, remove any spaces and special characters.
	- `lv2:default`, `lv2:minimum` and `lv2:maximum` should be quite self-explanatory. These are the values being displayed. In contrast to the VST version, those values are being passed into the processing of the plugin directly. For values not ranging from 0 to 1, you probably need to adjust the according part in the `run` function.
	- Also, parameters whose sliders don't map linearly to their values due to extra code added (usually in `<PluginName>.cpp`) should be adjusted to have a linear mapping. (If there will be an option built into LV2 to do this properly, this might be an option for the future.) An example of this can be found in Compresaturator (see the parameter "Expand"). Check out the difference between the VST and the LV2 version.
	- Values ranging from 0 to 1 usually don't have a unit. Otherwise, choose the appropriate unit. If it is not available, create it yourself. There is an example of this for the unit "ips" in [IronOxideClassic2](src/IronOxideClassic2/IronOxideClassic2.ttl).
- Finally, remove all the unused prefixes.

## Writing the C file

Next, move on to the C file.
- Change the `TEMPLATE_URI`. `TEMPLATE_URI` becomes something like `CONSOLE7CHANNEL_URI`. Also, update the URI to what you defined in the Turtle file. Make sure to update its usage in the `LV2_Descriptor` too.
- Rename the `Template` struct to `<PluginName>`. Rename the variable names to `<pluginName>`.
- Check if the plugin requires knowing the sample rate. If not, remove it from the plugin struct.
- Adjust the `PortIndex` enumeration according to the input ports of the plugin. Also, add these parameters to the plugin struct.
- Map the ports in the `connect_port` function.
- Copy the state variables from the original `<PluginName>.h` to the plugin struct. There are some VST specific variables in there that don't need to be copied. Usually, these are:
```c++
char _programName[kVstMaxProgNameLen + 1];
std::set< std::string > _canDo;
```
- Also, remember that you don't need to copy the variables named `A`, `B` and so on, as you should have already added those (with a better name that corresponds to their port name).
- Copy the relevant part of the original constructor in the file `<PluginName>.cpp` to the activate function.
- Copy the code in the method `processReplacing` (file: `<PluginName>Proc.cpp`). The basic outline is already present in the template. Leave that as it is and copy the two blocks of code to their appropriate place.
- Prepend `<pluginName>->` to the variables that are contained in the plugin struct.
- Adjust the part of the code that retrieves the port/parameter values. You need to replace `A` (...) with the dereferenced value of the corresponding parameter.
- Port all other C++ constructs to C. Usually, this is nothing more than a few casts.

## Getting the plugin ready for testing

The main work should be done now. Add the plugin to the build script (`meson.build`) and add an entry in `src/manifest.ttl.in`. Also, format the C source code using `clang-format`:

```bash
clang-format -i --style=file src/<PluginName>/<PluginName>.c
```

If you're done, validate that the plugin works. To validate the syntactical structure of the turtle files, run `validate.sh`. Then build the project and install the plugins. You can fire up a first test with `lv2bench`.

## Null test

Now, it's time for a null test.

- Open up your DAW, load in some audio and send it to two channels; one has the VST version of the plugin loaded, the other one has the LV2 version loaded and inverts the signal afterwards.
- Try adjusting the different parameters of the plugins and check if it nulls. Also, try adjusting the gain on the source track.
- Usually, there is always going to be some leftover dither, peaking at around -140 dB. This is fine.
- Things like compressors may drive you crazy here... Try to come up with a configuration, save the project, and reopen your DAW. Now, both plugins should be running "in sync" as they started operating at the same time.
- If you still have a signal poking through, try testing against the AU version (assuming you're on a Mac). Sometimes, the VST version behaves a bit differently from the AU version (at least in REAPER).
- Also, make sure that the plugin's parameters have the same order as they had in the original version.
- If the plugin makes use of units, don't forget to test them too. Sadly, REAPER doesn't display units. Ardour can help out with this.

## Submit the plugin

If the null test was successful, finally add an entry to the Changelog. Submit the patch, and I'll review it as soon as I've got time. Limit a commit to one plugin port to make the reviewing process easier. For a tutorial on how to send patches by email, visit https://git-send-email.io.

Thanks.
