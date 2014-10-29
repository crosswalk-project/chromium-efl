// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "popup_menu_item.h"
#include "popup_menu_item_private.h"
#include "WebPopupItem.h"
#include "base/logging.h"

Popup_Menu_Item::Popup_Menu_Item(const blink::WebPopupItem& item)
    : type(static_cast<Popup_Menu_Item_Type>(item.m_type))
    , textDirection(static_cast<blink::TextDirection>(item.m_textDirection))
    , hasTextDirectionOverride(item.m_hasTextDirectionOverride)
    , isEnabled(item.m_isEnabled)
    , isLabel(item.m_isLabel)
    , isSelected(item.m_isSelected)
    , text(item.m_text)
    , toolTip(item.m_toolTip)
    , accessibilityText(item.m_accessibilityText) {
}

Popup_Menu_Item_Type popup_menu_item_type_get(const Popup_Menu_Item* item) {
    EINA_SAFETY_ON_NULL_RETURN_VAL(item, POPUP_MENU_UNKNOWN);

    return item->type;
}

const char* popup_menu_item_text_get(const Popup_Menu_Item* item) {
    EINA_SAFETY_ON_NULL_RETURN_VAL(item, 0);

    return item->text.c_str();
}

blink::TextDirection popup_menu_item_text_direction_get(const Popup_Menu_Item* item) {
    EINA_SAFETY_ON_NULL_RETURN_VAL(item, blink::LTR);

    return item->textDirection;
}

Eina_Bool popup_menu_item_text_direction_override_get(const Popup_Menu_Item* item) {
    EINA_SAFETY_ON_NULL_RETURN_VAL(item, false);

    return item->hasTextDirectionOverride;
}

const char* popup_menu_item_tooltip_get(const Popup_Menu_Item* item) {
    EINA_SAFETY_ON_NULL_RETURN_VAL(item, 0);

    return item->toolTip.c_str();
}

const char* popup_menu_item_accessibility_text_get(const Popup_Menu_Item* item) {
    EINA_SAFETY_ON_NULL_RETURN_VAL(item, 0);

    return item->accessibilityText.c_str();
}

Eina_Bool popup_menu_item_enabled_get(const Popup_Menu_Item* item) {
    EINA_SAFETY_ON_NULL_RETURN_VAL(item, false);

    return item->isEnabled;
}

Eina_Bool popup_menu_item_is_label_get(const Popup_Menu_Item* item) {
    EINA_SAFETY_ON_NULL_RETURN_VAL(item, false);

    return item->isLabel;
}

Eina_Bool popup_menu_item_selected_get(const Popup_Menu_Item* item) {
    EINA_SAFETY_ON_NULL_RETURN_VAL(item, false);

    return item->isSelected;
}
