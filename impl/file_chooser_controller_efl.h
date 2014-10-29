// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef file_chooser_controller_efl_h
#define file_chooser_controller_efl_h

#include <Evas.h>

namespace content {

class RenderViewHost;

#define DEF_TITLE_OPEN "Open";
#define DEF_TITLE_SAVE "Save";
#define DEF_FILE_NAME "/tmp";

class FileChooserControllerEfl {
 public:
  FileChooserControllerEfl() : render_view_host_(NULL), params_(NULL) {
    parseParams();
  }

  FileChooserControllerEfl(RenderViewHost* render_view_host,
                           const content::FileChooserParams* params)
      : render_view_host_(render_view_host), params_(params) {
    parseParams();
  }
  ~FileChooserControllerEfl() { }

  void open();
  RenderViewHost* getRenderViewHost();
  void setRenderViewHost(RenderViewHost* render_view_host);
  const content::FileChooserParams* getParams();
  void setParams(const content::FileChooserParams* params);

 private:
  void parseParams();
  RenderViewHost* render_view_host_;
  const content::FileChooserParams* params_;
  std::string title;
  std::string def_file_name;
  Eina_Bool is_save_file;
  Eina_Bool folder_only;
};

} // namespace

#endif // file_chooser_controller_efl_h

