#ifndef CONTENT_CLIENT_EFL_H_
#define CONTENT_CLIENT_EFL_H_

#include "content/public/common/content_client.h"

#include "base/compiler_specific.h"

namespace IPC {
  class Message;
}

class ContentClientEfl : public content::ContentClient {
 public:
  // ContentClient implementation.
  virtual std::string GetProduct() const OVERRIDE;
  virtual std::string GetUserAgent() const OVERRIDE;
  virtual base::string16 GetLocalizedString(int message_id) const OVERRIDE;
  virtual base::StringPiece GetDataResource(
      int resource_id,
      ui::ScaleFactor scale_factor) const OVERRIDE;
  virtual bool CanSendWhileSwappedOut(const IPC::Message* message) OVERRIDE;
};

#endif  // CONTENT_CLIENT_EFL_H_
