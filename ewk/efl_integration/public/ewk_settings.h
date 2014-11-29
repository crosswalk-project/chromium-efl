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

/**
 * @file    ewk_settings.h
 * @brief   Describes the settings API.
 *
 * @note The ewk_settings is for setting the preference of specific ewk_view.
 * We can get the ewk_settings from ewk_view using ewk_view_settings_get() API.
 */

#ifndef ewk_settings_h
#define ewk_settings_h

#include <Eina.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for Ewk_Settings */
typedef struct Ewk_Settings Ewk_Settings;

/**
 * Creates a type name for the callback function used to notify the client when
 * the continuous spell checking setting was changed by WebKit.
 *
 * @param enable @c EINA_TRUE if continuous spell checking is enabled or @c EINA_FALSE if it's disabled
 */
typedef void (*Ewk_Settings_Continuous_Spell_Checking_Change_Cb)(Eina_Bool enable);

/**
 * \enum    _Ewk_Editable_Link_Behavior
 *
 * @brief   Editable link behavior mode (Must remain in sync with WKEditableLinkBehavior)
 */
enum _Ewk_Editable_Link_Behavior {
    EWK_EDITABLE_LINK_BEHAVIOR_DEFAULT,
    EWK_EDITABLE_LINK_BEHAVIOR_ALWAYS_LIVE,
    EWK_EDITABLE_LINK_BEHAVIOR_ONLY_LIVE_WITH_SHIFTKEY,
    EWK_EDITABLE_LINK_BEHAVIOR_LIVE_WHEN_NOT_FOCUSED,
    EWK_EDITABLE_LINK_BEHAVIOR_NEVER_LIVE
};
typedef enum _Ewk_Editable_Link_Behavior Ewk_Editable_Link_Behavior;

enum _Ewk_Legacy_Font_Size_Mode {
    EWK_LEGACY_FONT_SIZE_MODE_ALWAYS,
    EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH,
    EWK_LEGACY_FONT_SIZE_MODE_NEVER
};

typedef enum _Ewk_Legacy_Font_Size_Mode Ewk_Legacy_Font_Size_Mode;

enum _Ewk_List_Style_Position {
    EWK_LIST_STYLE_POSITION_OUTSIDE, /**< Default WebKit value. */
    EWK_LIST_STYLE_POSITION_INSIDE
};
typedef enum _Ewk_List_Style_Position Ewk_List_Style_Position;

/*
 * Enables/disables the Javascript Fullscreen API. The Javascript API allows
 * to request full screen mode, for more information see:
 * http://dvcs.w3.org/hg/fullscreen/raw-file/tip/Overview.html
 *
 * Default value for Javascript Fullscreen API setting is @c EINA_TRUE .
 *
 * @param settings settings object to enable Javascript Fullscreen API
 * @param enable @c EINA_TRUE to enable Javascript Fullscreen API or
 *               @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_fullscreen_enabled_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns whether the Javascript Fullscreen API is enabled or not.
 *
 * @param settings settings object to query whether Javascript Fullscreen API is enabled
 *
 * @return @c EINA_TRUE if the Javascript Fullscreen API is enabled
 *         @c EINA_FALSE if not or on failure
 */
EAPI Eina_Bool ewk_settings_fullscreen_enabled_get(const Ewk_Settings *settings);

/**
 * Enables/disables the javascript executing.
 *
 * @param settings settings object to set javascript executing
 * @param enable @c EINA_TRUE to enable javascript executing
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_javascript_enabled_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns the javascript can be executable or not.
 *
 * @param settings settings object to query if the javascript can be executed
 *
 * @return @c EINA_TRUE if the javascript can be executed
 *         @c EINA_FALSE if not or on failure
 */
EAPI Eina_Bool ewk_settings_javascript_enabled_get(const Ewk_Settings *settings);

/**
 * Enables/disables auto loading of the images.
 *
 * @param settings settings object to set auto loading of the images
 * @param automatic @c EINA_TRUE to enable auto loading of the images,
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_loads_images_automatically_set(Ewk_Settings *settings, Eina_Bool automatic);

/**
 * Returns the images can be loaded automatically or not.
 *
 * @param settings settings object to get auto loading of the images
 *
 * @return @c EINA_TRUE if the images are loaded automatically,
 *         @c EINA_FALSE if not or on failure
 */
EAPI Eina_Bool ewk_settings_loads_images_automatically_get(const Ewk_Settings *settings);

/**
 * Requests enables/disables the plug-ins.
 *
 * @param settings settings object to set the plug-ins
 * @param enable @c EINA_TRUE to enable the plug-ins
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_plugins_enabled_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returs enables/disables the plug-ins.
 *
 * @param settings settings object to set the plug-ins
*
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_plugins_enabled_get(const Ewk_Settings *settings);

/**
 * Requests setting of auto fit.
 *
 * @param settings settings object to fit to width
 * @param enable to fit to width.
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_auto_fitting_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns the auto fit status.
 *
 * @param settings settings object to fit to width
 *
 * @return @c EINA_TRUE if enable auto fit or @c EINA_FALSE.
 */
EAPI Eina_Bool ewk_settings_auto_fitting_get(const Ewk_Settings *settings);

/**
 * Requests setting of force zoom.
 *
 * @param settings settings object to enable force zoom
 * @param enable to force zoom
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_force_zoom_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns the force zoom status.
 *
 * @param settings settings object to enable force zoom
 *
 * @return @c EINA_TRUE if enable force zoom or @c EINA_FALSE.
 */
EAPI Eina_Bool ewk_settings_force_zoom_get(const Ewk_Settings *settings);

/**
 * Requests to set the default font size.
 *
 * @param settings settings object to set the default font size
 * @param size a new default font size to set
 *
 * @return @c EINA_TRUE on success @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_settings_font_default_size_set(Ewk_Settings *settings, int size);

/**
 * Returns the default font size.
 *
 * @param settings settings object to set the default font size
 *
 * @return @c default font size.
 */
EAPI int ewk_settings_font_default_size_get(const Ewk_Settings *settings);

/**
 * Requests enables/disables if the scripts can open the new windows.
 *
 * @param settings settings object to set if the scripts can open the new windows
 * @param allow @c EINA_TRUE if the scripts can open the new windows
 *        @c EINA_FALSE if not
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure (scripts are disabled)
 */
EAPI Eina_Bool ewk_settings_scripts_window_open_set(Ewk_Settings *settings, Eina_Bool allow);

/**
 * Returns enables/disables if the scripts can open the new windows.
 *
 * @param settings settings object to set if the scripts can open the new windows
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure (scripts are disabled)
 */
EAPI Eina_Bool ewk_settings_scripts_window_open_get(const Ewk_Settings *settings);

/**
 * Requests for drawing layer borders.
 *
 * @param settings settings object to drawing layer borders.
 * @param enable EINA_TRUE to draw layer borders.
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */

EAPI Eina_Bool ewk_settings_compositing_borders_visible_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Checks whether WebKit supports the @a encoding.
 *
 * @param encoding the encoding string to check whether WebKit supports it
 *
 * @return @c EINA_TRUE if WebKit supports @a encoding or @c EINA_FALSE if not or
 *      on failure
 */
EAPI Eina_Bool ewk_settings_is_encoding_valid(const char* encoding);

/**
 * Requests to set default text encoding name.
 *
 * @param settings settings object to set default text encoding name
 * @param encoding default text encoding name
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_default_encoding_set(Ewk_Settings *settings, const char* encoding);

/**
 * Returns default text encoding name.
 *
 * @param settings settings object to query default text encoding nae
 *
 * @return default text encoding name
 */
EAPI const char* ewk_settings_default_encoding_get(const Ewk_Settings *settings);

/**
 * Requests enables/disables private browsing.
 *
 * @param settings settings object to set private browsing
 * @param enable @c EINA_TRUE to enable private browsing
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_private_browsing_enabled_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns enables/disables private browsing.
 *
 * @param settings settings object to query if private browsing was enabled
 *
 * @return @c EINA_TRUE if private browsing was enabled
 *         @c EINA_FALSE if not or on failure
 */
EAPI Eina_Bool ewk_settings_private_browsing_enabled_get(const Ewk_Settings *settings);

/**
 * Requests to set editable link behavior.
 *
 * @param settings settings object to set editable link behavior
 * @param behavior editable link behaviro
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_editable_link_behavior_set(Ewk_Settings *settings, Ewk_Editable_Link_Behavior behavior);

/**
 * Requests to set the load remote images enable/disable
 *
 * @param settings settings object to set load remote images
 *
 * @param loadRemoteImages @c EINA_TRUE to enable the load remote images
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_load_remote_images_set(Ewk_Settings *settings, Eina_Bool loadRemoteImages);
/**
 * Returns enable/disable the load remote images
 *
 * @param settings settings object to get editable link behavior
 *
 * @return @c EINA_TRUE on enable or @c EINA_FALSE on disable
 */
EAPI Eina_Bool ewk_settings_load_remote_images_get(const Ewk_Settings *settings);

/**
 * Sets link magnifier enabled.
 *
 * @param settings settings object
 * @param enabled link magnifier enabled
 */
EAPI void ewk_settings_link_magnifier_enabled_set(Ewk_Settings *settings, Eina_Bool enabled);

/**
 * Gets link magnifier enabled.
 *
 * @param settings settings object
 *
 * @return @c EINA_TRUE if link magnifier enabled, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_settings_link_magnifier_enabled_get(const Ewk_Settings *settings);

/**
 * Requests to enable/disable link effect
 *
 * @param settings settings object to enable/disable link effect
 *
 * @param linkEffectEnabled @c EINA_TRUE to enable the link effect
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_link_effect_enabled_set(Ewk_Settings *settings, Eina_Bool linkEffectEnabled);

/**
 * Returns enable/disable link effect
 *
 * @param settings settings object to get whether link effect is enabled or disabled
 *
 * @return @c EINA_TRUE on enable or @c EINA_FALSE on disable
 */
EAPI Eina_Bool ewk_settings_link_effect_enabled_get(const Ewk_Settings *settings);

/**
 * Requests to set the scan malware enable/disable.
 *
 * @param settings settings object to set scan malware
 *
 * @param scan_malware_enabled @c EINA_TRUE to enable the scan malware
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_scan_malware_enabled_set(Ewk_Settings *settings, Eina_Bool scan_malware_enabled);

/**
 * Requests to set the spdy enable/disable.
 *
 * @param settings settings object to set spdy on soup
 *
 * @param spdy_enabled @c EINA_TRUE to enable the spdy on soup
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_spdy_enabled_set(Ewk_Settings *settings, Eina_Bool spdy_enabled);

/**
 * Requests to set the performance features of soup enable/disable.
 *
 * @param settings settings object to set performance features on soup
 *
 * @param spdy_enabled @c EINA_TRUE to enable the performance features on soup
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_performance_features_enabled_set(Ewk_Settings *settings, Eina_Bool performance_features_enabled);

/**
 * Requests to set using encoding detector.
 *
 * @param settings settings object to set using encoding detector
 * @param use use encoding detector
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_uses_encoding_detector_set(Ewk_Settings *settings, Eina_Bool use);

/**
 * Returns uses encoding detector.
 *
 * @param settings settings object to query uses encoding detector
 *
 * @return uses encoding detector
 */
EAPI Eina_Bool ewk_settings_uses_encoding_detector_get(const Ewk_Settings *settings);

/**
 * Requests to set using default keypad (default value : true)
 *
 * @param settings settings object to use default keypad
 * @param enable @c EINA_TRUE to use default keypad  @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_default_keypad_enabled_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns enable/disable using default keypad
 *
 * @param settings settings object to use default keypad
 *
 * @return @c EINA_TRUE on enable or @c EINA_FALSE on disable
 */
EAPI Eina_Bool ewk_settings_default_keypad_enabled_get(const Ewk_Settings *settings);

/**
 * Requests to set using keypad without user action (default value : true)
 *
 * @param settings settings object using keypad without user action
 * @param enable @c EINA_TRUE to use without user action @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_uses_keypad_without_user_action_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns using keypad without user action
 *
 * @param settings settings object using keypad without user action
 * @param settings settings object to query using keypad without user action
 *
 * @return @c EINA_TRUE on enable or @c EINA_FALSE on disable
 */
EAPI Eina_Bool ewk_settings_uses_keypad_without_user_action_get(const Ewk_Settings *settings);


/**
 * Requests setting use of text zoom.
 *
 * @param settings settings object to text zoom
 * @param enable to text zoom.
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_text_zoom_enabled_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns whether text zoom is enabled or not.
 *
 * @param settings settings object to text zoom
 *
 * @return @c EINA_TRUE if enable text zoom or @c EINA_FALSE.
 */
EAPI Eina_Bool ewk_settings_text_zoom_enabled_get(const Ewk_Settings *settings);

/**
 * Requests enable/disable password form autofill
 *
 * @param setting setting object to set password form autofill
 * @param enable @c EINA_TRUE to enable password form autofill
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_autofill_password_form_enabled_set(Ewk_Settings* settings, Eina_Bool enable);

/**
 * Returns if password form autofill is enabled or disabled.
 *
 * @param setting setting object to get password form autofill
 *
 * @return @c EINA_TRUE if password form autofill is enabled
 *         @c EINA_FALSE if password form autofill is disabled
 */
EAPI Eina_Bool ewk_settings_autofill_password_form_enabled_get(Ewk_Settings* settings);

/**
 * Requests enable/disable form candidate data for autofill
 *
 * @param setting setting object to set form candidate data for autofill
 * @param enable @c EINA_TRUE to enable form candidate data for autofill
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_form_candidate_data_enabled_set(Ewk_Settings* settings, Eina_Bool enable);

/**
 * Returns if form candidate data for autofill is enabled or disabled.
 *
 * @param setting setting object to get form candidate data for autofill
 *
 * @return @c EINA_TRUE if form candidate data for autofill is enabled
 *         @c EINA_FALSE if form candidate data for autofill is disabled
 */
EAPI Eina_Bool ewk_settings_form_candidate_data_enabled_get(Ewk_Settings* settings);

/**
 * Enables/disables form autofill profile feature.
 *
 * By default, form autofill profile is disabled.
 *
 * @param settings settings object to set the form autofill profile
 * @param enable @c EINA_TRUE to enable the text autosizing
 *               @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 *
 * @see ewk_settings_form_profile_data_enabled_get()
 */
EAPI Eina_Bool ewk_settings_form_profile_data_enabled_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns whether the autofill_text feature is enabled.
 *
 * @param settings settings object to query whether autofill_text feature is enabled
 *
 * @return @c EINA_TRUE if the autofill_text feature is enabled
 *         @c EINA_FALSE if not or on failure
 */
EAPI Eina_Bool ewk_settings_form_profile_data_enabled_get(const Ewk_Settings *settings);

/**
 * Requests enable/disable text selection by default WebKit.
 *
 * @param settings setting object to set text selection by default WebKit
 * @param enable @c EINA_TRUE to enable text selection by default WebKit
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_text_selection_enabled_set(Ewk_Settings* settings, Eina_Bool enable);

/**
 * Returns if text selection by default WebKit is enabled or disabled.
 *
 * @param settings setting object to get text selection by default WebKit
 *
 * @return @c EINA_TRUE if text selection by default WebKit is enabled
 *         @c EINA_FALSE if not or on failure
 */
EAPI Eina_Bool ewk_settings_text_selection_enabled_get(const Ewk_Settings* settings);

/**
 * Requests enables/disables to clear text selection when webview lose focus
 *
 * @param settings setting object to set to clear text selection when webview lose focus
 * @param enable @c EINA_TRUE to clear text selection when webview lose focus
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_clear_text_selection_automatically_set(Ewk_Settings* settings, Eina_Bool enable);

/**
 * Returns whether text selection is cleared when webview lose focus or not.
 *
 * @param settings setting object to get whether text selection is cleared when webview lose focus or not
 *
 * @return @c EINA_TRUE if text selection is cleared when webview lose focus
 *         @c EINA_FALSE if not or on failure
 */
EAPI Eina_Bool ewk_settings_clear_text_selection_automatically_get(const Ewk_Settings* settings);

/**
 * Enables/disables text autosizing.
 *
 * By default, the text autosizing is disabled.
 *
 * @param settings settings object to set the text autosizing
 * @param enable @c EINA_TRUE to enable the text autosizing
 *               @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 *
 * @see ewk_settings_text_autosizing_enabled_get()
 */
EAPI Eina_Bool ewk_settings_text_autosizing_enabled_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Returns whether the text autosizing is enabled.
 *
 * The text autosizing is a feature which adjusts the font size of text in wide
 * columns, and makes text more legible.
 *
 * @param settings settings object to query whether text autosizing is enabled
 *
 * @return @c EINA_TRUE if the text autosizing is enabled
 *         @c EINA_FALSE if not or on failure
 */
EAPI Eina_Bool ewk_settings_text_autosizing_enabled_get(const Ewk_Settings *settings);


/**
 * Sets the scale factor for text autosizing.
 *
 * Default value is 1.0.
 *
 * @param settings settings object to set the text autosizing
 * @param factor font scale factor for text autosizing
 */
EAPI Eina_Bool ewk_settings_text_autosizing_font_scale_factor_set(Ewk_Settings *settings, double factor);

/**
 * Gets the current scale factor for text autosizing.
 *
 * @param settings settings object to set scale factor for text autosizing
 *
 * @return the current font scale factor for text autosizing
 */
EAPI double ewk_settings_text_autosizing_font_scale_factor_get(const Ewk_Settings *settings);

/**
 * Requests to enable/disable edge effect
 *
 * @param settings settings object to enable/disable edge effect
 *
 * @param enable @c EINA_TRUE to enable the edge effect
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_edge_effect_enabled_set(Ewk_Settings* settings, Eina_Bool enable);

/**
 * Returns enable/disable edge effect
 *
 * @param settings settings object to get whether edge effect is enabled or disabled
 *
 * @return @c EINA_TRUE on enable or @c EINA_FALSE on disable
 */
EAPI Eina_Bool ewk_settings_edge_effect_enabled_get(const Ewk_Settings* settings);

/**
 * Sets text style for selection mode enabled.
 *
 * @param settings settings object
 * @param enabled text style for selection mode
 */

EAPI void ewk_settings_text_style_state_enabled_set(Ewk_Settings *settings, Eina_Bool enabled);
/**
 * Gets text style for selection mode enabled.
 *
 * @param settings settings object
 *
 * @return @c EINA_TRUE if text style for selection mode enabled, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_settings_text_style_state_enabled_get(const Ewk_Settings *settings);

/**
 * Requests to enable/disable to select word by double tap
 *
 * @param settings settings object to enable/disable to select word by double tap
 * @param enable @c EINA_TRUE to select word by double tap
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_select_word_automatically_set(Ewk_Settings *settings, Eina_Bool enabled);

/**
 * Returns enable/disable text selection by double tap
 *
 * @param settings settings object to get whether word by double tap is selected
 *
 * @return @c EINA_TRUE on enable or @c EINA_FALSE on disable
 */
EAPI Eina_Bool ewk_settings_select_word_automatically_get(const Ewk_Settings *settings);

/**
 * Sets legacy font size mode
 *
 * @param settings settings object
 * @param mode legacy font size mode
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_current_legacy_font_size_mode_set(Ewk_Settings *settings, Ewk_Legacy_Font_Size_Mode mode);

/**
 * Returns set legacy font size mode
 *
 * @param settings settings object
 *
 * @return @c Ewk_Legacy_Font_Size_Mode set legacy font size mode
 */
EAPI Ewk_Legacy_Font_Size_Mode ewk_settings_current_legacy_font_size_mode_get(const Ewk_Settings *settings);

/**
 * Sets to paste image as URI (default: paste as base64-encoded-data)
 *
 * @param settings settings object
* @param enable @c EINA_TRUE to paste image as URI    @c EINA_FALSE to paste image as data
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_paste_image_uri_mode_set(Ewk_Settings *settings, Eina_Bool enabled);

/**
 * Returns whether  paste image as URI mode is enabled
 *
 * @param settings settings object
 *
 * @return @c EINA_TRUE on enable or @c EINA_FALSE on disable
 */
EAPI Eina_Bool ewk_settings_paste_image_uri_mode_get(const Ewk_Settings *settings);

/**
 * Gets the initial position value for the HTML list element <ul></ul>.
 *
 * @param settings setting object to get the initial position value
 *
 * @return the initial position value for the HTML list element.
 */
EAPI Ewk_List_Style_Position ewk_settings_initial_list_style_position_get(const Ewk_Settings* settings);

/**
 * Sets the initial position value for the HTML list element <ul></ul>.
 *
 * This value affect the lists that are going to be created,
 * does not make sense to manipulate it for existed elements.
 *
 * @param settings setting object to set the initial list style position
 * @param style a new style to set
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_initial_list_style_position_set(Ewk_Settings* settings, Ewk_List_Style_Position style);

/**
 * Enable or disable supporting of -webkit-text-size-adjust
 *
 * -webkit-text-size-adjust affects text size adjusting feature.
 *
 * @param settings setting object to set the support of -webkit-text-size-adjust
 * @param enable @c EINA_TRUE to support -webkit-text-size-adjust, @c EINA_FALSE not to support
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_webkit_text_size_adjust_enabled_set(Ewk_Settings* settings, Eina_Bool enabled);

/**
 * Requests to enable/disable to detect email address when tapping on email address without link property
 *
 * @param settings settings object to enable/disable to detect email address when tapping on email address without link property
 *
 * @param enable @c EINA_TRUE to enable to detect email address when tapping on email address without link property
 *        @c EINA_FALSE to disable
 */
EAPI void ewk_settings_detect_contents_automatically_set(Ewk_Settings* settings, Eina_Bool enable);

/**
 * Returns enable/disable to detect email address when tapping on email address without link property
 *
 * @param settings settings object to get whether email address is detected when tapping on email address without link property
 *
 * @return @c EINA_TRUE on enable or @c EINA_FALSE on disable
 */
EAPI Eina_Bool ewk_settings_detect_contents_automatically_get(const Ewk_Settings* settings);

/**
 * Sets cache builder mode enabled.
 *
 * @param settings settings object
 * @param enabled cache builder mode
 */
EAPI void ewk_settings_cache_builder_enabled_set(Ewk_Settings *settings, Eina_Bool enabled);

/**
 * Returns the default font size.
 *
 * @param settings settings object to get the default font size
 *
 * @return @c the default font size or @c 0 on failure
 */
EAPI int ewk_settings_default_font_size_get(const Ewk_Settings *settings);

/**
 * Sets the default font size.
 *
 * By default, the default font size is @c 16.
 *
 * @param settings settings object to set the default font size
 * @param size a new default font size to set
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_default_font_size_set(Ewk_Settings *settings, int size);

/**
 * Sets the default text encoding name.
 *
 * @param settings settings object to set default text encoding name
 * @param encoding default text encoding name
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_default_text_encoding_name_set(Ewk_Settings *settings, const char *encoding);

/**
 * Enables/disables the encoding detector.
 *
 * By default, the encoding detector is disabled.
 *
 * @param settings settings object to set the encoding detector
 * @param enable @c EINA_TRUE to enable the encoding detector,
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_settings_encoding_detector_enabled_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Enables/disables if the scripts can open new windows.
 *
 * By default, the scripts can open new windows.
 *
 * @param settings settings object to set if the scripts can open new windows
 * @param enable @c EINA_TRUE if the scripts can open new windows
 *        @c EINA_FALSE if not
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure (scripts are disabled)
 */
EAPI Eina_Bool ewk_settings_scripts_can_open_windows_set(Ewk_Settings *settings, Eina_Bool enable);

/**
 * Requests enables/disables to the specific extra feature
 *
 * @param settings setting object to enable/disable the specific extra feature
 * @param feature feature name
 * @param enable @c EINA_TRUE to enable the specific extra feature
 *        @c EINA_FALSE to disable
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI void ewk_settings_extra_feature_set(Ewk_Settings* settings, const char* feature, Eina_Bool enable);

/**
 * Returns enable/disable to the specific extra feature
 *
 * @param settings settings object to get whether the specific extra feature is enabled or not.
 * @param feature feature name
 *
 * @return @c EINA_TRUE on enable or @c EINA_FALSE on disable
 */
EAPI Eina_Bool ewk_settings_extra_feature_get(const Ewk_Settings* settings, const char* feature);

#ifdef __cplusplus
}
#endif
#endif // ewk_settings_h
