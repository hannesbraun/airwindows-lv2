# Airwindows plugins (LV2 port)

*This is an LV2 port of the Airwindows plugins made by Chris Johnson.*

Have a look at https://www.airwindows.com for a detailed description of all the plugins. Find the original source code at https://github.com/airwindows/airwindows.

Are you missing your favorite Airwindows plugin? [Open an issue](https://github.com/hannesbraun/airwindows-lv2/issues/new), so the port of that plugin can be prioritized.

## Installation

There are different methods for installing these plugins. Some of them are listed in the following.

### Pre-built versions

Download the latest pre-built version from [GitHub Releases](https://github.com/hannesbraun/airwindows-lv2/releases).
These plugins are bundeled within a single directory called `Airwindows.lv2`. Just copy or move this directory to the appropriate path for your system:

- Linux: `/usr/local/lib/lv2`
- macOS: `/Library/Audio/Plug-Ins/LV2`
- Windows: `C:\Program Files\Common Files\LV2`

### openSUSE Build Service

If you are using openSUSE, you can use the [openSUSE Build Service](https://build.opensuse.org).
Add the repository for the [`multimedia:proaudio`](https://build.opensuse.org/project/show/multimedia:proaudio) project.
Inside, you can find the package [`lv2-airwindows`](https://build.opensuse.org/package/show/multimedia:proaudio/lv2-airwindows) containing the plugins.
You can find more detailed installation instructions here: https://software.opensuse.org//download.html?project=multimedia%3Aproaudio&package=lv2-airwindows.

### Building the plugins yourself

You can also build and install the plugins yourself as described in [Building](#building).

## Building

Meson, LV2 and a C compiler are required to build this project. Setup the build directory as follows:

```bash
meson setup build

# Alternative: use a custom LV2 plugin directoy
# For installing the plugins into the user library on macOS:
meson setup build -Dlv2dir=/Users/<username>/Library/Audio/Plug-Ins/LV2
```

Compile and install the plugins:
```bash
meson compile -C build
meson install -C build
```

If you have problems with building this project, have a look at the [build instructions for GitHub Actions](.github/workflows/build.yml). Those might be helpful.

## Contributing

See [CONTRIBUTING](CONTRIBUTING.md).

## License

This project is licensed under the [MIT License](LICENSE).

Copyright (c) 2018-2022 Chris Johnson

Copyright (c) 2022 Hannes Braun
