# Airwindows plugins (LV2 port)

*This is an LV2 port of the Airwindows plugins originally created by Chris Johnson.*

Have a look at https://www.airwindows.com for a detailed description of all the plugins. Find the original source code at https://github.com/airwindows/airwindows.

Are you missing your favorite Airwindows plugin? Tell me by sending an email to the [airwindows-lv2-devel mailing list], so the port of that plugin can be prioritized. You can also open an issue directly by sending an email to [~hannes/airwindows-lv2@todo.sr.ht](mailto:~hannes/airwindows-lv2@todo.sr.ht).

## Important Note
This repository is only getting mirrored to GitHub. Since April 2023, the "main" hosting location is https://sr.ht/~hannes/airwindows-lv2.
I'll still keep the GitHub mirror up-to-date for some time and you can still open issues and pull requests on GitHub, if you prefer that.
If you'd like to do me a favor though, submit the issue/patch to the [airwindows-lv2-devel mailing list] on SourceHut. Thanks!

More information about the reasoning of this change:
- https://drewdevault.com/2022/03/29/free-software-free-infrastructure.html
- https://giveupgithub.org/

## Installation

There are different methods for installing these plugins. Some of them are listed in the following.

### Pre-built versions

Download the latest pre-built version from [SourceHut](https://git.sr.ht/~hannes/airwindows-lv2/refs). Click on the latest version (e.g. v18.0) and you
should find the downloads for macOS and Windows.

These plugins are bundeled within a single directory called `Airwindows.lv2`. Just copy or move this directory to the appropriate path for your system:

- Linux: `/usr/local/lib/lv2`
- macOS: `/Library/Audio/Plug-Ins/LV2`
- Windows: `C:\Program Files\Common Files\LV2`

If you're using Linux, please consider packaging this software, so you can install it through your system's package manager.
If you've got problems doing this, let me know on the [airwindows-lv2-devel mailing list] and I'll try to help you packaging.
In the following Linux distributions, airwindows-lv2 is already available through your package manager:

- openSUSE Tumbleweed (as `lv2-airwindows`)
- NixOS

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

If you have problems with building this project, have a look at the [CI build instructions](.build.yml). Those might be helpful.

## Contributing

See [CONTRIBUTING](CONTRIBUTING.md).

In case of any questions, problems or bugs, also send a message to the [airwindows-lv2-devel mailing list].

## License

This project is licensed under the [MIT License](LICENSE).

Copyright (c) 2018-2023 Chris Johnson

Copyright (c) 2022-2023 Hannes Braun

[airwindows-lv2-devel mailing list]: mailto:~hannes/airwindows-lv2-devel@lists.sr.ht
