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

#ifndef WebPopupItem_h
#define WebPopupItem_h

#include "third_party/WebKit/Source/platform/text/TextDirection.h"
#include <string>

namespace blink {

struct WebPopupItem {
    enum Type {
        Separator,
        Item
    };

    WebPopupItem();
    WebPopupItem(Type);
    WebPopupItem(Type, const std::string& text, WebCore::TextDirection, bool hasTextDirectionOverride, const std::string& toolTip, const std::string& accessibilityText, bool isEnabled, bool isLabel, bool isSelected);

    Type m_type;
    std::string m_text;
    WebCore::TextDirection m_textDirection;
    bool m_hasTextDirectionOverride;
    std::string m_toolTip;
    std::string m_accessibilityText;
    bool m_isEnabled;
    bool m_isLabel;
    bool m_isSelected;
};

} // namespace blink

#endif // WebPopupItem_h
