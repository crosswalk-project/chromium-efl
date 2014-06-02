#include "ewk_frame_private.h"

#include <content/public/browser/render_frame_host.h>
#include <content/public/browser/render_process_host.h>
#include <content/public/browser/render_view_host.h>
#include <content/public/browser/web_contents.h>

#include <navigation_policy_params.h>

using content::RenderFrameHost;
using content::RenderViewHost;
using content::WebContents;

Ewk_Frame::Ewk_Frame(RenderFrameHost* rfh)
  : is_main_frame_(false) {
  if (rfh) {
    RenderViewHost *rvh = rfh->GetRenderViewHost();
    if (rvh) {
      WebContents *wc = WebContents::FromRenderViewHost(rvh);

      if (wc) {
        is_main_frame_ = wc->GetMainFrame() == rfh;
      }
    }
  }
}

Ewk_Frame::Ewk_Frame(const NavigationPolicyParams &params)
  : is_main_frame_(params.is_main_frame) {
}
