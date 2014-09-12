{
  'variables': {
    '__GCC_VERSION%': '<!(gcc -dumpversion)',
    'conditions': [
      ['building_for_tizen==1', {
        'clang': 0,
      }],
    ],
  },
  'target_defaults': {
    'defines': [
      'COMPONENT_BUILD=1',
      'SKIA_DLL=1',
      'TIZEN_ENGINE_SUPPORT=1',
      'TIZEN_EDGE_EFFECT=1',
      'BUILDING_V8_SHARED=1',
      'V8_SHARED=1',
      'USE_DL_PREFIX=1',
# [M37] TODO:Autofill related code chagned. Need to enable after Fixing.
      #'TIZEN_AUTOFILL_SUPPORT=1',
    ],
    # use_aura is enabled but clipboard_efl.cc implements clipboard.
    'sources/': [
      ['exclude', 'browser/renderer_host/native_web_keyboard_event_aura\\.cc$'],
      ['exclude', 'browser/renderer_host/render_widget_host_view_aura\\.cc$'],
      ['exclude', 'browser/web_contents/web_contents_view_aura\\.cc$'],
      ['exclude', 'clipboard/clipboard_aurax11\\.cc$'],
      ['exclude', 'gl_context_egl\\.cc$'],
      ['exclude', 'gl_context_glx\\.cc$'],
      ['exclude', 'x11_event_source_glib\\.cc$'],
      ['exclude', 'x11_event_source_libevent\\.cc$'],
      ['exclude', 'x11_types\\.cc$'],
    ],
    'conditions': [
       ['building_for_tizen==1', {
         'defines': [
         'OS_TIZEN=1',
         # For GCC 4.5.3 bundled arm.h has a bug and don't define __ARM_PCS when it should.
         # Force define this flag for the whole chromium on gbs gcc 4.5.3.
         # Non-arm builds will ingore it in any case.
         '__ARM_PCS',
         ],
         'sources/': [
           ['exclude', 'browser/device_sensors/data_fetcher_shared_memory_default.cc$'],
         ],
      }, {
        'sources/': [
          ['exclude', 'browser/device_sensors/data_fetcher_impl_tizen\\.(cc|h)$'],
          ['exclude', 'browser/device_sensors/data_fetcher_shared_memory_tizen\\.cc$'],
          ['exclude', 'browser/geolocation/location_provider_efl\\.(cc|h)$'],
          ['exclude', 'browser/motion/MotionUI\\.(h|cc)$'],
          ['exclude', 'browser/motion/wkext_motion\\.(h|cc)$'],
#          ['exclude', 'browser/media/tizen/webaudio_decoder_browser_tizen\\.(h|cc)$'],
  #        ['exclude', 'renderer/media/tizen/audio_decoder_tizen\\.(h|cc)$'],
        ],
      }],
      ['building_for_tizen==1 and building_for_tizen_mobile==1', {
         'defines': [
           'OS_TIZEN_MOBILE=1',
           'TIZEN_CONTENTS_DETECTION=1',
           'TIZEN_ENABLE_CAPI_SUPPORT=1',
           ],
      }],
      ['building_for_tizen==1 and building_for_tizen_tv==1', {
       'defines': ['OS_TIZEN_TV=1'],
      }],
      ['host_arch=="arm"', {
        'target_conditions': [
          ['_toolset=="host"', {
            'cflags!': [ '-m32' ],
            'ldflags!': [ '-m32' ],
            'asflags!': [ '-32' ],
          }],
        ],
      }],
    ],
  },
}
