## Introduction

chromium-efl is a Chromium/Blink engine port to tizen/efl platform. The port
implements Chromium/Blink platform APIs.

It also exposes a webview API implementation based on chromium-efl port. It is
supposed to be completely source and binary compatible with EFL-WebKit2.

## Details

1. gclient pulls chromium-efl into "src/tizen_src".
2. The it runs 2 hooks in order to get the rest of the source:

```
* generate-gclient-xwalk: .gclient-xwalk is created by running
    src/tizen_src/scripts/xwalkgenerate_gclient-xwalk.py (this is a fork
    of the same script in xwalk repository).
* fetch-deps: part of the xwalk solution in .gclient, gets called from within
    xwalk pull procedure. It actually fetches all depedencies based on .gclient-xwalk.
```

## Procedure

1. create .gclient file (as below)

```
solutions = [
  { "name"        : "src/tizen_src",
    "url"         : "https://github.com/crosswalk-project/chromium-efl.git@efl/crosswalk-10/39.0.2171.19",
  },
]
```

2. gclient sync

## Building

* For Desktop build

    $ ./build/build_desktop.sh [-h|--help] [--skip-gyp] [--skip-ninja] [--ccache] [--debug]

## Coding style

Internally we use the chromium coding style: http://www.chromium.org/developers/coding-style.
For public headers we follow efl style.

## License

Chromium-efl's code uses the BSD license, see our `LICENSE` file.
