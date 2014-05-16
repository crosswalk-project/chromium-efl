#include "base/logging.h"
#include "popup_controller_efl.h"
#include <Eina.h>
#include <Elementary.h>
#include <Ecore_X.h>

#ifdef TIZEN_CONTENTS_DETECTION

namespace content {

static void item_selected_cb(void *data, Evas_Object *obj, void *event_info) {
  LOG(INFO) << __PRETTY_FUNCTION__ << " : " << elm_object_item_text_get(static_cast<Elm_Object_Item*>(event_info))
    << " content: " << static_cast<PopupControllerEfl*>(data)->content().c_str()
    << " type: " << static_cast<PopupControllerEfl*>(data)->popupContentType();

  // NOT IMPLEMENTED

  static_cast<PopupControllerEfl*>(data)->closePopup();
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

  content_ = std::string(message);

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
    elm_popup_item_append(popup_, "Copy", NULL, item_selected_cb, this);
  } else if (content_type_ == PHONE) {
    elm_popup_item_append(popup_, "Call", NULL, item_selected_cb, this);
    elm_popup_item_append(popup_, "Send message", NULL, item_selected_cb, this);
    elm_popup_item_append(popup_, "Add to contacts", NULL, item_selected_cb, this);
    elm_popup_item_append(popup_, "Copy", NULL, item_selected_cb, this);
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
