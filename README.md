## Introduction

chromium-efl is a Chromium/Blink engine port to tizen/efl platform. The port
implements Chromium/Blink platform APIs.

It also exposes a webview API implementation based on chromium-efl port. It is
supposed to be completely source and binary compatible with EFL-WebKit2.

## Details

1. gclient pulls chromium-efl into "src/tizen_src".
2. The it runs 3 custom hooks in order to get the rest of the source:

```
* fetch-src-blink: src/tizen_src/sync_repos.sh is called and
    pulls in src and src/third_party/WebKit from a pre-defined locations.
* generate-gclient-xwalk: .gclient-xwalk is created by running
    src/tizen_src/scripts/xwalkgenerate_gclient-xwalk.py (this is a fork
    of the same script in xwalk repository).
* fetch-deps: actually fetches all depedencies in .gclient-xwalk.
```

## Procedure

1. create .gclient file (as below)

```
solutions = [
  { "name"        : "src/tizen_src",
    "url"         : "git@github.com:crosswalk-project/chromium-efl.git@efl/crosswalk-10/39.0.2171.19",
    "managed"     : True,
    "custom_hooks": [
      {
        # fetch custom src and blink repositories.
        "name": "fetch-src-blink",
        "pattern": ".",
        "action": ["src/tizen_src/sync_repos.sh"],
      },
      {
        # Generate .gclient-xwalk for Crosswalk's dependencies.
        "name": "generate-gclient-xwalk",
        "pattern": ".",
        "action": ["python", "src/tizen_src/scripts/xwalk/generate_gclient-xwalk.py"],
      },
      {
        # Fetch Crosswalk dependencies.
        "name": "fetch-deps",
        "pattern": ".",
        "action": ["python", "src/xwalk/tools/fetch_deps.py", "-v"],
      },
      {
        # At some point, we will integrate to gyp_xwalk. Not now...
        "name": "gyp-xwalk",
      }
    ],
    "safesync_url": "",
  },
]
cache_dir = None
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
