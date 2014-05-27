/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef WEB_PROCESS_CONTENT_MAIN_DELEGATE_H
#define WEB_PROCESS_CONTENT_MAIN_DELEGATE_H

#include "base/compiler_specific.h"
#include "content/public/app/content_main.h"
#include "content/public/app/content_main_delegate.h"

namespace content {

class WebProcessContentMainDelegateEfl: public content::ContentMainDelegate {
 public:
  virtual void PreSandboxStartup() OVERRIDE;
  virtual bool BasicStartupComplete(int* exit_code) OVERRIDE;
  virtual ContentRendererClient* CreateContentRendererClient() OVERRIDE;
};

} // namespace content

#endif // WEB_PROCESS_CONTENT_MAIN_DELEGATE_H
