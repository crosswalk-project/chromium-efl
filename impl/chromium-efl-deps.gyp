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
    'variables': {
      'dependent_pkgs': [
        'ecore',
        'ecore-x',
        'ecore-evas',
        'ecore-input',
        'ecore-imf-evas',
        'evas',
        'elementary',
      ],
    },
    'direct_dependent_settings': {
      'cflags': [
        '<!@(<(pkg-config) --cflags <(dependent_pkgs))',
      ],
    },
    'link_settings': {
      'ldflags': [
        '<!@(<(pkg-config) --libs-only-L --libs-only-other <(dependent_pkgs))',
      ],
      'libraries': [
        '<!@(<(pkg-config) --libs-only-l <(dependent_pkgs))',
      ],
    },
  },
  {
    'target_name': 'gstreamer',
    'type': 'none',
    'variables': {
      'dependent_pkgs': [
        'glib-2.0',
        'gstreamer-0.10',
        'gstreamer-base-0.10',
        'gstreamer-app-0.10',
        'gstreamer-pbutils-0.10',
      ],
    },
    'direct_dependent_settings': {
      'cflags': [
        '<!@(<(pkg-config) --cflags <(dependent_pkgs))',
      ],
    },
    'link_settings': {
      'ldflags': [
        '<!@(<(pkg-config) --libs-only-L --libs-only-other <(dependent_pkgs))',
      ],
      'libraries': [
        '<!@(<(pkg-config) --libs-only-l <(dependent_pkgs))',
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
        'variables': {
          'dependent_pkgs': [
            'capi-appfw-application',
            'capi-system-info',
            'capi-system-sensor',
            'capi-system-device',
            'capi-location-manager',
            'capi-media-audio-io',
            'capi-media-camera',
            'vconf',
            'libtbm',
            'libdri2',
            'gles20',
            'ui-gadget-1',
            'feedback',
          ],
        },
        'direct_dependent_settings': {
          'cflags': [
            '<!@(<(pkg-config) --cflags <(dependent_pkgs))',
          ],
        },
        'link_settings': {
          'ldflags': [
            '<!@(<(pkg-config) --libs-only-L --libs-only-other <(dependent_pkgs))',
          ],
          'libraries': [
            '<!@(<(pkg-config) --libs-only-l <(dependent_pkgs))',
          ],
        },
        'conditions': [
          ['building_for_tizen_mobile==1', {
            'variables': {
              'dependent_pkgs': [
                'efl-assist',
              ],
            },
            'direct_dependent_settings': {
              'include_dirs': [
                '/usr/include/ug-1',
                '/usr/include/appfw',
              ],
              'cflags': [
                '<!@(<(pkg-config) --cflags <(dependent_pkgs))',
              ],
            },
            'link_settings': {
              'ldflags': [
                '<!@(<(pkg-config) --libs-only-L --libs-only-other <(dependent_pkgs))',
              ],
              'libraries': [
                '<!@(<(pkg-config) --libs-only-l <(dependent_pkgs))',
              ],
            },
          }],
          # FIXME_youngsoo: This should be removed when TV upgrades
          # to Tizen 2.3 or higher.
          ['chromium_efl_tizen_version=="2.2.1"', {
            'direct_dependent_settings': {
              'include_dirs': [
                '/usr/include/sensor',
              ]
            }
          }],
        ],
      },
      ],
    }],
  ], # conditions
}
