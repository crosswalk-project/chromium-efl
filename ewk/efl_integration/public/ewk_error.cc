// Copyright (C) 2012 Intel Corporation.
// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_error.h"

#include "net/base/net_errors.h"

// TODO: remove dependency
#include <../impl/API/ewk_error_private.h>

#include "private/ewk_private.h"

Ewk_Error_Type ewk_error_type_get(const Ewk_Error* error)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(error, EWK_ERROR_TYPE_NONE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(error->domain, EWK_ERROR_TYPE_NONE);

  // TODO: right now no other Ewk_Error domain is used
  if (strcmp(error->domain, net::kErrorDomain) == 0) {
    return EWK_ERROR_TYPE_NETWORK;
  }

  LOG_EWK_API_MOCKUP("Unknown error domain: %s", error->domain);
  return EWK_ERROR_TYPE_NONE;
}

const char* ewk_error_url_get(const Ewk_Error* error)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(error, 0);
  return error->url;
}

int ewk_error_code_get(const Ewk_Error* error)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(error, 0);
  return error->error_code;
}

const char* ewk_error_description_get(const Ewk_Error* error)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(error, 0);
  return error->description;
}

const char* ewk_error_domain_get(const Ewk_Error* error)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}
