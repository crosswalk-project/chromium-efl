 /* This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#if defined(OS_TIZEN_TV) && !defined(EWK_BRINGUP)

#ifndef ewk_favicon_database_private_h
#define ewk_favicon_database_private_h

#include <Evas.h>

#include <../impl/browser/favicon/favicon_database.h>
#include <tizen_webview/public/tw_web_context.h>
#include "private/ewk_context_private.h"

struct EwkFaviconDatabase {
 public:
  EwkFaviconDatabase()
  :favicon_database(NULL)
  {
  }

  ~EwkFaviconDatabase()
  {
    if (favicon_database)
      delete favicon_database;
    favicon_database = NULL;
  }

  void InitEwkFaviconDatabase(const Ewk_Context* ewkContext)
  {
    if (!favicon_database)
      favicon_database = const_cast<Ewk_Context*>(ewkContext)->GetFaviconDatabase();
    return;
  }
 private:
  FaviconDatabase* favicon_database;
};
#endif // ewk_favicon_database_private_h
#endif