{
  'target_defaults': {
    'link_settings': {
      'ldflags': [
        '-Wl,--no-undefined',
      ],
    },
    'dependencies': [
      '<(efl_impl_dir)/chromium-efl-deps.gyp:efl',
    ],
    'conditions': [
     ['building_for_tizen==1', {
      'dependencies': [
        '<(efl_impl_dir)/chromium-efl-deps.gyp:tizen',
      ],
     }],
     ['custom_libc_dir!=""', {
       'ldflags': [
         # We want to statically link libstdc++/libgcc_s.
         '-static-libstdc++',
         '-static-libgcc',
         '-Wl,-rpath,<(custom_libc_dir)',
         '-Wl,--dynamic-linker=<(custom_libc_dir)/ld-linux.so.3',
       ],
     }],
  ]},

  'includes': [
    'unittest/ewk-tests.gypi'
  ],

  'targets': [{
    'target_name': 'chromium-ewk',
    'type': 'shared_library',
    'include_dirs': [
      '<(efl_impl_dir)',
      'efl_integration',
      'efl_integration/public',
    ],

    'dependencies': [
      '<(efl_impl_dir)/chromium-efl.gyp:chromium-efl',
    ],

    'sources': [
      'efl_integration/public/ewk_application_cache_manager.cc',
      'efl_integration/public/ewk_application_cache_manager.h',
      'efl_integration/public/ewk_auth_challenge.cc',
      'efl_integration/public/ewk_auth_challenge.h',
      'efl_integration/public/ewk_auth_request.cc',
      'efl_integration/public/ewk_auth_request.h',
      'efl_integration/public/ewk_autofill_profile.cc',
      'efl_integration/public/ewk_autofill_profile.h',
      'efl_integration/public/ewk_back_forward_list.cc',
      'efl_integration/public/ewk_back_forward_list.h',
      'efl_integration/public/ewk_back_forward_list_item.cc',
      'efl_integration/public/ewk_back_forward_list_item.h',
      'efl_integration/public/ewk_certificate.cc',
      'efl_integration/public/ewk_certificate.h',
      'efl_integration/public/ewk_console_message.cc',
      'efl_integration/public/ewk_console_message.h',
      'efl_integration/public/ewk_content_screening_detection.cc',
      'efl_integration/public/ewk_content_screening_detection.h',
      'efl_integration/public/ewk_context.cc',
      'efl_integration/public/ewk_context.h',
      'efl_integration/public/ewk_context_menu.cc',
      'efl_integration/public/ewk_context_menu.h',
      'efl_integration/public/ewk_cookie_manager.cc',
      'efl_integration/public/ewk_cookie_manager.h',
      'efl_integration/public/ewk_custom_handlers.cc',
      'efl_integration/public/ewk_custom_handlers.h',
      'efl_integration/public/ewk_dispatcher.cc',
      'efl_integration/public/ewk_dispatcher.h',
      'efl_integration/public/ewk_error.cc',
      'efl_integration/public/ewk_error.h',
      'efl_integration/public/ewk_favicon_database.cc',
      'efl_integration/public/ewk_favicon_database.h',
      'efl_integration/public/ewk_frame.cc',
      'efl_integration/public/ewk_frame.h',
      'efl_integration/public/ewk_geolocation.cc',
      'efl_integration/public/ewk_geolocation.h',
      'efl_integration/public/ewk_history.cc',
      'efl_integration/public/ewk_history.h',
      'efl_integration/public/ewk_hit_test.cc',
      'efl_integration/public/ewk_hit_test.h',
      'efl_integration/public/ewk_intercept_request.cc',
      'efl_integration/public/ewk_intercept_request.h',
      'efl_integration/public/ewk_ipc_message.cc',
      'efl_integration/public/ewk_ipc_message.h',
      'efl_integration/public/ewk_main.cc',
      'efl_integration/public/ewk_main.h',
      'efl_integration/public/ewk_notification.cc',
      'efl_integration/public/ewk_notification.h',
      'efl_integration/public/ewk_object.cc',
      'efl_integration/public/ewk_object.h',
      'efl_integration/public/ewk_page_group.cc',
      'efl_integration/public/ewk_page_group.h',
      'efl_integration/public/ewk_policy_decision.cc',
      'efl_integration/public/ewk_policy_decision.h',
      'efl_integration/public/ewk_popup_menu_item.cc',
      'efl_integration/public/ewk_popup_menu_item.h',
      'efl_integration/public/ewk_quota_permission_request.cc',
      'efl_integration/public/ewk_quota_permission_request.h',
      'efl_integration/public/ewk_security_origin.cc',
      'efl_integration/public/ewk_security_origin.h',
      'efl_integration/public/ewk_settings.cc',
      'efl_integration/public/ewk_settings.h',
      'efl_integration/public/ewk_storage_manager.cc',
      'efl_integration/public/ewk_storage_manager.h',
      'efl_integration/public/ewk_text_style.cc',
      'efl_integration/public/ewk_text_style.h',
      'efl_integration/public/ewk_user_media.cc',
      'efl_integration/public/ewk_user_media.h',
      'efl_integration/public/ewk_view.cc',
      'efl_integration/public/ewk_view.h',
      'efl_integration/public/ewk_web_application_icon_data.cc',
      'efl_integration/public/ewk_web_application_icon_data.h',
      'efl_integration/public/ewk_window_features.cc',
      'efl_integration/public/ewk_window_features.h',
      'efl_integration/public/ewk_chromium.h',
      'efl_integration/public/ewk_enums.h',
      'efl_integration/public/ewk_export.h',
      'efl_integration/public/ewk_log.h',
      'efl_integration/public/ewk_touch.h',

      'efl_integration/private/api_static_check.cc',
      'efl_integration/private/chromium_glue.cc',
      'efl_integration/private/chromium_glue.h',
      'efl_integration/private/ewk_back_forward_list_private.h',
      'efl_integration/private/ewk_back_forward_list_private.cc',
      'efl_integration/private/ewk_context_private.cc',
      'efl_integration/private/ewk_context_private.h',
      'efl_integration/private/ewk_frame_private.h',
      'efl_integration/private/ewk_hit_test_private.h',
      'efl_integration/private/ewk_hit_test_private.cc',
      'efl_integration/private/ewk_main_private.h',
      'efl_integration/private/ewk_main_private.cc',
      'efl_integration/private/ewk_notification_private.h',
      'efl_integration/private/ewk_notification_private.cc',
      'efl_integration/private/ewk_policy_decision_private.cc',
      'efl_integration/private/ewk_policy_decision_private.h',
      'efl_integration/private/ewk_private.h',
      'efl_integration/private/ewk_quota_permission_request_private.h',
      'efl_integration/private/ewk_quota_permission_request_private.cc',
      'efl_integration/private/ewk_security_origin_private.h',
      'efl_integration/private/ewk_security_origin_private.cc',
      'efl_integration/private/ewk_view_private.h',
      'efl_integration/private/ewk_view_private.cc',
      'efl_integration/private/ewk_wrt_private.h',
      'efl_integration/private/ewk_wrt_private.cc',
      'efl_integration/private/ewk_history_private.h',
      'efl_integration/private/ewk_history_private.cc',
      'efl_integration/private/webview_delegate_ewk.h',
      'efl_integration/private/webview_delegate_ewk.cc',
      'efl_integration/private/ewk_favicon_database_private.h',
      'efl_integration/public/text_encoding_map_efl.cc',
      'efl_integration/public/text_encoding_map_efl.h',
    ],
    'conditions': [
      ['gcc_4_6_x==1', {
        'defines': [
          'override=',
          'final=',
        ],
      }],
      ['ewk_bringup==1', {
        'defines': [ 'EWK_BRINGUP=1' ],
      }]
    ]
  },

  {
    'target_name': 'efl_webview_app',
    'type': 'executable',

    'copies': [{
       'destination': '<(PRODUCT_DIR)',
       'files': [ 'efl_webview_app/efl_webview_app.sh', ],
     }],

    'dependencies': [
      'chromium-ewk',
      '<(efl_impl_dir)/chromium-efl.gyp:efl_webprocess',
    ],
    'include_dirs': [
      'efl_integration',
    ],
    'sources': [
      'efl_webview_app/app.c',
    ],
    'link_settings': {
      'ldflags': [
        '-Llib',
      ],
      'libraries': [
        '-lchromium-ewk',
      ],
    },
  },

  {
    'target_name': 'mini_browser',
    'type': 'executable',
     'conditions': [
      ['building_for_tizen==1', { # FIXME: mini_browser shouldn't depend on dlog and appcore-efl without OS(TIZEN) checks
       'sources': [
        'efl_webview_app/mini_browser.c',
        'efl_webview_app/browser-object.h',
        'efl_webview_app/browser-string.h',
       ],
       'include_dirs': [
        'efl_integration',
       ],
       'dependencies': [
        'chromium-ewk',
        '<(efl_impl_dir)/chromium-efl.gyp:efl_webprocess',
       ],
       'cflags': [
        '<!@(pkg-config --cflags capi-appfw-application)',
        '<!@(pkg-config --cflags dlog)',
       ],
       'link_settings': {
        'ldflags': [
          '<!@(pkg-config --libs-only-L --libs-only-other capi-appfw-application)',
          '<!@(pkg-config --libs-only-L --libs-only-other dlog)',
        ],
        'libraries': [
          '<!@(pkg-config --libs-only-l capi-appfw-application)',
          '<!@(pkg-config --libs-only-l dlog)',
        ]
       },
     }]
    ],
  }],
}
