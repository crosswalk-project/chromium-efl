## Introduction

chromium-efl is a Chromium/Blink engine port to Tizen/EFL platform. It implements Chromium/Blink platform APIs, and exposes an embedding layer that is source and binary compatible with EFL-WebKit2.

## Fetch the source

* gclient pulls chromium-efl into "src/tizen_src".
* Then it runs 2 hooks in order to get the rest of the source:

```
* generate-gclient-xwalk: .gclient-xwalk is created by running
    src/tizen_src/scripts/xwalkgenerate_gclient-xwalk.py (this is a fork
    of the same script in xwalk repository).
* fetch-deps: part of the xwalk solution in .gclient, gets called from within
    xwalk pull procedure. It actually fetches all depedencies based on .gclient-xwalk.
```

1) Create a source directory

```
$ mkdir xwalk-efl
$ cd xwalk-efl
```

2) Auto-generate gclient's configuration file (.gclient):

```
gclient config --name=src/tizen_src \
    https://github.com/crosswalk-project/chromium-efl.git@efl/crosswalk-10/39.0.2171.19
```

3) gclient sync

## Building

* For Desktop build

```
$ cd src/tizen_src/
$ ./build/build_desktop.sh [-h|--help] [--skip-gyp] [--skip-ninja] [--ccache] [--debug]
```
## Coding style

Internally we use the chromium coding style: http://www.chromium.org/developers/coding-style.
For public headers we follow efl style.

## License

Chromium-efl's code uses the BSD license, see our `LICENSE` file.
