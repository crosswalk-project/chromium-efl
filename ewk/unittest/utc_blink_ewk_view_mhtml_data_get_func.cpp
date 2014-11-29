#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_mhtml_data_get : public utc_blink_ewk_base
{
protected:

  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }

  static void mhtmlDataGetCallback(Evas_Object *o, const char *data, void *user_data)
  {
    ASSERT_TRUE(user_data != NULL);
    ASSERT_TRUE(data != NULL);
    utc_blink_ewk_view_mhtml_data_get*const owner = static_cast<utc_blink_ewk_view_mhtml_data_get*>(user_data);
    bool const notEmpty = strlen(data);
    owner->EventLoopStop(notEmpty ? Success : Failure);
    if (notEmpty)
      utc_message("MHTML:\t\"%s\"", data);
  }
};

/**
 * @brief Tests if returns true when the mhtml data obtained properly.
 */
TEST_F(utc_blink_ewk_view_mhtml_data_get, POS_TEST)
{
  ASSERT_TRUE(ewk_view_html_string_load(GetEwkWebView(), "<p>Simple HTML</p>", 0, 0));
  // Wait until load finished.
  ASSERT_EQ(EventLoopStart(), Success);

  ASSERT_TRUE(ewk_view_mhtml_data_get(GetEwkWebView(), mhtmlDataGetCallback, this));
  // Wait until convert data.
  EXPECT_EQ(EventLoopStart(), Success);
}

/**
 * @brief Tests if returns false when the callback function is NULL or view is NULL.
 */
TEST_F(utc_blink_ewk_view_mhtml_data_get, NEG_TEST)
{
  ASSERT_TRUE(ewk_view_html_string_load(GetEwkWebView(), "<p>Simple HTML</p>", 0, 0));
  // Wait until load finished.
  ASSERT_EQ(EventLoopStart(), Success);

  EXPECT_FALSE(ewk_view_mhtml_data_get(GetEwkWebView(), NULL, this));
  EXPECT_FALSE(ewk_view_mhtml_data_get(NULL, mhtmlDataGetCallback, this));
}
