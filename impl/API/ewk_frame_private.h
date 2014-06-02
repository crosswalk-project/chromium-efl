#ifndef EWK_FRAME_PRIVATE_H_
#define EWK_FRAME_PRIVATE_H_

struct NavigationPolicyParams;

namespace content {
class RenderFrameHost;
} // namespace content

class Ewk_Frame {
public:
  Ewk_Frame(content::RenderFrameHost* rfh);
  Ewk_Frame(const NavigationPolicyParams &params);

  bool IsMainFrame() const { return is_main_frame_; }

private:
  bool is_main_frame_;
};

#endif // EWK_FRAME_PRIVATE_H_
