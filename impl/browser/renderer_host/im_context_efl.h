// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IM_CONTEXT_EFL
#define IM_CONTEXT_EFL

#include "ui/base/ime/text_input_mode.h"
#include "ui/base/ime/text_input_type.h"
#include "ui/base/ime/composition_text.h"
#include "ui/gfx/rect.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "browser/renderer_host/web_event_factory_efl.h"

#include <Evas.h>

typedef std::queue<base::string16> CommitQueue;
typedef std::queue<ui::CompositionText> PreeditQueue;

typedef struct _Ecore_IMF_Context Ecore_IMF_Context;

namespace content {

class RenderWidgetHostViewEfl;

class IMContextEfl {
 public:
  // Returns NULL if there is no available backend.
  static IMContextEfl* Create(RenderWidgetHostViewEfl*);

  ~IMContextEfl();

  void HandleKeyDownEvent(const Evas_Event_Key_Down* event, bool* wasFiltered);
  void HandleKeyUpEvent(const Evas_Event_Key_Up* event, bool* wasFiltered);

  void UpdateInputMethodType(ui::TextInputType, ui::TextInputMode,
                             bool can_compose_inline);
  void UpdateInputMethodState(ui::TextInputType, bool can_compose_inline,
                              bool show_if_needed);
  void UpdateCaretBounds(const gfx::Rect& caret_bounds);

  void OnFocusIn();
  void OnFocusOut();

  void ShowPanel();
  void HidePanel();

  void ResetIMFContext();
  void CancelComposition();
  void ConfirmComposition();
  void SetIsInFormTag(bool is_in_form_tag);
  bool IsShow();
  gfx::Rect GetIMERect() const { return ime_rect_; }
  void SetIMERect(const gfx::Rect& rect) { ime_rect_ = rect; }
  CommitQueue GetCommitQueue() { return commit_queue_; }
  PreeditQueue GetPreeditQueue() { return preedit_queue_; }
  void CommitQueuePop() { commit_queue_.pop(); }
  void PreeditQueuePop() { preedit_queue_.pop(); }
  void ClearQueues();

  void SendFakeCompositionKeyEvent(const base::string16& buf);

 private:
  IMContextEfl(RenderWidgetHostViewEfl*, Ecore_IMF_Context*);

  void InitializeIMFContext(Ecore_IMF_Context*);

  // callbacks
  static void IMFCommitCallback(void* data, Ecore_IMF_Context*, void* event_info);
  static void IMFPreeditChangedCallback(void* data, Ecore_IMF_Context* context, void* event_info);
  static void IMFInputPanelStateChangedCallback(void* data, Ecore_IMF_Context*, int state);
  static void IMFInputPanelGeometryChangedCallback(void* data, Ecore_IMF_Context*, int state);
  static void IMFCandidatePanelStateChangedCallback(void* data, Ecore_IMF_Context*, int state);
  static void IMFCandidatePanelGeometryChangedCallback(void* data, Ecore_IMF_Context*, int state);
  static Eina_Bool IMFRetrieveSurroundingCallback(void* data, Ecore_IMF_Context*, char** text, int* offset);
  static void IMFDeleteSurroundingCallback(void* data, Ecore_IMF_Context*, void* event_info);
  static void IMFCandidatePanelLanguageChangedCallback(void* data, Ecore_IMF_Context* context, int value);

  // callback handlers
  void OnCommit(void* event_info);
  void OnPreeditChanged(void* data, Ecore_IMF_Context* context, void* event_info);

  void OnInputPanelStateChanged(int state);
  void OnInputPanelGeometryChanged();

  void OnCandidateInputPanelStateChanged(int state);
  void OnCandidateInputPanelGeometryChanged();

  bool OnRetrieveSurrounding(char** text, int* offset);
  void OnDeleteSurrounding(void* event_info);
  void OnCandidateInputPanelLanguageChanged(Ecore_IMF_Context* context, int value);

  RenderWidgetHostImpl* GetRenderWidgetHostImpl() const;

  RenderWidgetHostViewEfl* view_;

  Ecore_IMF_Context* context_;

  // Whether or not the associated widget is focused.
  bool is_focused_;

  // Is software keyboard currently visible
  bool is_ime_panel_visible_;

  // Whether or not is in form tag.
  bool is_in_form_tag_;

  // The current soft keyboard mode and layout
  ui::TextInputMode current_mode_;
  ui::TextInputType current_type_;
  bool can_compose_inline_;

  ui::CompositionText composition_;

  gfx::Rect ime_rect_;

  CommitQueue commit_queue_;
  PreeditQueue preedit_queue_;

  bool is_handling_keydown_;
  bool is_ime_ctx_reset_;
};

} // namespace content

#endif
