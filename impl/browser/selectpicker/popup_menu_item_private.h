// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef popup_menu_item_private_h
#define popup_menu_item_private_h

#include "config.h"
#include "WebPopupItem.h"
#include "popup_menu_item.h"
#include "base/memory/scoped_ptr.h"
#include "third_party/WebKit/Source/platform/text/TextDirection.h"

#include <string>
#include <wtf/PassOwnPtr.h>

/**
 * \struct  Popup_Menu_Item
 * @brief   Contains the popup menu data.
 */
class Popup_Menu_Item {
public:
    Popup_Menu_Item_Type type;
    blink::TextDirection textDirection;

    bool hasTextDirectionOverride;
    bool isEnabled;
    bool isLabel;
    bool isSelected;

    std::string text;
    std::string toolTip;
    std::string accessibilityText;

    static PassOwnPtr<Popup_Menu_Item> create(const blink::WebPopupItem& item)
    {
      return adoptPtr(new Popup_Menu_Item(item));
    }

private:
    explicit Popup_Menu_Item(const blink::WebPopupItem& item);
};

#endif // popup_menu_item_private_h
