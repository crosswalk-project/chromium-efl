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

#ifndef NAVIGATION_POLICY_HANDLER_EFL_H_
#define NAVIGATION_POLICY_HANDLER_EFL_H_

#include "navigation_policy_params.h"

namespace content {
class RenderViewHost;
}

namespace IPC {
class Message;
}

class NavigationPolicyHandlerEfl {
 public:
  enum Decision {
    Undecied = 0,
    Handled,
    Unhandled
  };

  NavigationPolicyHandlerEfl(content::RenderViewHost* rvh, const NavigationPolicyParams& params);
  virtual ~NavigationPolicyHandlerEfl();
  void DownloadNavigation();
  Decision GetDecision() const { return decision_; }
  bool SetDecision(Decision);

 private:
  content::RenderViewHost* rvh_;
  Decision decision_;
  NavigationPolicyParams params_;
};

#endif /* NAVIGATION_POLICY_HANDLER_EFL_H_ */
