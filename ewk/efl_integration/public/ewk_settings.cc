// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_settings.h"

#include <tizen_webview/public/tw_webview.h>

// TODO: remove dependency
#include <ui/events/gestures/gesture_configuration.h>
#include <../impl/API/ewk_settings_private.h>
// TODO: move non-ewk header file to other location
#include "public/text_encoding_map_efl.h"
#include "eweb_view.h"

#include "private/chromium_glue.h"
#include "private/ewk_private.h"
#include "private/ewk_private.h"
#include "private/ewk_view_private.h"

using tizen_webview::WebView;
namespace {

void ewkUpdateWebkitPreferences(Evas_Object *ewkView)
{
  WebView* impl = WebView::FromEvasObject(ewkView);
  assert(impl);

  bool autoSelectWord = ewk_settings_select_word_automatically_get(impl->GetSettings());
  // TODO(sns.park) : remove dependency to chromium internals
  if(autoSelectWord){
#if !defined(EWK_BRINGUP)
    ui::GestureConfiguration::set_double_tap_to_zoom_enabled(false);
#endif
  }
  else{
#if !defined(EWK_BRINGUP)
    ui::GestureConfiguration::set_double_tap_to_zoom_enabled(true);
#endif
  }
  impl->UpdateWebKitPreferences();
}

}

Eina_Bool ewk_settings_fullscreen_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
#if !defined(EWK_BRINGUP)
#warning "[M38] fullscreen_enabled flag has been deleted from webpreference.h"
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->getPreferences().fullscreen_enabled = enable;
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
#else
  return false;
#endif
}

Eina_Bool ewk_settings_fullscreen_enabled_get(const Ewk_Settings* settings)
{
#if !defined(EWK_BRINGUP)
#warning "[M38] fullscreen_enabled flag has been deleted from webpreference.h"
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->getPreferences().fullscreen_enabled;
#else
  return false;
#endif
}

Eina_Bool ewk_settings_javascript_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->getPreferences().javascript_enabled = enable;
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_javascript_enabled_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->getPreferences().javascript_enabled;
}

Eina_Bool ewk_settings_loads_images_automatically_set(Ewk_Settings* settings, Eina_Bool automatic)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->getPreferences().loads_images_automatically = automatic;
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_loads_images_automatically_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->getPreferences().loads_images_automatically;
}

Eina_Bool ewk_settings_plugins_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->getPreferences().plugins_enabled = enable;
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_plugins_enabled_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->getPreferences().plugins_enabled;
}

Eina_Bool ewk_settings_auto_fitting_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  // FIXME: To be implemented when the functionality is required.
  // To be added in WebkitPreferences -> WebSettings -> Settings
  settings->setAutoFitting(enable);
  return true;
}

Eina_Bool ewk_settings_auto_fitting_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  // FIXME: To be implemented when the functionality is required.
  // To be added in WebkitPreferences -> WebSettings -> Settings
  return settings->autoFitting();
}

Eina_Bool ewk_settings_force_zoom_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if !defined(EWK_BRINGUP)
  settings->getPreferences().force_enable_zoom = enable;
#endif
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_force_zoom_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if !defined(EWK_BRINGUP)
  return settings->getPreferences().force_enable_zoom;
#else
  return false;
#endif
}

Eina_Bool ewk_settings_font_default_size_set(Ewk_Settings* settings, int size)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->getPreferences().default_font_size = size;
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

int ewk_settings_font_default_size_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->getPreferences().default_font_size;
}

Eina_Bool ewk_settings_scripts_window_open_set(Ewk_Settings* settings, Eina_Bool allow)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->getPreferences().javascript_can_open_windows_automatically = allow;
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_scripts_window_open_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->getPreferences().javascript_can_open_windows_automatically;
}

Eina_Bool ewk_settings_compositing_borders_visible_set(Ewk_Settings* settings, Eina_Bool enable)
{
  /*
  This API is impossible to implement in same manner as it was in webkit-efl
  for few reasons: chromium made compositing-borders-visible setting global - it
  is either enabled or disabled for whole process, it requires application restart
  because it is controled by command line switch. It must be provided before
  spawning any renderer/zygote processes
  */

  LOG_EWK_API_MOCKUP("Not implement by difference of webkit2 and chromium's behavior");
  return false;
}

Eina_Bool ewk_settings_default_encoding_set(Ewk_Settings* settings, const char* encoding)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(encoding, false);
  settings->getPreferences().default_encoding = std::string(encoding);
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_is_encoding_valid(const char* encoding)
{
  return TextEncodingMapEfl::isTextEncodingValid(encoding);
}

const char* ewk_settings_default_encoding_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, 0);
  return settings->getPreferences().default_encoding.c_str();
}

Eina_Bool ewk_settings_private_browsing_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  // This API is not supported by blink engine because it does not support toggling of private browsing mode
  // Chromium allows to create new WebContext with private browsing enabled for whole lifespan of WebContext
  // To support private browsing we need to add new ewk API that allows us to create either context or view with
  // private browsing enabled
  LOG_EWK_API_MOCKUP("Not supported by chromium");
  return EINA_FALSE;
}

Eina_Bool ewk_settings_private_browsing_enabled_get(const Ewk_Settings* settings)
{
  // This API is not supported by blink engine because it does not support toggling of private browsing mode
  // Chromium allows to create new WebContext with private browsing enabled for whole lifespan of WebContext
  // To support private browsing we need to add new ewk API that allows us to create either context or view with
  // private browsing enabled
  LOG_EWK_API_MOCKUP("Not supported by chromium");
  return EINA_FALSE;
}

Eina_Bool ewk_settings_editable_link_behavior_set(Ewk_Settings* settings, Ewk_Editable_Link_Behavior behavior)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if !defined(EWK_BRINGUP)
  settings->getPreferences().editable_link_behavior = static_cast<webkit_glue::EditableLinkBehavior>(behavior);
#endif
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_load_remote_images_set(Ewk_Settings* settings, Eina_Bool loadRemoteImages)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  // FIXME: To be implemented when the functionality is required.
  // To be added in WebkitPreferences -> WebSettings -> Settings
  settings->setLoadRemoteImages(loadRemoteImages);
  return true;
}

Eina_Bool ewk_settings_load_remote_images_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  // FIXME: To be implemented when the functionality is required.
  // To be added in WebkitPreferences -> WebSettings -> Settings
  return settings->loadRemoteImages();
}

Eina_Bool ewk_settings_scan_malware_enabled_set(Ewk_Settings* settings, Eina_Bool scanMalwareEnabled)
{
  // API is postponed. Possibly chromium's mechanism may be reused.
  LOG_EWK_API_MOCKUP("Not Supported yet");
  return false;
}

Eina_Bool ewk_settings_spdy_enabled_set(Ewk_Settings* settings, Eina_Bool spdyEnabled)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, EINA_FALSE);
  settings->setSpdyEnabled(spdyEnabled);

  return EINA_TRUE;
}

Eina_Bool ewk_settings_performance_features_enabled_set(Ewk_Settings* settings, Eina_Bool spdyEnabled)
{
  (void) settings;
  (void) spdyEnabled;
  // This function was used to set some libsoup settings.
  // Chromium doesn't use libsoup.
  LOG_EWK_API_MOCKUP("Not Supported by chromium");
  return EINA_FALSE;
}

void ewk_settings_link_magnifier_enabled_set(Ewk_Settings* settings, Eina_Bool enabled)
{
  EINA_SAFETY_ON_NULL_RETURN(settings);
  EWK_VIEW_IMPL_GET_OR_RETURN(settings->getEvasObject(), webview);

  webview->SetLinkMagnifierEnabled(enabled);
}

Eina_Bool ewk_settings_link_magnifier_enabled_get(const Ewk_Settings *settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, EINA_FALSE);
  EWK_VIEW_IMPL_GET_OR_RETURN(const_cast<Ewk_Settings *>(settings)->getEvasObject(), webview, EINA_FALSE);

  return webview->GetLinkMagnifierEnabled();
}

Eina_Bool ewk_settings_link_effect_enabled_set(Ewk_Settings* settings, Eina_Bool linkEffectEnabled)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
    settings->setLinkEffectEnabled(linkEffectEnabled);
    return true;
}

Eina_Bool ewk_settings_link_effect_enabled_get(const Ewk_Settings* settings)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
    return settings->linkEffectEnabled();
}

Eina_Bool ewk_settings_uses_encoding_detector_set(Ewk_Settings* settings, Eina_Bool use)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if !defined(EWK_BRINGUP)
  settings->getPreferences().uses_encoding_detector = use;
#endif
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_uses_encoding_detector_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if !defined(EWK_BRINGUP)
  return settings->getPreferences().uses_encoding_detector;
#else
  return false;
#endif
}

Eina_Bool ewk_settings_default_keypad_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);

    settings->setEnableDefaultKeypad(enable);

    return true;
}

Eina_Bool ewk_settings_default_keypad_enabled_get(const Ewk_Settings* settings)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);

    return settings->defaultKeypadEnabled();
}

Eina_Bool ewk_settings_uses_keypad_without_user_action_set(Ewk_Settings* settings, Eina_Bool use)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->setUseKeyPadWithoutUserAction(use);
  return true;
}

Eina_Bool ewk_settings_uses_keypad_without_user_action_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->useKeyPadWithoutUserAction();
}

Eina_Bool ewk_settings_text_zoom_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->setTextZoomEnabled(enable);
  return true;
}

Eina_Bool ewk_settings_text_zoom_enabled_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->textZoomEnabled();
}

Eina_Bool ewk_settings_autofill_password_form_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->setAutofillPasswordForm(enable);
  return true;
}

Eina_Bool ewk_settings_autofill_password_form_enabled_get(Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->autofillPasswordForm();
}

Eina_Bool ewk_settings_form_candidate_data_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->setFormCandidateData(enable);
  return true;
}

Eina_Bool ewk_settings_form_candidate_data_enabled_get(Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->formCandidateData();
}

Eina_Bool ewk_settings_form_profile_data_enabled_set(Ewk_Settings *settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->setAutofillProfileForm(enable);
  return true;
}

Eina_Bool ewk_settings_form_profile_data_enabled_get(const Ewk_Settings *settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->autofillProfileForm();
}

Eina_Bool ewk_settings_current_legacy_font_size_mode_set(Ewk_Settings *settings, Ewk_Legacy_Font_Size_Mode mode)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);

  settings->setCurrentLegacyFontSizeMode(chromium_glue::to(mode));
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Ewk_Legacy_Font_Size_Mode ewk_settings_current_legacy_font_size_mode_get(const Ewk_Settings *settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH);
  return chromium_glue::from(settings->currentLegacyFontSizeMode());
}

Eina_Bool ewk_settings_paste_image_uri_mode_set(Ewk_Settings *settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  // FIXME: To be implemented when the functionality is required.
  // To be added in WebkitPreferences -> WebSettings -> Settings
  settings->setPasteImageUriEnabled(enable);
  return true;
}

Eina_Bool ewk_settings_paste_image_uri_mode_get(const Ewk_Settings *settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  // FIXME: To be implemented when the functionality is required.
  // To be added in WebkitPreferences -> WebSettings -> Settings
  return settings->pasteImageUriEnabled();
}


Eina_Bool ewk_settings_text_selection_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->setTextSelectionEnabled(enable);
  return true;
}

Eina_Bool ewk_settings_text_selection_enabled_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->textSelectionEnabled();
}

Eina_Bool ewk_settings_clear_text_selection_automatically_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->setAutoClearTextSelection(enable);
  return true;
}

Eina_Bool ewk_settings_clear_text_selection_automatically_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->autoClearTextSelection();
}

Eina_Bool ewk_settings_text_autosizing_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if defined(OS_TIZEN_MOBILE) && !defined(EWK_BRINGUP)
  settings->getPreferences().text_autosizing_enabled = enable;
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
#else
  LOG_EWK_API_MOCKUP("Only for Tizen Mobile Browser");
  return false;
#endif
}

Eina_Bool ewk_settings_text_autosizing_enabled_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if defined(OS_TIZEN_MOBILE) && !defined(EWK_BRINGUP)
  return settings->getPreferences().text_autosizing_enabled;
#else
  LOG_EWK_API_MOCKUP("Only for Tizen Mobile Browser");
  return false;
#endif
}

Eina_Bool ewk_settings_text_autosizing_font_scale_factor_set(Ewk_Settings* settings, double factor)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if !defined(EWK_BRINGUP)
  settings->getPreferences().font_scale_factor = factor;
#endif
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

double ewk_settings_text_autosizing_font_scale_factor_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, -1.0);
#if !defined(EWK_BRINGUP)
  return settings->getPreferences().font_scale_factor;
#else
  return false;
#endif
}

Eina_Bool ewk_settings_edge_effect_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->setEdgeEffectEnabled(enable);
  return true;
}

Eina_Bool ewk_settings_edge_effect_enabled_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->edgeEffectEnabled();
}

void ewk_settings_text_style_state_enabled_set(Ewk_Settings* settings, Eina_Bool enabled)
{
  EINA_SAFETY_ON_NULL_RETURN(settings);
  settings->setTextStyleStateState(enabled);
}

Eina_Bool ewk_settings_text_style_state_enabled_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->textStyleStateState();
}

Eina_Bool ewk_settings_select_word_automatically_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->setAutoSelectWord(enable);
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_select_word_automatically_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  return settings->autoSelectWord();
}

Ewk_List_Style_Position ewk_settings_initial_list_style_position_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, EWK_LIST_STYLE_POSITION_OUTSIDE);
  // TODO: To be used when the functionality is required.
#if !defined(EWK_BRINGUP)
  return static_cast<Ewk_List_Style_Position>(settings->getPreferences().initial_list_style_position);
#else
  return static_cast<Ewk_List_Style_Position>(0);
#endif
}

Eina_Bool ewk_settings_initial_list_style_position_set(Ewk_Settings* settings, Ewk_List_Style_Position style)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if !defined(EWK_BRINGUP)
  settings->getPreferences().initial_list_style_position = static_cast<webkit_glue::ListStylePosition>(style);
#endif
  ewkUpdateWebkitPreferences(settings->getEvasObject());
  return true;
}

Eina_Bool ewk_settings_webkit_text_size_adjust_enabled_set(Ewk_Settings* settings, Eina_Bool enabled)
{
  // Chromium does not support CSS property "-webkit-text-size-adjust"
  // and this API was created to disable this property.
  // There is no need to implement this API.
  LOG_EWK_API_MOCKUP("Not Supported by chromium");
  return false;
}

// TODO: this API is gone in newer versions of webkit-efl
void ewk_settings_detect_contents_automatically_set(Ewk_Settings* settings, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN(settings);
#if !defined(EWK_BRINGUP)
  settings->setDetectContentsEnabled(enable);
#endif
}

Eina_Bool ewk_settings_detect_contents_automatically_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
#if !defined(EWK_BRINGUP)
  return settings->getDetectContentsEnabled();
#else
  return EINA_FALSE;
#endif
}

void ewk_settings_cache_builder_enabled_set(Ewk_Settings *settings, Eina_Bool enabled)
{
#if defined(OS_TIZEN_TV)
  EINA_SAFETY_ON_NULL_RETURN(settings);
  settings->setCacheBuilderEnabled(enabled);
#else
  LOG_EWK_API_MOCKUP("Only for Tizen TV Browser");
#endif
}

int ewk_settings_default_font_size_get(const Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, 0);
  return settings->getPreferences().default_font_size;
}

Eina_Bool ewk_settings_default_font_size_set(Ewk_Settings* settings, int size)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  settings->getPreferences().default_font_size = size;
  return true;
}

const char* ewk_settings_default_text_encoding_name_get(Ewk_Settings* settings)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, NULL);
  return settings->defaultTextEncoding();
}

Eina_Bool ewk_settings_default_text_encoding_name_set(Ewk_Settings* settings, const char* encoding)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(encoding, false);
  settings->setDefaultTextEncoding(encoding);
  return true;
}

#if defined(OS_TIZEN_TV)
Eina_Bool ewk_settings_encoding_detector_enabled_set(Ewk_Settings* settings, Eina_Bool enable)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
  return false;
}

Eina_Bool ewk_settings_scripts_can_open_windows_set(Ewk_Settings* settings, Eina_Bool enable)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
  return false;
}
#endif // OS_TIZEN_TV

namespace {
#define EXTRA_FEATURE_FUNCTIONS(VALNAME) \
  void Ewk_Extra_Feature_Set_ ## VALNAME(Ewk_Settings* settings, Eina_Bool value) \
  { \
    EINA_SAFETY_ON_NULL_RETURN(settings); \
    settings->set ## VALNAME ## Enabled(value == EINA_TRUE); \
  } \
  Eina_Bool Ewk_Extra_Feature_Get_ ## VALNAME(const Ewk_Settings* settings) \
  { \
    EINA_SAFETY_ON_NULL_RETURN_VAL(settings, EINA_FALSE); \
    return settings->get ## VALNAME ## Enabled() ? EINA_TRUE : EINA_FALSE; \
  }

#define EWK_EXTRA_FEATURE(NAME, VALNAME) {NAME, Ewk_Extra_Feature_Set_## VALNAME, Ewk_Extra_Feature_Get_ ## VALNAME}

#if !defined(EWK_BRINGUP)
  EXTRA_FEATURE_FUNCTIONS(LongPress)
  EXTRA_FEATURE_FUNCTIONS(LinkMagnifier)
  EXTRA_FEATURE_FUNCTIONS(DetectContents)
  EXTRA_FEATURE_FUNCTIONS(WebLogin)
  EXTRA_FEATURE_FUNCTIONS(DoubleTap)
  EXTRA_FEATURE_FUNCTIONS(Zoom)
  EXTRA_FEATURE_FUNCTIONS(OpenPanel)
  EXTRA_FEATURE_FUNCTIONS(AllowRestrictedURL)
  EXTRA_FEATURE_FUNCTIONS(URLBarHide)
#endif

  typedef struct {
    const char* name;
    void (*set)(Ewk_Settings*, Eina_Bool enable);
    Eina_Bool (*get)(const Ewk_Settings*);
  } Ewk_Extra_Feature;

  static Ewk_Extra_Feature extra_features[] = {
#if !defined(EWK_BRINGUP)
    EWK_EXTRA_FEATURE("longpress,enable", LongPress),
    EWK_EXTRA_FEATURE("link,magnifier", LinkMagnifier),
    EWK_EXTRA_FEATURE("detect,contents", DetectContents),
    EWK_EXTRA_FEATURE("web,login", WebLogin),
    EWK_EXTRA_FEATURE("doubletap,enable", DoubleTap),
    EWK_EXTRA_FEATURE("zoom,enable", Zoom),
    EWK_EXTRA_FEATURE("openpanel,enable", OpenPanel),
    EWK_EXTRA_FEATURE("allow,restrictedurl", AllowRestrictedURL),
    EWK_EXTRA_FEATURE("urlbar,hide", URLBarHide),
#endif
    {NULL, NULL, NULL}
  };

  Ewk_Extra_Feature* find_extra_feature(const char*feature)
  {
    for(Ewk_Extra_Feature *feat = extra_features; feat->name != NULL; feat++) {
      if (strcasecmp(feat->name, feature) == 0) {
        return feat;
      }
    }

    return NULL;
  }
}

Eina_Bool ewk_settings_extra_feature_get(const Ewk_Settings* settings, const char* feature)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(settings, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(feature, EINA_FALSE);
  Ewk_Extra_Feature *feat = find_extra_feature(feature);
  EINA_SAFETY_ON_NULL_RETURN_VAL(feat, EINA_FALSE);
  return feat->get(settings);
}

void ewk_settings_extra_feature_set(Ewk_Settings* settings, const char* feature, Eina_Bool enable)
{
  EINA_SAFETY_ON_NULL_RETURN(settings);
  EINA_SAFETY_ON_NULL_RETURN(feature);
  Ewk_Extra_Feature *feat = find_extra_feature(feature);
  EINA_SAFETY_ON_NULL_RETURN(feat);
  feat->set(settings, enable);
}
