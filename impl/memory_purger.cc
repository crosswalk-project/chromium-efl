// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "memory_purger.h"

#include <set>

#include "base/allocator/allocator_extension.h"
#include "base/bind.h"
#include "base/threading/thread.h"
#include "common/render_messages_efl.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/browser_thread.h"
#include "net/proxy/proxy_resolver.h"
#include "net/proxy/proxy_service.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"

using content::BrowserContext;
using content::BrowserThread;

// PurgeMemoryHelper -----------------------------------------------------------

// This is a small helper class used to ensure that the objects we want to use
// on multiple threads are properly refed, so they don't get deleted out from
// under us.
class PurgeMemoryIOHelper
    : public base::RefCountedThreadSafe<PurgeMemoryIOHelper> {
 public:
  PurgeMemoryIOHelper() {
  }

  void AddRequestContextGetter(
      scoped_refptr<net::URLRequestContextGetter> request_context_getter);

  void PurgeMemoryOnIOThread();

 private:
  friend class base::RefCountedThreadSafe<PurgeMemoryIOHelper>;

  virtual ~PurgeMemoryIOHelper() {}

  typedef scoped_refptr<net::URLRequestContextGetter> RequestContextGetter;
  std::vector<RequestContextGetter> request_context_getters_;

  DISALLOW_COPY_AND_ASSIGN(PurgeMemoryIOHelper);
};

void PurgeMemoryIOHelper::AddRequestContextGetter(
    scoped_refptr<net::URLRequestContextGetter> request_context_getter) {
  request_context_getters_.push_back(request_context_getter);
}

void PurgeMemoryIOHelper::PurgeMemoryOnIOThread() {
  // Ask ProxyServices to purge any memory they can (generally garbage in the
  // wrapped ProxyResolver's JS engine).
#warning "[M37] Fix memory purge"
#if 0
  for (size_t i = 0; i < request_context_getters_.size(); ++i) {
    request_context_getters_[i]->GetURLRequestContext()->proxy_service()->
        PurgeMemory();
  }
#endif
}

// -----------------------------------------------------------------------------

// static
void MemoryPurger::PurgeAll(BrowserContext* browser_context) {
  PurgeBrowser(browser_context);
  PurgeRenderers();
  // TODO(pk):
  // * Tell the plugin processes to release their free memory?  Other stuff?
  // * Enumerate what other processes exist and what to do for them.
}

// static
void MemoryPurger::PurgeBrowser(BrowserContext* browser_context) {
  // Dump the backing stores.
#warning "[M37] backing stores were removed, so the code below is probably safe to remove, check this."
  //content::RenderWidgetHost::RemoveAllBackingStores();
  scoped_refptr<PurgeMemoryIOHelper> purge_memory_io_helper(
      new PurgeMemoryIOHelper());
  purge_memory_io_helper->AddRequestContextGetter(
        make_scoped_refptr(browser_context->GetRequestContext()));
#warning "[M37] Another non existing function"
  //BrowserContext::PurgeMemory(browser_context);
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&PurgeMemoryIOHelper::PurgeMemoryOnIOThread,
                 purge_memory_io_helper.get()));
  // Tell our allocator to release any free pages it's still holding.
  //
  // TODO(pk): A lot of the above calls kick off actions on other threads.
  // Maybe we should find a way to avoid calling this until those actions
  // complete?
  base::allocator::ReleaseFreeMemory();
}

// static
void MemoryPurger::PurgeRenderers() {
  // Direct all renderers to free everything they can.
  //
  // Concern: Telling a bunch of renderer processes to destroy their data may
  // cause them to page everything in to do it, which could take a lot of time/
  // cause jank.
  for (content::RenderProcessHost::iterator i(
          content::RenderProcessHost::AllHostsIterator());
       !i.IsAtEnd(); i.Advance())
    PurgeRendererForHost(i.GetCurrentValue());
}

// static
void MemoryPurger::PurgeRendererForHost(content::RenderProcessHost* host) {
#if !defined(EWK_BRINGUP)
  // Direct the renderer to free everything it can.
  host->Send(new EwkViewMsg_PurgeMemory());
#endif
}
