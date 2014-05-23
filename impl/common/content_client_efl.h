#ifndef CONTENT_CLIENT_EFL_H_
#define CONTENT_CLIENT_EFL_H_

#include "content/public/common/content_client.h"

#include "base/compiler_specific.h"

class ContentClientEfl : public content::ContentClient {
 public:
  // ContentClient implementation.
  virtual std::string GetProduct() const OVERRIDE;
  virtual std::string GetUserAgent() const OVERRIDE;
};

#endif  // CONTENT_CLIENT_EFL_H_
