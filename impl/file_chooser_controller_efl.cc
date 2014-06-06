#include "content/common/view_messages.h"
#include "content/public/browser/render_view_host.h"
#include "file_chooser_controller_efl.h"
#include <Eina.h>
#include <Elementary.h>

#include "base/files/file_path.h"
#include "ui/shell_dialogs/selected_file_info.h"
#include "base/strings/utf_string_conversions.h"

namespace content {

static void _fs_done(void *data, Evas_Object *obj,void *event_info) {
  const char *sel_path = (char *)event_info;

  FileChooserControllerEfl *inst = (FileChooserControllerEfl*) data;

  std::vector<ui::SelectedFileInfo> files;

  RenderViewHost* render_view_host = inst->getRenderViewHost();
  if (!render_view_host)
    return;

  if (sel_path) {
    GURL url(sel_path);
    if (!url.is_valid()) {
      base::FilePath path(url.SchemeIsFile() ? url.path() : sel_path);
      files.push_back(ui::SelectedFileInfo(path, base::FilePath()));
    }
  }

  render_view_host->FilesSelectedInChooser(files, inst->getParams()->mode);
  evas_object_del(elm_object_top_widget_get(obj));
}

RenderViewHost* FileChooserControllerEfl::getRenderViewHost() {
  return render_view_host_;
}

void FileChooserControllerEfl::setRenderViewHost(RenderViewHost* render_view_host) {
  render_view_host_ = render_view_host;
}

const content::FileChooserParams* FileChooserControllerEfl::getParams() {
  return params_;
}

void FileChooserControllerEfl::setParams(const content::FileChooserParams* params) {
  params_ = params;
}

void FileChooserControllerEfl::parseParams() {
  title = DEF_TITLE_OPEN;
  def_file_name = DEF_FILE_NAME;
  is_save_file = EINA_FALSE;
  folder_only = EINA_FALSE;

  if (!params_)
      return;

  switch (params_->mode) {
    case FileChooserParams::Open:
      break;
    case FileChooserParams::OpenMultiple:
    // only since elementary 1.8
      break;
    case FileChooserParams::UploadFolder:
      folder_only = EINA_TRUE;
      break;
    case FileChooserParams::Save:
      title = DEF_TITLE_SAVE;
      is_save_file = EINA_TRUE;
    break;
    default:
      NOTREACHED();
  }

  if (!params_->title.empty())
    title = base::UTF16ToUTF8(params_->title);

  if (!params_->default_file_name.empty()) {
    if (params_->default_file_name.EndsWithSeparator())
      def_file_name = params_->default_file_name.value();
    else
      def_file_name = params_->default_file_name.DirName().value();
  }
}

void FileChooserControllerEfl::open() {
  Evas_Object *win, *fs, *bx;

  win = elm_win_util_standard_add("fileselector", title.c_str());

  elm_win_modal_set(win, EINA_TRUE);

  bx = elm_box_add(win);
  elm_win_resize_object_add(win, bx);
  elm_box_horizontal_set(bx, EINA_TRUE);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(bx);

  fs = elm_fileselector_add(win);

  elm_fileselector_is_save_set(fs, is_save_file);

  elm_fileselector_folder_only_set(fs, folder_only);

  elm_fileselector_expandable_set(fs, EINA_FALSE);

  /* start the fileselector in proper dir */
  elm_fileselector_path_set(fs, def_file_name.c_str());

  evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_box_pack_end(bx, fs);
  evas_object_show(fs);

  /* the 'done' cb is called when the user presses ok/cancel */
  evas_object_smart_callback_add(fs, "done", _fs_done, this);

  evas_object_resize(win, 400, 400);
  evas_object_show(win);
}

} //namespace

