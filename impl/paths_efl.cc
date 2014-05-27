#include "paths_efl.h"

#include "base/base_paths.h"
#include "base/path_service.h"
#include "base/files/file_path.h"

static const base::FilePath::CharType kDataPath[] = FILE_PATH_LITERAL(DATA_DIR);
static const base::FilePath::CharType kExePath[] = FILE_PATH_LITERAL(EXE_DIR);
static const base::FilePath::CharType kWebDBPath[] = FILE_PATH_LITERAL(WEBDB_DIR);
#ifdef OS_TIZEN
static const base::FilePath::CharType kEdjeThemesDir[] = FILE_PATH_LITERAL("themes");
#else
static const base::FilePath::CharType kEdjeThemesDir[] = FILE_PATH_LITERAL("resources");
#endif

//static
void PathsEfl::Register()
{
  PathService::RegisterProvider(GetPath, PATH_START, PATH_END);
}

//static
bool PathsEfl::GetPath(int key, base::FilePath* result)
{
  switch (key) {
#ifdef OS_TIZEN
   case base::DIR_EXE:
      *result = base::FilePath(kExePath);
      return true;
    case EDJE_RESOURCE_DIR:
      *result = base::FilePath(kDataPath).Append(kEdjeThemesDir);
      return true;
    case WEB_DATABSE_DIR:
      *result = base::FilePath(kWebDBPath);
      return true;
#else
    case WEB_DATABSE_DIR:
    case EDJE_RESOURCE_DIR:
      PathService::Get(base::DIR_MODULE, result);
      *result = result->Append(kEdjeThemesDir);
      return true;
#endif
    default:
      return false;
  }
}
