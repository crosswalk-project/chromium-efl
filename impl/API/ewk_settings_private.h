/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ewk_settings_private_h
#define ewk_settings_private_h

#include <Eina.h>
#include <Evas.h>

#include <content/public/common/web_preferences.h>

#include "public/ewk_settings.h"


class Ewk_Settings {
  public:
    Ewk_Settings(Evas_Object* evas_object, content::WebPreferences preferences)
        : m_preferences(preferences),
          m_autofillPasswordForm(false),
          m_formCandidateData(false),
          m_autofillProfileForm(false),
          m_textSelectionEnabled(true),
          m_autoClearTextSelection(true),
          m_autoSelectWord(false),
          m_edgeEffectEnabled(true),
          m_textZoomEnabled(true),
          m_loadRemoteImages(true),
          m_currentLegacyFontSizeMode(EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH),
          m_pasteImageUriEnabled(true),
          m_defaultKeypadEnabled(true),
          m_useKeyPadWithoutUserAction(true),
          m_textStyleState(true),
          m_autoFitting(false),
          m_linkEffectEnabled(true),
          m_evas_object(evas_object) {
    }

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
    void setCurrentLegacyFontSizeMode(Ewk_Legacy_Font_Size_Mode mode) {
      m_currentLegacyFontSizeMode = mode;
    }
    Ewk_Legacy_Font_Size_Mode currentLegacyFontSizeMode() const { return m_currentLegacyFontSizeMode; }
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
    Ewk_Legacy_Font_Size_Mode m_currentLegacyFontSizeMode;
    bool m_pasteImageUriEnabled;
    bool m_defaultKeypadEnabled;
    bool m_useKeyPadWithoutUserAction;
    bool m_textStyleState;
    bool m_autoFitting;
    bool m_linkEffectEnabled;
    Evas_Object* m_evas_object;
};

#endif // ewk_settings_private_h

