/*
 * Copyright (C) 2009-2010 ProFUSION embedded systems
 * Copyright (C) 2009-2013 Samsung Electronics. All rights reserved.
 * Copyright (C) 2012 Intel Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */

#include "ewk_main.h"

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_IMF.h>
#include <Edje.h>
#include <Eina.h>
#include <Evas.h>

#ifdef HAVE_ECORE_X
#include <Ecore_X.h>
#endif

#include "public/ewk_log.h"
#include "private/webview_delegate_ewk.h"
#include "private/ewk_private.h"

// TODO: remove dependency
#include <../impl/command_line_efl.h>
#include <../impl/eweb_context.h>
#include <../impl/ewk_global_data.h>

static int _ewkInitCount = 0;

/////////////////////////////////////////////////////////////////////////////
//private function declaration here
static void _ewk_init_web_engine(void);
static void _ewk_shutdown_web_engine(void);

/**
 * \var     _ewk_log_dom
 * @brief   the log domain identifier that is used with EINA's macros
 */
int _ewk_log_dom = -1;

int ewk_init(void)
{
  if (_ewkInitCount)
      return ++_ewkInitCount;

  if (!eina_init())
      goto error_eina;

  _ewk_log_dom = eina_log_domain_register("ewebview-blink", EINA_COLOR_ORANGE);
  if (_ewk_log_dom < 0) {
      EINA_LOG_CRIT("could not register log domain 'ewebview-blink'");
      goto error_log_domain;
  }

  if (!evas_init()) {
      CRITICAL("could not init evas.");
      goto error_evas;
  }

  if (!ecore_init()) {
      CRITICAL("could not init ecore.");
      goto error_ecore;
  }

  if (!ecore_evas_init()) {
      CRITICAL("could not init ecore_evas.");
      goto error_ecore_evas;
  }

  if (!ecore_imf_init()) {
      CRITICAL("could not init ecore_imf.");
      goto error_ecore_imf;
  }

#ifdef HAVE_ECORE_X
  if (!ecore_x_init(0)) {
      CRITICAL("could not init ecore_x.");
      goto error_ecore_x;
  }
#endif

  if (!edje_init()) {
      CRITICAL("Could not init edje.");
      goto error_edje;
  }

  // FIXME: do we need glib integration like WebKit?
#if 0
#if !GLIB_CHECK_VERSION(2, 35, 0)
  g_type_init();
#endif

  if (!ecore_main_loop_glib_integrate()) {
      WARN("Ecore was not compiled with GLib support, some plugins will not "
          "work (ie: Adobe Flash)");
  }
#endif

  _ewk_init_web_engine();
  return ++_ewkInitCount;

error_edje:
#ifdef HAVE_ECORE_X
  ecore_x_shutdown();
error_ecore_x:
#else
  ecore_imf_shutdown();
#endif
error_ecore_imf:
  ecore_evas_shutdown();
error_ecore_evas:
  ecore_shutdown();
error_ecore:
  evas_shutdown();
error_evas:
  eina_log_domain_unregister(_ewk_log_dom);
  _ewk_log_dom = -1;
error_log_domain:
  eina_shutdown();
error_eina:
  return 0;
}

int ewk_shutdown(void)
{
  if (--_ewkInitCount)
      return _ewkInitCount;

  _ewk_shutdown_web_engine();

  edje_shutdown();
#ifdef HAVE_ECORE_X
  ecore_x_shutdown();
#endif
  ecore_imf_shutdown();
  ecore_evas_shutdown();
  ecore_shutdown();
  evas_shutdown();
  eina_log_domain_unregister(_ewk_log_dom);
  _ewk_log_dom = -1;
  eina_shutdown();

  return 0;
}

void ewk_set_arguments(int argc, char** argv)
{
    CommandLineEfl::Init(argc, argv);
}

void ewk_home_directory_set(const char* path)
{
  LOG_EWK_API_MOCKUP();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Private functions implementations for ewk_main module

//Initialize web engine
void _ewk_init_web_engine()
{
  if(!WebViewDelegate::IsDelegateRegistered()) {
    WebViewDelegate::RegisterDelegate(
      const_cast<WebViewDelegateEwk*>(&WebViewDelegateEwk::GetInstance()));
  }
}

void _ewk_shutdown_web_engine(void)
{
    //TODO: any web engine destroy to be done here
    CommandLineEfl::Shutdown();
    if (EwkGlobalData::GetInstance()) {
      delete EwkGlobalData::GetInstance();
    }
}

