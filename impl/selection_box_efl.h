// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef selection_box_efl_h
#define selection_box_efl_h

#include "base/memory/scoped_ptr.h"
#include "content/public/common/context_menu_params.h"
#include "content/public/common/menu_item.h"
#include "ui/gfx/range/range.h"
#include "ui/gfx/rect.h"

class EWebView;

namespace content {

// Hold the data related to drwaing the selection handlers
// and context menu. Also stores all the data required for selection
// controlling
class SelectionBoxEfl {
 public:
  SelectionBoxEfl(EWebView* parent_view);

  void SetStatus(bool enable) { status_ = enable; }
  bool GetStatus() const { return status_; }
  void SetEditable(bool enable) { GetContextMenuParams()->is_editable = editable_ = enable; }
  bool GetEditable() const { return editable_; }
  void UpdateSelectStringData(const base::string16& text);
  void UpdateRectData(const gfx::Rect& left_rect, const gfx::Rect& right_rect);
  void ClearRectData();
  bool IsInEditField() const;
  void SetCaretSelectionStatus(const bool enable) { is_caret_selection_ = enable; }
  bool GetCaretSelectionStatus() const { return is_caret_selection_; }
  gfx::Rect GetLeftRect() const { return left_rect_; }
  gfx::Rect GetRightRect() const { return right_rect_; }
  ContextMenuParams* GetContextMenuParams() const { return context_params_.get(); }

 private:
  // Save the state of selection, if active or not
  bool status_;

  // Save if the selection is in one of the editable fields
  bool editable_;

  // Caret is in a input field
  bool is_caret_selection_;

  // Start of selection
  gfx::Rect left_rect_;

  // End of selection
  gfx::Rect right_rect_;

  // Contains the menu item data for which context needs to be populated
  scoped_ptr<ContextMenuParams> context_params_;

  EWebView* parent_view_;
};

}
#endif
