{
  'variables': {
    # We don't actually need to custumize pkg-config but keep having it in a
    # variable just in case so if we have to we can override with a oneliner.
    'pkg-config': 'pkg-config',
  },
  'targets': [
  {
    'target_name': 'efl',
    'type': 'none',
    'direct_dependent_settings': {
      'cflags': [
        '<!@(<(pkg-config) --cflags ecore)',
        '<!@(<(pkg-config) --cflags ecore-x)',
        '<!@(<(pkg-config) --cflags ecore-evas)',
        '<!@(<(pkg-config) --cflags ecore-input)',
        '<!@(<(pkg-config) --cflags ecore-imf-evas)',
        '<!@(<(pkg-config) --cflags evas)',
        '<!@(<(pkg-config) --cflags elementary)',
      ],
    },
    'link_settings': {
      'ldflags': [
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore-x)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore-evas)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore-input)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other ecore-imf-evas)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other evas)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other elementary)',
      ],
      'libraries': [
        '<!@(<(pkg-config) --libs-only-l ecore)',
        '<!@(<(pkg-config) --libs-only-l ecore-x)',
        '<!@(<(pkg-config) --libs-only-l ecore-evas)',
        '<!@(<(pkg-config) --libs-only-l ecore-input)',
        '<!@(<(pkg-config) --libs-only-l ecore-imf-evas)',
        '<!@(<(pkg-config) --libs-only-l evas)',
        '<!@(<(pkg-config) --libs-only-l elementary)',
      ],
    },
  },
  {
    'target_name': 'gstreamer',
    'type': 'none',
    'direct_dependent_settings': {
      'cflags': [
        '<!@(<(pkg-config) --cflags glib-2.0)',
        '<!@(<(pkg-config) --cflags gstreamer-0.10)',
        '<!@(<(pkg-config) --cflags gstreamer-base-0.10)',
        '<!@(<(pkg-config) --cflags gstreamer-app-0.10)',
        '<!@(<(pkg-config) --cflags gstreamer-pbutils-0.10)',
      ],
    },
    'link_settings': {
      'ldflags': [
        '<!@(<(pkg-config) --libs-only-L --libs-only-other glib-2.0)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other gstreamer-0.10)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other gstreamer-base-0.10)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other gstreamer-app-0.10)',
        '<!@(<(pkg-config) --libs-only-L --libs-only-other gstreamer-pbutils-0.10)',
      ],
      'libraries': [
        '<!@(<(pkg-config) --libs-only-l glib-2.0)',
        '<!@(<(pkg-config) --libs-only-l gstreamer-0.10)',
        '<!@(<(pkg-config) --libs-only-l gstreamer-base-0.10)',
        '<!@(<(pkg-config) --libs-only-l gstreamer-app-0.10)',
        '<!@(<(pkg-config) --libs-only-l gstreamer-pbutils-0.10)',
      ],
    },
  },
  ], # targets

  # Only add tizen target on tizen otherwise gyp fails with pkg-config error.
  'conditions': [
    ['building_for_tizen==1', {
      'targets': [
      {
        'target_name': 'tizen',
        'type': 'none',
        'direct_dependent_settings': {
          'cflags': [
            '<!@(<(pkg-config) --cflags capi-appfw-application)',
            '<!@(<(pkg-config) --cflags capi-system-info)',
            '<!@(<(pkg-config) --cflags capi-system-sensor)',
            '<!@(<(pkg-config) --cflags capi-system-device)',
            '<!@(<(pkg-config) --cflags capi-location-manager)',
            '<!@(<(pkg-config) --cflags capi-media-audio-io)',
            '<!@(<(pkg-config) --cflags capi-media-camera)',
            '<!@(<(pkg-config) --cflags vconf)',
            '<!@(<(pkg-config) --cflags libtbm)',
            '<!@(<(pkg-config) --cflags libdri2)',
            '<!@(<(pkg-config) --cflags gles20)',
            '<!@(<(pkg-config) --libs-only-l ui-gadget-1)',
          ],
        },
        'link_settings': {
          'ldflags': [
            '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-appfw-application)',
            '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-system-info)',
            '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-system-sensor)',
            '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-system-device)',
            '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-location-manager)',
            '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-media-audio-io)',
            '<!@(<(pkg-config) --libs-only-L --libs-only-other capi-media-camera)',
            '<!@(<(pkg-config) --libs-only-L --libs-only-other vconf)',
          ],
          'libraries': [
            '<!@(<(pkg-config) --libs-only-l capi-appfw-application)',
            '<!@(<(pkg-config) --libs-only-l capi-system-info)',
            '<!@(<(pkg-config) --libs-only-l capi-system-sensor)',
            '<!@(<(pkg-config) --libs-only-l capi-system-device)',
            '<!@(<(pkg-config) --libs-only-l capi-location-manager)',
            '<!@(<(pkg-config) --libs-only-l capi-media-audio-io)',
            '<!@(<(pkg-config) --libs-only-l capi-media-camera)',
            '<!@(<(pkg-config) --libs-only-l vconf)',
            '<!@(<(pkg-config) --libs-only-l libtbm)',
            '<!@(<(pkg-config) --libs-only-l libdri2)',
            '<!@(<(pkg-config) --libs-only-l gles20)',
          ],
        },
        'conditions': [
          ['building_for_tizen_mobile==1', {
            'direct_dependent_settings': {
              'include_dirs': [
                '/usr/include/ug-1',
                '/usr/include/appfw',
              ],
              'cflags': [
                '<!@(<(pkg-config) --cflags efl-assist)',
              ],
            },
            'link_settings': {
              'ldflags': [
                '<!@(<(pkg-config) --libs-only-L --libs-only-other efl-assist)',
              ],
              'libraries': [
                '<!@(<(pkg-config) --libs-only-l efl-assist)',
              ],
            },
          }],
        ],
      },
      ],
    }],
  ], # conditions
}
