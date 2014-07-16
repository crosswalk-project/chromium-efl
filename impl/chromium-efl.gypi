{
  'variables': {
    '__GCC_VERSION%': '<!(gcc -dumpversion)',
    'conditions': [
      ['sysroot!=""', {
        'pkg-config': '<(chrome_src_dir)/build/linux/pkg-config-wrapper <(sysroot) <(target_arch)',
      }, {
        'pkg-config': 'pkg-config'
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
      'ENABLE_ORIENTATION_EVENTS=1'
    ],
    'cflags': [
      '<!@(<(pkg-config) --cflags ecore)',
      '<!@(<(pkg-config) --cflags ecore-x)',
      '<!@(<(pkg-config) --cflags ecore-evas)',
      '<!@(<(pkg-config) --cflags ecore-input)',
      '<!@(<(pkg-config) --cflags ecore-imf-evas)',
      '<!@(<(pkg-config) --cflags evas)',
      '<!@(<(pkg-config) --cflags elementary)',
      '<!@(<(pkg-config) --cflags glib-2.0)',
      '<!@(<(pkg-config) --cflags gstreamer-0.10)',
      '<!@(<(pkg-config) --cflags gstreamer-base-0.10)',
      '<!@(<(pkg-config) --cflags gstreamer-app-0.10)',
      '<!@(<(pkg-config) --cflags gstreamer-pbutils-0.10)',
    ],
    'link_settings': {
      'ldflags': [
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore-x)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore-evas)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore-input)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore-imf-evas)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other evas)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other elementary)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other glib-2.0)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other gstreamer-0.10)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other gstreamer-base-0.10)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other gstreamer-app-0.10)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other gstreamer-pbutils-0.10)',
      ],
      'libraries': [
        '<!@(<(pkg-config) --libs-only-l ecore)',
        '<!@(<(pkg-config) --libs-only-l ecore-x)',
        '<!@(<(pkg-config) --libs-only-l ecore-evas)',
        '<!@(<(pkg-config) --libs-only-l ecore-input)',
        '<!@(<(pkg-config) --libs-only-l ecore-imf-evas)',
        '<!@(<(pkg-config) --libs-only-l evas)',
        '<!@(<(pkg-config) --libs-only-l elementary)',
        '<!@(<(pkg-config) --libs-only-l glib-2.0)',
        '<!@(<(pkg-config) --libs-only-l gstreamer-0.10)',
        '<!@(<(pkg-config) --libs-only-l gstreamer-base-0.10)',
        '<!@(<(pkg-config) --libs-only-l gstreamer-app-0.10)',
        '<!@(<(pkg-config) --libs-only-l gstreamer-pbutils-0.10)',
      ],
    },
    # use_aura is enabled but clipboard_efl.cc implements clipboard.
    'sources/': [
      ['exclude', 'browser/renderer_host/native_web_keyboard_event_aura\\.cc$'],
      ['exclude', 'browser/renderer_host/render_widget_host_view_aura\\.cc$'],
      ['exclude', 'browser/web_contents/web_contents_view_aura\\.cc$'],
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
         'cflags': [
            '<!@(<(pkg-config) --cflags capi-system-info)',
            '<!@(<(pkg-config) --cflags capi-system-sensor)',
            '<!@(<(pkg-config) --cflags capi-location-manager)',
            '<!@(<(pkg-config) --cflags capi-media-camera)',
            '<!@(<(pkg-config) --cflags vconf)',
            '<!@(<(pkg-config) --cflags libtbm)',
            '<!@(<(pkg-config) --cflags libdri2)',
            '<!@(<(pkg-config) --cflags gles20)',
         ],
         'link_settings': {
           'ldflags': [
             '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-system-info)',
             '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-system-sensor)',
             '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-location-manager)',
             '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-media-camera)',
             '<!@(<(pkg-config) --libs-only-L --libs-only-other vconf)',
           ],
           'libraries': [
             '<!@(<(pkg-config) --libs-only-l capi-system-info)',
             '<!@(<(pkg-config) --libs-only-l capi-system-sensor)',
             '<!@(<(pkg-config) --libs-only-l capi-location-manager)',
             '<!@(<(pkg-config) --libs-only-l capi-media-camera)',
             '<!@(<(pkg-config) --libs-only-l vconf)',
             '<!@(<(pkg-config) --libs-only-l libtbm)',
             '<!@(<(pkg-config) --libs-only-l libdri2)',
             '<!@(<(pkg-config) --libs-only-l gles20)',
           ],
         },
      }, {
        'sources/': [
          ['exclude', 'browser/device_sensors/data_fetcher_impl_tizen\\.(cc|h)$'],
          ['exclude', 'browser/device_sensors/data_fetcher_shared_memory_tizen\\.cc$'],
          ['exclude', 'browser/geolocation/location_provider_efl\\.(cc|h)$'],
          ['exclude', 'browser/motion/MotionUI\\.(h|cc)$'],
          ['exclude', 'browser/motion/wkext_motion\\.(h|cc)$'],
        ],
      }],
      ['building_for_tizen_mobile==1', {
         'defines': ['OS_TIZEN_MOBILE=1'],
      }],
      ['building_for_tizen_tv==1', {
       'defines': ['OS_TIZEN_TV=1'],
      }],
    ],
  },
}
