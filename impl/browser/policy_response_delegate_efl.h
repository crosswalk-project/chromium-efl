#ifndef POLICY_RESPONSE_DELEGATE_EFL_H_
#define POLICY_RESPONSE_DELEGATE_EFL_H_

#include "base/memory/ref_counted.h"
#include "net/base/completion_callback.h"
#include "url/gurl.h"

struct _Ewk_Policy_Decision;

namespace net {
class URLRequest;
class HttpResponseHeaders;
}

class PolicyResponseDelegateEfl: public base::RefCountedThreadSafe<PolicyResponseDelegateEfl> {
 public:
  PolicyResponseDelegateEfl(net::URLRequest* request,
                            const net::CompletionCallback& callback,
                            const net::HttpResponseHeaders* original_response_headers);
  virtual void UseResponse();
  virtual void IgnoreResponse();
  void HandleURLRequestDestroyedOnIOThread();

  int GetRenderProcessId() const { return render_process_id_; }
  int GetRenderFrameId() const { return render_frame_id_; }

 private:
  friend class base::RefCountedThreadSafe<PolicyResponseDelegateEfl>;

  void HandlePolicyResponseOnUIThread();
  void UseResponseOnIOThread();
  void IgnoreResponseOnIOThread();

  scoped_ptr<_Ewk_Policy_Decision> policy_decision_;
  net::CompletionCallback callback_;
  int render_process_id_;
  int render_frame_id_;
  // Should be accessed only on IO thread.
  bool processed_;
};

#endif /* POLICY_RESPONSE_DELEGATE_EFL_H_ */
