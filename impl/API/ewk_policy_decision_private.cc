/*
 * Copyright (C) 2014 Samsung Electronics
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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "ewk_policy_decision_private.h"

#include "web_contents_delegate_efl.h"

void _Ewk_Policy_Decision::Use() {
  isDecided = true;
  switch (type) {
    case POLICY_RESPONSE:
      policy_response_delegate->UseResponse();
      break;
    case POLICY_NAVIGATION:
      navigation_policy_handler->SetDecision(NavigationPolicyHandlerEfl::Unhandled);
      break;
    case POLICY_NEWWINDOW:
      new_window_policy_delegate->set_new_window_policy(true);
      break;
    default:
      NOTREACHED();
      break;
  }
}
