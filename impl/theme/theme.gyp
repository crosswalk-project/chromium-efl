{
  'targets': [{
    'target_name': 'tizen_theme',
    'type': 'static_library',

    'includes': [
      # NOTE: gyp includes need to be relative
      '../../../build/common.gypi',
      '../../../third_party/WebKit/Source/build/features.gypi',
      '../../../skia/skia_common.gypi',
    ],
    'dependencies': [
      '<(chrome_src_dir)/third_party/WebKit/Source/core/core.gyp:webcore',
    ],
    'include_dirs': [
      '.',
      '<(chrome_src_dir)',
      '<(chrome_src_dir)/gpu',
      '<(chrome_src_dir)/third_party/WebKit',
      '<(chrome_src_dir)/third_party/WebKit/Source',
      '<(chrome_src_dir)/third_party/skia/include/core',
      '<(chrome_src_dir)/third_party/skia/include/utils',
      '<(chrome_src_dir)/v8/include',
      '<(SHARED_INTERMEDIATE_DIR)',
      '<(SHARED_INTERMEDIATE_DIR)/webkit/',
      '<(SHARED_INTERMEDIATE_DIR)/blink',
    ],
    'defines': [
      'INSIDE_BLINK=1',
      '<@(feature_defines)',
    ],
    'sources': [
      'RenderThemeChromiumDefault_override.cpp',
      'RenderThemeChromiumTizen.cpp',
      'RenderThemeChromiumTizen.h',
      '<(SHARED_INTERMEDIATE_DIR)/TizenUserAgentStyleSheetsData.cpp',
    ],
    'actions': [
      {
        'action_name': 'TizenUserAgentStyleSheets',
        'variables': {
          'scripts': [
            '<(chrome_src_dir)/third_party/WebKit/Source/build/scripts/make-file-arrays.py',
          ],
          'stylesheets': [ 'themeChromiumTizen.css', ],
        },
        'inputs': [
          '<@(scripts)',
          '<@(stylesheets)'
        ],
        'outputs': [
          '<(SHARED_INTERMEDIATE_DIR)/TizenUserAgentStyleSheets.h',
          '<(SHARED_INTERMEDIATE_DIR)/TizenUserAgentStyleSheetsData.cpp',
        ],
        'action': [
          'python',
          '<@(scripts)',
          '--namespace',
          'blink',
          '--out-h=<(SHARED_INTERMEDIATE_DIR)/TizenUserAgentStyleSheets.h',
          '--out-cpp=<(SHARED_INTERMEDIATE_DIR)/TizenUserAgentStyleSheetsData.cpp',
          '<@(stylesheets)',
        ],
      },
    ], # actions
  }],
}
