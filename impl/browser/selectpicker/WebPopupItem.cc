/*
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */

#include "WebPopupItem.h"

using namespace blink;

namespace blink {

WebPopupItem::WebPopupItem()
    : m_type(Unknown)
    , m_textDirection(LTR)
    , m_hasTextDirectionOverride(false)
    , m_isEnabled(true)
    , m_isSelected(false) {
}

WebPopupItem::WebPopupItem(Type type)
    : m_type(type)
    , m_textDirection(LTR)
    , m_hasTextDirectionOverride(false)
    , m_isEnabled(true)
    , m_isLabel(false)
    , m_isSelected(false) {
}

WebPopupItem::WebPopupItem(Type type, const std::string& text, TextDirection textDirection, bool hasTextDirectionOverride, const std::string& toolTip, const std::string& accessibilityText, bool isEnabled, bool isLabel, bool isSelected)
    : m_type(type)
    , m_text(text)
    , m_textDirection(textDirection)
    , m_hasTextDirectionOverride(hasTextDirectionOverride)
    , m_toolTip(toolTip)
    , m_accessibilityText(accessibilityText)
    , m_isEnabled(isEnabled)
    , m_isLabel(isLabel)
    , m_isSelected(isSelected) {
}

} // namespace blink
