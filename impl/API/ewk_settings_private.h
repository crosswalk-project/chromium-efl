// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_settings_private_h
#define ewk_settings_private_h

#include <Eina.h>
#include <Evas.h>

#include <content/public/common/web_preferences.h>

#include "tizen_webview/public/tw_legacy_font_size_mode.h"


class Ewk_Settings {
  public:
    Ewk_Settings(Evas_Object* evas_object, content::WebPreferences preferences)
        : m_preferences(preferences)
        , m_autofillPasswordForm(false)
        , m_formCandidateData(false)
        , m_autofillProfileForm(false)
        , m_textSelectionEnabled(true)
        , m_autoClearTextSelection(true)
        , m_autoSelectWord(false)
        , m_edgeEffectEnabled(true)
        , m_textZoomEnabled(true)
        , m_loadRemoteImages(true)
        , m_currentLegacyFontSizeMode(tizen_webview::TW_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH)
        , m_pasteImageUriEnabled(true)
        , m_defaultKeypadEnabled(true)
        , m_useKeyPadWithoutUserAction(true)
        , m_textStyleState(true)
        , m_autoFitting(false)
        , m_linkEffectEnabled(true)
        , m_evas_object(evas_object)
    { }

    const char* defaultTextEncoding() const { return m_defaultTextEncoding; }
    void setDefaultTextEncoding(const char*);
    bool autofillPasswordForm() const { return m_autofillPasswordForm; }
    void setAutofillPasswordForm(bool enable) { m_autofillPasswordForm = enable; }
    bool formCandidateData() const { return m_formCandidateData; }
    void setFormCandidateData(bool enable) { m_formCandidateData = enable; }
    bool autofillProfileForm() const { return m_autofillProfileForm; }
    void setAutofillProfileForm(bool enable) { m_autofillProfileForm = enable; }
    bool textSelectionEnabled() const { return m_textSelectionEnabled; }
    void setTextSelectionEnabled(bool enable) { m_textSelectionEnabled = enable; }
    bool autoClearTextSelection() const { return m_autoClearTextSelection; }
    void setAutoClearTextSelection(bool enable) { m_autoClearTextSelection = enable; }
    bool autoSelectWord() const { return m_autoSelectWord; }
    void setAutoSelectWord(bool enable) { m_autoSelectWord = enable; }
    bool edgeEffectEnabled() const { return m_edgeEffectEnabled; }
    void setEdgeEffectEnabled(bool enable) { m_edgeEffectEnabled = enable; }
    void setTextZoomEnabled(bool enable) { m_textZoomEnabled = enable; }
    bool textZoomEnabled() const { return m_textZoomEnabled; }
    void setLoadRemoteImages(bool loadRemoteImages) { m_loadRemoteImages = loadRemoteImages; }
    bool loadRemoteImages() const { return m_loadRemoteImages; }
    void setCurrentLegacyFontSizeMode(tizen_webview::Legacy_Font_Size_Mode mode);
    tizen_webview::Legacy_Font_Size_Mode currentLegacyFontSizeMode() const { return m_currentLegacyFontSizeMode; }
    void setPasteImageUriEnabled(bool enable) { m_pasteImageUriEnabled = enable; }
    bool pasteImageUriEnabled() const {return m_pasteImageUriEnabled;}
    bool defaultKeypadEnabled() const { return m_defaultKeypadEnabled; }
    void setEnableDefaultKeypad(bool flag) { m_defaultKeypadEnabled = flag; }
    bool useKeyPadWithoutUserAction() const { return m_useKeyPadWithoutUserAction; }
    void setUseKeyPadWithoutUserAction(bool enable) { m_useKeyPadWithoutUserAction = enable; }
    bool textStyleStateState() const { return m_textStyleState; }
    void setTextStyleStateState(bool enable) { m_textStyleState = enable; }
    bool autoFitting() const {return m_autoFitting; }
    void setAutoFitting(bool enable) {m_autoFitting = enable; }
    bool linkEffectEnabled() const { return m_linkEffectEnabled; }
    void setLinkEffectEnabled(bool flag) { m_linkEffectEnabled = flag; }
    void setSpdyEnabled(bool flag);
    void setDetectContentsAutomatically(bool enable) { m_detectContentsAutomatically = enable; }
    bool detectContentsAutomatically() const { return m_detectContentsAutomatically; }

    Evas_Object* getEvasObject() { return m_evas_object; }
    content::WebPreferences& getPreferences() { return m_preferences; }
    const content::WebPreferences& getPreferences() const { return m_preferences; }

  private:
    content::WebPreferences m_preferences;
    Eina_Stringshare* m_defaultTextEncoding;
    bool m_autofillPasswordForm;
    bool m_formCandidateData;
    bool m_autofillProfileForm;
    bool m_textSelectionEnabled;
    bool m_autoClearTextSelection;
    bool m_autoSelectWord;
    bool m_edgeEffectEnabled;
    bool m_textZoomEnabled;
    bool m_loadRemoteImages;
    tizen_webview::Legacy_Font_Size_Mode m_currentLegacyFontSizeMode;
    bool m_pasteImageUriEnabled;
    bool m_defaultKeypadEnabled;
    bool m_useKeyPadWithoutUserAction;
    bool m_textStyleState;
    bool m_autoFitting;
    bool m_linkEffectEnabled;
    bool m_detectContentsAutomatically;
    Evas_Object* m_evas_object;
};

#endif // ewk_settings_private_h

