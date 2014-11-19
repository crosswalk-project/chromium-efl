{
  'include_dirs': [
    '<(chrome_src_dir)/v8/include',
  ],
  'sources': [
    'media/base/tizen/webaudio_media_codec_info_tizen.h',
    'content/browser/media/tizen/webaudio_decoder_browser_gstreamer.cc',
    'content/browser/media/tizen/webaudio_decoder_browser_gstreamer.h',
    'content/renderer/media/tizen/audio_decoder_gstreamer.cc',
    'content/renderer/media/tizen/audio_decoder_gstreamer.h',
    'content/browser/media/media_web_contents_observer_tizen.cc',
    'content/browser/media/media_web_contents_observer_tizen.h',
    'content/browser/media/tizen/browser_demuxer_tizen.cc',
    'content/browser/media/tizen/browser_demuxer_tizen.h',
    'content/browser/media/tizen/browser_media_player_manager_tizen.cc',
    'content/browser/media/tizen/browser_media_player_manager_tizen.h',
    'content/common/media/tizen/media_player_messages_enums_tizen.h',
    'content/common/media/tizen/media_player_messages_tizen.h',
    'content/renderer/media/tizen/media_source_delegate_tizen.cc',
    'content/renderer/media/tizen/media_source_delegate_tizen.h',
    'content/renderer/media/tizen/renderer_demuxer_tizen.cc',
    'content/renderer/media/tizen/renderer_demuxer_tizen.h',
    'content/renderer/media/tizen/renderer_media_player_manager_tizen.cc',
    'content/renderer/media/tizen/renderer_media_player_manager_tizen.h',
    'media/base/tizen/demuxer_stream_player_params_tizen.cc',
    'media/base/tizen/demuxer_stream_player_params_tizen.h',
    'media/base/tizen/demuxer_tizen.h',
    'media/base/tizen/media_player_tizen.h',
    'media/base/tizen/media_player_bridge_gstreamer.cc',
    'media/base/tizen/media_player_bridge_gstreamer.h',
    'media/base/tizen/media_player_manager_tizen.h',
    'media/base/tizen/media_source_player_gstreamer.cc',
    'media/base/tizen/media_source_player_gstreamer.h',
    'media/base/tizen/webmediaplayer_tizen.cc',
    'media/base/tizen/webmediaplayer_tizen.h',#ME and MSE
    'media/video/capture/tizen/video_capture_device_factory_tizen_helper.cc',
  ],
  'conditions': [
    ['building_for_tizen==1', {
      'defines': [
       'TIZEN_VIDEO_CAPTURE_SUPPORT=1',
      ],
      'sources': [
        'media/video/capture/tizen/video_capture_device_tizen.h',
        'media/video/capture/tizen/video_capture_device_tizen.cc',
        'media/video/capture/tizen/video_capture_device_factory_tizen.h',
        'media/video/capture/tizen/video_capture_device_factory_tizen.cc',
      ],
      'dependencies': [
        '<(chrome_src_dir)/third_party/libyuv/libyuv.gyp:libyuv',
      ],
    }],
    ['building_for_tizen_tv==1', {
      'sources': [
        'media/audio/tizen/audio_session_manager.h',
        'media/audio/tizen/audio_session_manager.cc', #ASM
      ],
    }],
  ],
}
