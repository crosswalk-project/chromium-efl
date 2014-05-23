/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 */

#include "common/version_info.h"

#include "common/version_info_efl.h"

namespace EflWebView {

const char kOSTypeTizen[] = "Tizen";
const char kOSTypeAndroid[] = "Android";

VersionInfo* VersionInfo::version_info_ = NULL;

VersionInfo* VersionInfo::GetInstance()
{
  if(!version_info_)
    version_info_ = new VersionInfo;
  return version_info_;
}

void VersionInfo::DeleteInstance()
{
  if(version_info_)
    delete version_info_;
  version_info_ = NULL;
}

VersionInfo::VersionInfo()
  : product_name_(PRODUCT_NAME),
    product_version_(PRODUCT_VERSION),
    fake_android_(true)
{
}

void VersionInfo::SetProductName(const std::string& name)
{
  product_name_ = name;
}

std::string VersionInfo::LastChange() const
{
  return LAST_CHANGE;
}

bool VersionInfo::IsOfficialBuild() const
{
  return IS_OFFICIAL_BUILD;
}

std::string VersionInfo::CreateVersionString() const
{
  std::string current_version;
  if (is_valid()) {
    current_version += Version();
    current_version += " (";
    current_version += " ";
    current_version += LastChange();
    current_version += " ";
    current_version += OSType();
    current_version += ")";
  }
  return current_version;
}

std::string VersionInfo::OSType() const
{
  return (fake_android_ ? kOSTypeAndroid : kOSTypeTizen);
}

std::string VersionInfo::ProductNameAndVersionForUserAgent() const
{
  if (!is_valid())
    return std::string();
  return ( Name() + std::string("/") + Version());
}

} //namespace EflWebView
