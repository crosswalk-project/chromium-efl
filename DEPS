include_rules = [
]

vars = {
}

deps = {
  "src/xwalk": "https://github.com/crosswalk-project/crosswalk-efl.git@efl/crosswalk-11/11.40.277.3",
}

hooks = [
  {
    # Generate .gclient-xwalk for Crosswalk's dependencies, including
    # custom chromium and WebKit repositories.
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
]
