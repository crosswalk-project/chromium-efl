// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "popup_controller_efl.h"
#include "components/clipboard/clipboard_helper_efl.h"
#include <Eina.h>
#include <Elementary.h>
#include <Ecore_X.h>

#ifdef TIZEN_CONTENTS_DETECTION

namespace content {

static void item_selected_cb(void *data, Evas_Object *obj, void *event_info) {
  PopupControllerEfl* controller = static_cast<PopupControllerEfl*>(data);
  std::string info = elm_object_item_text_get(static_cast<Elm_Object_Item*>(event_info));

  controller->closePopup();

  service_h svcHandle = 0;
  if (service_create(&svcHandle) < 0 || !svcHandle) {
    LOG(ERROR) << __PRETTY_FUNCTION__ << " : " << "could not create service.";
    return;
  }

  if (controller->popupContentType() == PopupControllerEfl::PHONE) {
    if (info == "Call") {
      service_set_operation(svcHandle, SERVICE_OPERATION_DIAL);
      service_set_uri(svcHandle, controller->fullContent().c_str());
    } else if (info == "Send message") {
      service_set_operation(svcHandle, SERVICE_OPERATION_COMPOSE);
      service_add_extra_data(svcHandle, "http://tizen.org/appcontrol/data/messagetype", "sms");
      service_add_extra_data(svcHandle, SERVICE_DATA_TO, controller->content().c_str());
      service_add_extra_data(svcHandle, "http://tizen.org/appcontrol/data/return_result", "false");
      service_set_app_id(svcHandle, "msg-composer-efl");
    } else if (info == "Add to contacts") {
      service_set_operation(svcHandle, "http://tizen.org/appcontrol/operation/social/add");
      service_add_extra_data(svcHandle, "http://tizen.org/appcontrol/data/social/item_type", "contact");
      service_add_extra_data(svcHandle, "http://tizen.org/appcontrol/data/social/phone", controller->content().c_str());
      service_set_app_id(svcHandle, "contacts-details-efl");
    }
  } else if (controller->popupContentType() == PopupControllerEfl::EMAIL) {
    if (info == "Send email") {
      service_set_operation(svcHandle, SERVICE_OPERATION_COMPOSE);
      service_add_extra_data(svcHandle, SERVICE_DATA_TO, controller->content().c_str());
      service_set_app_id(svcHandle, "email-composer-efl");
    } else if (info == "Add to contacts") {
      service_set_operation(svcHandle, "http://tizen.org/appcontrol/operation/social/add");
      service_add_extra_data(svcHandle, "http://tizen.org/appcontrol/data/social/item_type", "contact");
      service_add_extra_data(svcHandle, "http://tizen.org/appcontrol/data/social/email", controller->content().c_str());
      service_set_app_id(svcHandle, "contacts-details-efl");
    }
  }

  int ret = service_send_launch_request(svcHandle, 0, 0);
  if (ret != SERVICE_ERROR_NONE) {
    LOG(ERROR) << __PRETTY_FUNCTION__ << " : " << "could not launch application.";
    service_destroy(svcHandle);
    return;
  }
  service_destroy(svcHandle);
}

static void copy_cb(void *data, Evas_Object *obj, void *event_info) {
  PopupControllerEfl* controller = static_cast<PopupControllerEfl*>(data);

  ClipboardHelperEfl::GetInstance()->SetData(controller->content(), ClipboardHelperEfl::CLIPBOARD_DATA_TYPE_PLAIN_TEXT);

  controller->closePopup();
}

static void block_clicked_cb(void *data, Evas_Object *obj, void *event_info) {
  evas_object_hide(static_cast<Evas_Object*>(data));
}

PopupControllerEfl::PopupControllerEfl(EWebView* webView)
  : web_view_(webView),
    popup_(0),
    content_type_(UNKNOWN) {
}

void PopupControllerEfl::openPopup(const char* message) {
  if (!message)
    return;

  popup_ = elm_popup_add(web_view_->evas_object());
  if (!popup_)
    return;

  full_content_ = std::string(message);

  if (content_.find("mailto:") != std::string::npos) {
    content_.erase(0, 7);
    content_type_ = EMAIL;
  } else if (content_.find("tel:") != std::string::npos) {
    content_.erase(0, 4);
    content_type_ = PHONE;
  } else
    return;

  elm_object_part_text_set(popup_, "title,text", const_cast<char*>(content_.c_str()));

  Evas_Object* layout = elm_layout_add(popup_);
  if (!layout)
    return;

  // Set popup_ default layout
  elm_layout_theme_set(layout, "layout", "application", "default");
  evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(layout);
  elm_object_content_set(popup_, layout);

  // Append popup_ items
  if (content_type_ == EMAIL) {
    elm_popup_item_append(popup_, "Send email", NULL, item_selected_cb, this);
    elm_popup_item_append(popup_, "Add to contacts", NULL, item_selected_cb, this);
    elm_popup_item_append(popup_, "Copy", NULL, copy_cb, this);
  } else if (content_type_ == PHONE) {
    elm_popup_item_append(popup_, "Call", NULL, item_selected_cb, this);
    elm_popup_item_append(popup_, "Send message", NULL, item_selected_cb, this);
    elm_popup_item_append(popup_, "Add to contacts", NULL, item_selected_cb, this);
    elm_popup_item_append(popup_, "Copy", NULL, copy_cb, this);
  }

  // Close popup after tapping on Block Event area
  evas_object_smart_callback_add(popup_, "block,clicked", block_clicked_cb, popup_);

  // Resize and show popup
  evas_object_resize(popup_, 400, 400);
  evas_object_show(popup_);
}

void PopupControllerEfl::closePopup() {
  if (popup_)
    evas_object_hide(popup_);
}

} //namespace
#endif //TIZEN_CONTENTS_DETECTION
