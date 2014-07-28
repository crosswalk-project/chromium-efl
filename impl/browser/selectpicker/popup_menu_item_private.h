/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */



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
