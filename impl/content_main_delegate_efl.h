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

#ifndef CONTENT_MAIN_DELEGATE
#define CONTENT_MAIN_DELEGATE

#include "content/public/app/content_main_delegate.h"

#include "base/compiler_specific.h"
#include "content/public/browser/content_browser_client.h"
#include "common/content_client_efl.h"

class EWebContext;

namespace content {

class ContentMainDelegateEfl
    : public ContentMainDelegate {
 public:
  ContentMainDelegateEfl() {}
  // Tells the embedder that the absolute basic startup has been done, i.e.
  // it's now safe to create singletons and check the command line. Return true
  // if the process should exit afterwards, and if so, |exit_code| should be
  // set. This is the place for embedder to do the things that must happen at
  // the start. Most of its startup code should be in the methods below.
  virtual bool BasicStartupComplete(int* exit_code) OVERRIDE;
  virtual void PreSandboxStartup() OVERRIDE;
  virtual ContentBrowserClient* CreateContentBrowserClient() OVERRIDE;

 private:
  scoped_ptr<ContentBrowserClient> client_;
  ContentClientEfl content_client_;
  DISALLOW_COPY_AND_ASSIGN(ContentMainDelegateEfl);
};

}

#endif
