// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_extra_feature_set : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_settings_extra_feature_set():
    settings(NULL)
  {
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(Success);
  }

  void PostSetUp()
  {
    // load file just to initialize everything
    ewk_view_url_set(GetEwkWebView(), GetResourceUrl("/common/sample.html").c_str());
    ASSERT_EQ(Success, EventLoopStart());
    settings = ewk_view_settings_get(GetEwkWebView());
    ASSERT_TRUE(settings);
  }

protected:
  Ewk_Settings* settings;
};

TEST_F(utc_blink_ewk_settings_extra_feature_set, EXISTING_FEATURES)
{
  static const char* features[] = {
    "longpress,enable",
    "link,magnifier",
    "detect,contents",
    "web,login",
    "doubletap,enable",
    "zoom,enable",
    "openpanel,enable",
    "allow,restrictedurl",
    "urlbar,hide",
    NULL};

  for (const char** feat = features; *feat; feat++) {
    Eina_Bool defaultValue = ewk_settings_extra_feature_get(settings, *feat);
    Eina_Bool newValue = defaultValue == EINA_TRUE ? EINA_FALSE : EINA_TRUE;
    ewk_settings_extra_feature_set(settings, *feat, newValue);
    ASSERT_EQ(newValue, ewk_settings_extra_feature_get(settings, *feat)) << *feat << " value did not change!";
  }
}

TEST_F(utc_blink_ewk_settings_extra_feature_set, NON_EXISTING_FEATURE)
{
  static const char* feat = "nonexistent,feature";
  ASSERT_EQ(EINA_FALSE, ewk_settings_extra_feature_get(settings, feat));
  ewk_settings_extra_feature_set(settings, feat, EINA_TRUE);
  ASSERT_EQ(EINA_FALSE, ewk_settings_extra_feature_get(settings, feat));
}

TEST_F(utc_blink_ewk_settings_extra_feature_set, INVALID_ARGS)
{
  // The only thing we can do is expect that it won't crash
  ewk_settings_extra_feature_set(NULL, NULL, EINA_TRUE);
  ewk_settings_extra_feature_set(settings, NULL, EINA_TRUE);
  ewk_settings_extra_feature_set(NULL, "invalid", EINA_TRUE);
}
