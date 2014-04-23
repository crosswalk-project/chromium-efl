/*
 * Copyright (C) 2014 Samsung Electronics
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#ifndef IM_CONTEXT_EFL
#define IM_CONTEXT_EFL

#include "ui/base/ime/text_input_mode.h"
#include "ui/base/ime/text_input_type.h"
#include "ui/base/ime/composition_text.h"

#include <Evas.h>

namespace gfx {
class Rect;
}

typedef struct _Ecore_IMF_Context Ecore_IMF_Context;

namespace content {

class RenderWidgetHostViewEfl;

class IMContextEfl {
 public:
  // Returns NULL if there is no available backend.
  static IMContextEfl* Create(RenderWidgetHostViewEfl*);

  ~IMContextEfl();

  void Reset();
  void HandleKeyDownEvent(const Evas_Event_Key_Down* event, bool* wasFiltered);
  void HandleKeyUpEvent(const Evas_Event_Key_Up* event, bool* wasFiltered);

  void UpdateInputMethodState(ui::TextInputType type, bool can_compose_inline, ui::TextInputMode input_mode,
                              bool is_user_action = false);
  void UpdateInputMethodState(ui::TextInputType type);
  void UpdateCaretBounds(const gfx::Rect& caret_bounds);

  void OnFocusIn();
  void OnFocusOut();

  void CancelComposition();
  void ConfirmComposition();

 private:
  IMContextEfl(RenderWidgetHostViewEfl*, Ecore_IMF_Context*);

  void InitializeIMFContext(Ecore_IMF_Context*);
  void ResetIMFContext();

  void ShowPanel(ui::TextInputType, ui::TextInputMode, bool is_user_action = false);
  void HidePanel();

  // callbacks
  static void IMFCommitCallback(void* data, Ecore_IMF_Context*, void* event_info) { reinterpret_cast<IMContextEfl*>(data)->OnCommit(event_info); }
  static void IMFPreeditChangedCallback(void* data, Ecore_IMF_Context* context, void* event_info) { reinterpret_cast<IMContextEfl*>(data)->OnPreeditChanged(data, context, event_info); }

  static void IMFInputPanelStateChangedCallback(void* data, Ecore_IMF_Context*, int state) { reinterpret_cast<IMContextEfl*>(data)->OnInputPanelStateChanged(state); }
  static void IMFInputPanelGeometryChangedCallback(void* data, Ecore_IMF_Context*, int state) { reinterpret_cast<IMContextEfl*>(data)->OnInputPanelGeometryChanged(); }

  static void IMFCandidatePanelStateChangedCallback(void* data, Ecore_IMF_Context*, int state) { reinterpret_cast<IMContextEfl*>(data)->OnCandidateInputPanelStateChanged(state); }
  static void IMFCandidatePanelGeometryChangedCallback(void* data, Ecore_IMF_Context*, int state) { reinterpret_cast<IMContextEfl*>(data)->OnCandidateInputPanelGeometryChanged(); }

  static Eina_Bool IMFRetrieveSurroundingCallback(void* data, Ecore_IMF_Context*, char** text, int* offset) { return reinterpret_cast<IMContextEfl*>(data)->OnRetrieveSurrounding(text, offset); }
  static void IMFDeleteSurroundingCallback(void* data, Ecore_IMF_Context*, void* event_info) { reinterpret_cast<IMContextEfl*>(data)->OnDeleteSurrounding(event_info); }
  static void IMFCandidatePanelLanguageChangedCallback(void* data, Ecore_IMF_Context* context, int value) { reinterpret_cast<IMContextEfl*>(data)->OnCandidateInputPanelLanguageChanged(context, value); }
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

  RenderWidgetHostViewEfl* view_;

  Ecore_IMF_Context* context_;

  // Whether or not the accociated widget is focused.
  bool focused_;

  // Whether or not the IME is enabled.
  bool enabled_;

  // Whether ShowPanel was ever called.
  // At the first time when we show the input panel we can use the initial context which we create
  // in order to check that input method is supported on the platform. Later on we need to reset context
  // before showing the panel to be able to switch between different layouts.
  bool panel_was_ever_shown_;
  ui::CompositionText composition_;
};

} // namespace content

#endif
