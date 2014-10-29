// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_contents_pdf_get : public utc_blink_ewk_base
{
 protected:
  bool file_exists;
  void PreSetUp()
  {
    file_exists=false;
    evas_object_smart_callback_add(GetEwkWebView(), "frame,rendered", FrameRendered, this);
  }

  void PostTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "frame,rendered", FrameRendered);
  }

  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop( Success );
  }

  static void FrameRendered(void* data, Evas_Object* obj, void* event_info)
  {
    utc_blink_ewk_view_contents_pdf_get*owner=static_cast<utc_blink_ewk_view_contents_pdf_get*>(data);
    utc_message("[Frame rendered] ::");
    if(owner)
      owner->EventLoopStop( Success );
  }

  static Eina_Bool VerifyFile(void* data)
  {
    if(data)
    {
      utc_blink_ewk_view_contents_pdf_get*owner=static_cast<utc_blink_ewk_view_contents_pdf_get*>(data);
      owner->file_exists = ecore_file_exists("/tmp/sample.pdf");
      owner->EventLoopStop( Success );
    }
    return ECORE_CALLBACK_CANCEL;
  }
};

/**
 * @brief Positive test case of ewk_view_contents_pdf_get().
 * Page is loaded and API is called to save pdf file.
 * Then existence of file is verified by 3 second timer.
 */
TEST_F(utc_blink_ewk_view_contents_pdf_get, POS_TEST)
{
  bool result = ewk_view_url_set(GetEwkWebView(),"http://www.google.com");
  if (!result)
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  Evas_Coord width = 0;
  Evas_Coord height = 0;
  result = ewk_view_contents_size_get(GetEwkWebView(), &width, &height);
  if(!result || !width || !height)
    utc_fail();

  result = ewk_view_contents_pdf_get(GetEwkWebView(), width, height, "/tmp/sample.pdf");
  if (!result)
    utc_fail();
 // waiting for 3 seconds to pdf printer to work.
  ecore_timer_add(3, VerifyFile, this);
  EventLoopStart();

  utc_check_true(file_exists);
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_contents_pdf_get, NEG_TEST)
{
  bool result = ewk_view_contents_pdf_get(NULL, 200, 200, "/tmp/sample.pdf");
  utc_check_false(result);
}
