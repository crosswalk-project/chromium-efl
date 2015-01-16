{
  'variables': {
    'ewk_bringup%': 1,
    '__GCC_VERSION%': '<!(gcc -dumpversion)',
    'conditions': [
      ['building_for_tizen==1', {
        'clang': 0,
      }],
      ['building_for_tizen_mobile==1', {
        'gcc_4_6_x': 1,
      }, {
        'gcc_4_6_x': 0,
      }],
    ],
   'chromium_efl_tizen_version%': '2.4',
   'custom_libc_dir%': '',
   'prebuilt_ld_gold_dir%': '',
   'grit_additional_defines': ['-D', 'use_efl',],
  },
  'target_defaults': {
    'defines': [
      'COMPONENT_BUILD=1',
      'SKIA_DLL=1',
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
      ['exclude', 'x11_event_source_glib\\.cc$'],
      ['exclude', 'x11_event_source_libevent\\.cc$'],
      ['exclude', 'x11_types\\.cc$'],
    ],
    'target_conditions': [
      ['_target_name=="content_renderer" and use_efl==1', {
        'sources': [
          '<(chrome_src_dir)/content/renderer/external_popup_menu.cc',
          '<(chrome_src_dir)/content/renderer/external_popup_menu.h',
        ],
      }],
      ['_target_name=="webcore_rendering"', {
        'sources/': [
          # Have to exclude under target_conditions here because of multiple
          # includes/excludes in core.gyp
          ['exclude', 'rendering/RenderThemeChromiumDefault\\.cpp$'],
          ['exclude', 'rendering/RenderThemeChromiumLinux\\.cpp$'],
        ],
      }],
      ['_target_name=="gesture_detection"', {
        'sources/': [
          ['exclude', 'gesture_detection/gesture_configuration_default\\.cc$'],
        ],
      }],
      ['_target_name=="boringssl" or _target_name=="crypto" or _target_name=="genperf" or  _target_name=="yasm" or _target_name=="speech_proto" or _target_name=="skia_library" or  _target_name=="http_server" or _target_name=="libjingle" or  _target_name=="libjingle_webrtc_common" or _target_name=="content_renderer"', {
        'defines!': [
          'final=',
        ],
      }],
      ['_target_name=="usrsctplib"', {
        'defines!': [
          'override=',
        ],
      }],
      ['_target_name=="webrtc_base" or _target_name=="rtc_base" or _target_name=="v8_base"', {
        'defines!': [
          'final=',
          'override=',
        ],
      }],
    ],
    'conditions': [
       ['use_efl==1', {
         'defines': [
           'USE_EFL=1',
         ],
       }],
       ['gcc_4_6_x==1', {
         'defines': [
           'GCC_4_6_X=1',
           'override=',
           'final=',
         ],
       }],
       ['tizen_multimedia_support==1', {
         'defines': [
           'TIZEN_MULTIMEDIA_SUPPORT=1',
         ],
         'sources/': [
           ['exclude', 'renderer/media/audio_decoder\\.(cc|h)$'],
         ],
         'include_dirs': [
           '<(efl_impl_dir)',
         ],
       }],
       ['building_for_tizen==1', {
         'defines': [
           'OS_TIZEN=1',
           'TIZEN_MULTIMEDIA_PIXMAP_SUPPORT=1',
         # For GCC 4.5.3 bundled arm.h has a bug and don't define __ARM_PCS when it should.
         # Force define this flag for the whole chromium on gbs gcc 4.5.3.
         # Non-arm builds will ingore it in any case.
         '__ARM_PCS',
         ],
         'conditions': [
           ['chromium_efl_tizen_version=="2.4"', {
             'defines': [
               'TIZEN_V_2_4=1',
             ],
           }],
           ['chromium_efl_tizen_version=="2.3"', {
             'defines': [
               'TIZEN_V_2_3=1',
             ],
           }],
           ['chromium_efl_tizen_version=="2.2.1"', {
             'defines': [
               'TIZEN_LEGACY_V_2_2_1=1',
             ],
           }],
           ['tizen_multimedia_support==1', {
             'sources/': [
               ['exclude', 'video/capture/linux/video_capture_device_factory_linux.cc$'],
               ['exclude', 'video/capture/linux/video_capture_device_factory_linux.h$'],
               ['exclude', 'video/capture/linux/video_capture_device_linux.h$'],
               ['exclude', 'video/capture/linux/video_capture_device_linux.cc$'],
             ],
           }],
         ],
         'sources/': [
           ['exclude', 'browser/device_sensors/data_fetcher_shared_memory_default.cc$'],
           ['exclude', 'browser/sound_effect.cc$'],
           ['exclude', 'battery_status_manager_linux\\.(h|cc)$'],
         ],
         'cflags_cc': [ '-std=gnu++0x', '-fpermissive' ],
      }, {
        'sources/': [
          ['exclude', 'browser/device_sensors/data_fetcher_impl_tizen\\.(cc|h)$'],
          ['exclude', 'browser/device_sensors/data_fetcher_shared_memory_tizen\\.cc$'],
          ['exclude', 'browser/geolocation/location_provider_efl\\.(cc|h)$'],
          ['exclude', 'browser/motion/MotionUI\\.(h|cc)$'],
          ['exclude', 'browser/motion/wkext_motion\\.(h|cc)$'],
          ['exclude', 'browser/sound_effect_tizen.cc$'],
          ['exclude', 'device/battery/battery_status_manager_tizen\\.(h|cc)$'],
          ['exclude', 'third_party/webrtc/system_wrappers/source/cpu_features_tizen.c'],
        ],
      }],
      ['building_for_tizen_mobile==1', {
         'defines': [
           'OS_TIZEN_MOBILE=1',
           'TIZEN_CONTENTS_DETECTION=1',
           'TIZEN_ENABLE_CAPI_SUPPORT=1',
           ],
      }],
      ['building_for_tizen_tv==1', {
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
