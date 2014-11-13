// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/shell.h"

#include <Evas.h>
#include <Ecore_Evas.h>
#include <Elementary.h>

#include "base/command_line.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "content/public/browser/render_view_host.h"
#include "ui/gl/gl_switches.h"
#include "cc/base/switches.h"

namespace content {

Evas_Object* Shell::native_view_ = NULL;
Shell::Impl* Shell::impl_ = NULL;

class Shell::Impl {
 public:
  Impl(gfx::Size size)
      : url_bar_(NULL), web_view_(NULL)
      , refresh_btn_(NULL), stop_btn_(NULL), back_btn_(NULL)
      , forward_btn_(NULL), exit_handler_(NULL), shell_(NULL) {
    Evas_Object *icon = NULL;

    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
    elm_config_preferred_engine_set("opengl_x11");
    window_ = elm_win_util_standard_add(
        "content_shell_efl", "Content Shell EFL");
    elm_win_autodel_set(window_, EINA_TRUE);
    evas_object_resize(window_, size.width(), size.height());

    Evas_Object *box = elm_box_add(window_);
    evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    //elm_box_padding_set(box, 20, 20);
    elm_win_resize_object_add(window_, box);
    evas_object_show(box);

    url_bar_ = elm_entry_add(box);
    elm_entry_single_line_set(url_bar_, EINA_TRUE);
    elm_entry_scrollable_set(url_bar_, EINA_TRUE);
    elm_entry_input_panel_layout_set(url_bar_, ELM_INPUT_PANEL_LAYOUT_URL);
    evas_object_size_hint_weight_set(url_bar_, EVAS_HINT_EXPAND, 0.0f);
    evas_object_size_hint_align_set(url_bar_, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(url_bar_, "activated", OnURLEntered, this);
    elm_box_pack_end(box, url_bar_);
    evas_object_show(url_bar_);

    Evas_Object *button_box = elm_box_add(box);
    elm_box_horizontal_set(button_box, EINA_TRUE);

    back_btn_ = elm_button_add(button_box);
    icon = elm_icon_add(button_box);
    elm_icon_standard_set(icon, "arrow_left");
    elm_object_part_content_set(back_btn_, "icon", icon);
    evas_object_smart_callback_add(back_btn_, "clicked", OnBack, this);
    elm_box_pack_end(button_box, back_btn_);
    evas_object_show(back_btn_);

    forward_btn_ = elm_button_add(button_box);
    icon = elm_icon_add(button_box);
    elm_icon_standard_set(icon, "arrow_right");
    elm_object_part_content_set(forward_btn_, "icon", icon);
    evas_object_smart_callback_add(forward_btn_, "clicked", OnForward, this);
    elm_box_pack_end(button_box, forward_btn_);
    evas_object_show(forward_btn_);

    refresh_btn_ = elm_button_add(button_box);
    icon = elm_icon_add(button_box);
    elm_icon_standard_set(icon, "refresh");
    elm_object_part_content_set(refresh_btn_, "icon", icon);
    evas_object_smart_callback_add(refresh_btn_, "clicked", OnRefresh, this);
    elm_box_pack_end(button_box, refresh_btn_);
    evas_object_show(refresh_btn_);

    stop_btn_ = elm_button_add(button_box);
    icon = elm_icon_add(button_box);
    elm_icon_standard_set(icon, "close");
    elm_object_part_content_set(stop_btn_, "icon", icon);
    evas_object_smart_callback_add(stop_btn_, "clicked", OnStop, this);
    elm_box_pack_end(button_box, stop_btn_);
    evas_object_show(stop_btn_);

#if !defined(OS_TIZEN)
    elm_object_text_set(back_btn_, "Back");
    elm_object_text_set(forward_btn_, "Forward");
    elm_object_text_set(refresh_btn_, "Refresh");
    elm_object_text_set(stop_btn_, "Stop");
#endif

    evas_object_show(button_box);
    elm_object_part_content_set(url_bar_, "icon", button_box);

    Evas* evas = evas_object_evas_get(window_);
    web_view_ = evas_object_image_filled_add(evas);
    evas_object_image_size_set(web_view_, size.width(), size.height());
    evas_object_size_hint_align_set(web_view_, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(web_view_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_box_pack_end(box, web_view_);
    evas_object_show(web_view_);

    evas_object_event_callback_add(web_view_, EVAS_CALLBACK_RESIZE, OnResize, this);
    exit_handler_ = ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, &OnExit, this);
    evas_object_smart_callback_add(window_, "delete,request", OnWindowDelRequest, this);
  }

  void SetAddressBarURL(const char* url) {
    // XXX: Check unnecessart when HW rendering works on desktop
    if (url_bar_) {
      elm_object_text_set(url_bar_, url);
    }
  }

  Evas_Object* GetWebView() const {
    return web_view_;
  }

  void SetShell(Shell* shell) {
    shell_ = shell;
  }

  void ShowWindow(int width, int height) {
    evas_object_resize(window_, width, height);
    evas_object_show(window_);
  }

  void SetWindowTitle(const base::string16& title) {
    elm_win_title_set(window_, UTF16ToUTF8(title).c_str());
  }

  void ToggleControl(UIControl control, bool is_enabled) {
    // XXX: After HW rendering is fixed on desktop elm buttons
    //      should always exist when entering this function
    if (control == BACK_BUTTON && back_btn_) {
      elm_object_disabled_set(back_btn_, !is_enabled);
    } else if (control == FORWARD_BUTTON && forward_btn_) {
      elm_object_disabled_set(forward_btn_, !is_enabled);
    } else if (control == STOP_BUTTON && stop_btn_) {
      elm_object_disabled_set(stop_btn_, !is_enabled);
    }
  }

 private:
  void Close() {
    elm_exit();
    shell_->Close();
  }

  static void OnResize(void* data, Evas*, Evas_Object*, void*) {
    Shell::Impl* thiz = static_cast<Shell::Impl*>(data);

    int width, height;
    evas_object_geometry_get(thiz->web_view_, 0, 0, &width, &height);
    evas_object_image_size_set(native_view_, width, height);

    if (thiz->shell_ && thiz->shell_->web_contents()) {
      thiz->shell_->web_contents()->GetRenderViewHost()->WasResized();
    }
  }

  static Eina_Bool OnExit(void* data, int, void*) {
    static_cast<Shell::Impl*>(data)->Close();
    return 1;
  }

  static void OnWindowDelRequest(void* data, Evas_Object*, void*) {
    Shell::Impl* thiz = static_cast<Shell::Impl*>(data);
    thiz->window_ = NULL;
    thiz->Close();
  }

  static void OnBack(void* data, Evas_Object*, void*) {
    static_cast<Shell::Impl*>(data)->shell_->GoBackOrForward(-1);
  }

  static void OnForward(void* data, Evas_Object*, void*) {
    static_cast<Shell::Impl*>(data)->shell_->GoBackOrForward(1);
  }

  static void OnRefresh(void* data, Evas_Object*, void*) {
    static_cast<Shell::Impl*>(data)->shell_->Reload();
  }

  static void OnStop(void* data, Evas_Object*, void*) {
    static_cast<Shell::Impl*>(data)->shell_->Stop();
  }

  static void OnURLEntered(void *data, Evas_Object *obj, void*) {
    GURL url(elm_object_text_get(obj));
    if (!url.has_scheme()) {
      url = GURL(std::string("http://") + std::string(elm_object_text_get(obj)));
      elm_object_text_set(obj, url.spec().c_str());
    }
    static_cast<Shell::Impl*>(data)->shell_->LoadURL(url);
  }

  Evas_Object* window_;
  Evas_Object* url_bar_;
  Evas_Object* web_view_;
  Evas_Object* refresh_btn_;
  Evas_Object* stop_btn_;
  Evas_Object* back_btn_;
  Evas_Object* forward_btn_;
  Ecore_Event_Handler* exit_handler_;

  Shell* shell_;
};

// static
void Shell::PlatformInitialize(const gfx::Size& default_window_size) {
  elm_init(0, NULL);

  impl_ = new Impl(default_window_size);
  native_view_ = impl_->GetWebView();

  // This is not the best place to append port specific flags, but
  // it works and keeps changes to upsteam sources minimal.
  //
  // XXX: Investigate why EFL port does not work without threaded
  //      compositing.
  CommandLine::ForCurrentProcess()->
      AppendSwitch(switches::kDisableDelegatedRenderer);
  CommandLine::ForCurrentProcess()->
      AppendSwitch(cc::switches::kCompositeToMailbox);

  LOG(INFO) << "EFL Shell platform initialized";
}

void Shell::PlatformExit() {
  native_view_ = NULL;
  LOG(INFO) << "EFL Shell platform exit";
}

void Shell::PlatformCleanUp() {
  delete impl_;
}

void Shell::PlatformEnableUIControl(UIControl control, bool is_enabled) {
  impl_->ToggleControl(control, is_enabled);
}

void Shell::PlatformSetAddressBarURL(const GURL& url) {
  impl_->SetAddressBarURL(url.spec().c_str());
}

void Shell::PlatformSetIsLoading(bool loading) {
}

void Shell::PlatformCreateWindow(int width, int height) {
  impl_->ShowWindow(width, height);
}

void Shell::PlatformSetContents() {
  impl_->SetShell(this);
}

void Shell::PlatformResizeSubViews() {
}

void Shell::Close() {
  LOG(INFO) << "Closing Content Shell EFL";
  delete this;
}

void Shell::PlatformSetTitle(const base::string16& title) {
  impl_->SetWindowTitle(title);
}

bool Shell::PlatformHandleContextMenu(
    const content::ContextMenuParams& params) {
  return false;
}

}  // namespace content
