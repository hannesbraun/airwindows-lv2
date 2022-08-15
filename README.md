# Airwindows plugins (LV2 port)

*This is an LV2 port of the Airwindows plugins made by Chris Johnson.*

Have a look at https://www.airwindows.com for a detailed description of all the plugins. Find the original source code at https://github.com/airwindows/airwindows.

## Building

Meson, LV2 and a C compiler are required to build this project. Setup the build directory as follows:

```bash
meson setup build

# Alternative: use a custom LV2 directoy
# For installing the plugins into the user library on macOS:
meson setup build -Dlv2dir=/Users/<username>/Library/Audio/Plug-Ins/LV2
```

Compile and install the plugins:
```bash
meson compile -C build
meson install -C build
```

## License

This project is licensed under the [MIT License](LICENSE).

Copyright (c) 2018-2022 Chris Johnson

Copyright (c) 2022 Hannes Braun
