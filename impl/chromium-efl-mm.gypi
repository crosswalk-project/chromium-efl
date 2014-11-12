{
  'sources': [
    'media/base/tizen/webaudio_media_codec_info_tizen.h',
    'content/browser/media/tizen/webaudio_decoder_browser_gstreamer.cc',
    'content/browser/media/tizen/webaudio_decoder_browser_gstreamer.h',
    'content/renderer/media/tizen/audio_decoder_gstreamer.cc',
    'content/renderer/media/tizen/audio_decoder_gstreamer.h',
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
  ],
}
