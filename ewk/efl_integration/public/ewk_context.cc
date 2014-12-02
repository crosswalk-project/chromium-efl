// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_context.h"

// TODO: remove dependency
#include <../src/base/memory/ref_counted.h>
#include <../impl/browser/vibration/vibration_provider_client.h>
#include <../impl/browser_context_efl.h>
#include <../impl/http_user_agent_settings_efl.h>
#include <../impl/API/ewk_cookie_manager_private.h>
#if !defined(EWK_BRINGUP)
#include <../impl/API/ewk_context_form_autofill_profile_private.h>
#endif

#include <tizen_webview/public/tw_security_origin.h>
#include <tizen_webview/public/tw_web_context.h>
#include <tizen_webview/public/tw_url.h>

#include "public/ewk_security_origin.h"
#include "private/ewk_context_private.h"
#include "private/ewk_private.h"
#include "private/ewk_security_origin_private.h"
#include "private/chromium_glue.h"
#include "private/ewk_favicon_database_private.h"


using tizen_webview::WebContext;

Ewk_Context *ewk_context_ref(Ewk_Context *context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, NULL);
  context->AddRef();
  return context;
}

void ewk_context_unref(Ewk_Context *context)
{
  EINA_SAFETY_ON_NULL_RETURN(context);
  context->Release();
}

Ewk_Cookie_Manager* ewk_context_cookie_manager_get(const Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, 0);
  return context->ewkCookieManager();
}

Ewk_Context* ewk_context_default_get()
{
  return static_cast<Ewk_Context*>(WebContext::DefaultContext());
}

Ewk_Context* ewk_context_new()
{
  return static_cast<Ewk_Context*>(WebContext::Create());
}

Ewk_Context *ewk_context_new_with_injected_bundle_path(const char *path)
{
  //with chromium engine this API not used.
  //returning default context
  NOTIMPLEMENTED();
  return NULL;
}

void ewk_context_delete(Ewk_Context* context)
{
  if (context)
    context->Release();
}

void ewk_context_proxy_uri_set(Ewk_Context* context, const char* proxy)
{
  // browser only api
  EINA_SAFETY_ON_NULL_RETURN(context);
  EINA_SAFETY_ON_NULL_RETURN(proxy);
  if (0 != strcmp(proxy,""))
    context->SetProxyUri(proxy);
}

const char* ewk_context_proxy_uri_get(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context,0);
  return context->GetProxyUri();
}

Eina_Bool ewk_context_notify_low_memory(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, false);
  context->NotifyLowMemory();
  return true;
}

Eina_Bool ewk_context_origins_free(Eina_List* origins)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(origins, false);

  void* current_origin;
  EINA_LIST_FREE(origins, current_origin) {
    Ewk_Security_Origin* origin = static_cast<Ewk_Security_Origin*>(current_origin);
    delete origin;
  }
  return true;
}

Eina_Bool ewk_context_application_cache_delete_all(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->DeleteAllApplicationCache();
  return true;
}

Eina_Bool ewk_context_application_cache_delete(Ewk_Context* context, Ewk_Security_Origin* origin)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(origin, EINA_FALSE);
  context->DeleteApplicationCacheForSite(origin->GetURL());
  return true;
}

Eina_Bool ewk_context_application_cache_origins_get(Ewk_Context* context, Ewk_Web_Application_Cache_Origins_Get_Callback callback, void* user_data)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);

  context->GetAllOriginsWithApplicationCache(callback, user_data);
  return true;
}

Eina_Bool ewk_context_application_cache_usage_for_origin_get(Ewk_Context* context, const Ewk_Security_Origin* origin, Ewk_Web_Application_Cache_Usage_For_Origin_Get_Callback callback, void* user_data)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(origin, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->GetApplicationCacheUsage(origin->GetURL(), callback, user_data);
  return true;
}

Eina_Bool ewk_context_application_cache_quota_for_origin_set(Ewk_Context* context, const Ewk_Security_Origin* origin, int64_t quota)
{
  NOTIMPLEMENTED() << "Chromium does not support quota management for individual features.";
  return false;
}

Eina_Bool ewk_context_icon_database_path_set(Ewk_Context* context, const char* path)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  return context->SetFaviconDatabasePath(path) ? EINA_TRUE : EINA_FALSE;
}

Evas_Object* ewk_context_icon_database_icon_object_add(Ewk_Context* context, const char* uri, Evas* canvas)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(uri, NULL);
  EINA_SAFETY_ON_NULL_RETURN_VAL(canvas, NULL);
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, NULL);
  return context->AddFaviconObject(uri, canvas);
}

Eina_Bool ewk_context_local_file_system_all_delete(Ewk_Context *context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->FileSystemDelete(tizen_webview::URL());
  return true;
}

Eina_Bool ewk_context_local_file_system_delete(Ewk_Context *context, Ewk_Security_Origin *origin)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(origin, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->FileSystemDelete(tizen_webview::URL(ewk_security_origin_host_get(origin)));
  return true;
}

Eina_Bool ewk_context_local_file_system_origins_get(const Ewk_Context *context,
                                                    Ewk_Local_File_System_Origins_Get_Callback callback,
                                                    void *user_data)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->GetAllOriginsWithFileSystem(callback, user_data);
  return true;
}

Eina_Bool ewk_context_web_database_delete_all(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->WebDBDelete(tizen_webview::URL());
  return true;
}

Eina_Bool ewk_context_web_database_delete(Ewk_Context* context, Ewk_Security_Origin* origin)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(origin, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->WebDBDelete(tizen_webview::URL(ewk_security_origin_host_get(origin)));
  return true;
}

Eina_Bool ewk_context_web_database_origins_get(Ewk_Context* context,
                                               Ewk_Web_Database_Origins_Get_Callback callback,
                                               void* user_data)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->GetAllOriginsWithWebDB(callback, user_data);
  return true;
}

Eina_Bool ewk_context_web_database_quota_for_origin_get(Ewk_Context* context, Ewk_Web_Database_Quota_Get_Callback callback, void* userData, Ewk_Security_Origin* origin)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

Eina_Bool ewk_context_web_database_quota_for_origin_set(Ewk_Context* context, Ewk_Security_Origin* origin, uint64_t quota)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

Eina_Bool ewk_context_web_indexed_database_delete_all(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->IndexedDBDelete();
  return true;
}

Eina_Bool ewk_context_web_storage_delete_all(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->WebStorageDelete();
  return true;
}

Eina_Bool ewk_context_web_storage_origin_delete(Ewk_Context* context, Ewk_Security_Origin* origin)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(origin, EINA_FALSE);

  context->WebStorageDelete(origin->GetURL());
  return EINA_TRUE;
}

Eina_Bool ewk_context_web_storage_origins_get(Ewk_Context* context, Ewk_Web_Storage_Origins_Get_Callback callback, void* user_data)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);

  context->WebStorageOriginsAllGet(callback, user_data);
  return EINA_TRUE;
}

Eina_Bool ewk_context_web_storage_path_set(Ewk_Context* /*context*/, const char* /*path*/)
{
  //With chromium engine no separate path for web storage supported.
  //This API not implemented
  NOTIMPLEMENTED();
  return EINA_FALSE;
}

Eina_Bool ewk_context_web_storage_usage_for_origin_get(Ewk_Context* context, Ewk_Security_Origin* origin, Ewk_Web_Storage_Usage_Get_Callback callback, void* userData)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

Eina_Bool ewk_context_soup_data_directory_set(Ewk_Context* /*context*/, const char* /*path*/)
{
  //chomium engine does not use libsoup hence this API will not be implemented
  NOTIMPLEMENTED();
  return EINA_FALSE;
}

Eina_Bool ewk_context_cache_model_set(Ewk_Context* context, Ewk_Cache_Model model)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->SetCacheModel(chromium_glue::to(model));
  return true;
}

Ewk_Cache_Model ewk_context_cache_model_get(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EWK_CACHE_MODEL_DOCUMENT_VIEWER);
  return chromium_glue::from(context->GetCacheModel());
}

Eina_Bool ewk_context_cache_disabled_set(Ewk_Context* context, Eina_Bool cacheDisabled)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->SetNetworkCacheEnable(!cacheDisabled);
  return true;
}

Eina_Bool ewk_context_cache_disabled_get(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  return !context->GetNetworkCacheEnable();
}

Eina_Bool ewk_context_certificate_file_set(Ewk_Context *context, const char *certificate_file)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(certificate_file, EINA_FALSE);
  context->SetCertificate(certificate_file);
  return true;
}

const char* ewk_context_certificate_file_get(const Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, NULL);
#if !defined(EWK_BRINGUP)
  return context->GetCertificate();
#else
  LOG_EWK_API_MOCKUP();
  return NULL;
#endif
}

Eina_Bool ewk_context_cache_clear(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->ClearNetworkCache();
  context->ClearWebkitCache();
  return EINA_TRUE;
}

void ewk_context_did_start_download_callback_set(Ewk_Context* context,
                                                 Ewk_Context_Did_Start_Download_Callback callback,
                                                 void* userData)
{
  EINA_SAFETY_ON_NULL_RETURN(context);
  context->SetDidStartDownloadCallback(callback, userData);
}

void ewk_context_mime_override_callback_set(Ewk_Context_Override_Mime_For_Url_Callback callback)
{
  WebContext::SetMimeOverrideCallback(callback);
}

void ewk_context_memory_sampler_start(Ewk_Context* context, double timerInterval)
{
  EINA_SAFETY_ON_NULL_RETURN(context);
  context->StartMemorySampler(timerInterval);
}

void ewk_context_memory_sampler_stop(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN(context);
  context->StopMemorySampler();
}

Eina_Bool ewk_context_additional_plugin_path_set(Ewk_Context *context, const char *path)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(path, EINA_FALSE);
  context->AddExtraPluginDir(path);
  return true;
}

void ewk_context_memory_saving_mode_set(Ewk_Context* context, Eina_Bool mode)
{
  //memory saving mode should be handled in chromium in different way
  //than it is handled in WebKit, so implementation of this feature
  //is postponed, for now dummy implementation is enough
  NOTIMPLEMENTED();
  return;
}

void ewk_context_form_password_data_clear(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN(context);
  context->ClearPasswordData();
}

void ewk_context_form_password_data_delete(Ewk_Context* ewkContext, const char* url)
{
  LOG_EWK_API_MOCKUP();
}

Eina_List* ewk_context_form_password_data_list_get(Ewk_Context* ewkContext)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

void ewk_context_form_password_data_list_free(Ewk_Context* ewkContext, Eina_List* list)
{
  LOG_EWK_API_MOCKUP();
}

void ewk_context_form_candidate_data_clear(Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN(context);
  context->ClearCandidateData();
}

Eina_List* ewk_context_form_autofill_profile_get_all(Ewk_Context* context)
{
  if (NULL == context) {
    return NULL;
  }
#if !defined(EWK_BRINGUP)
  return EwkContextFormAutofillProfileManager::priv_form_autofill_profile_get_all(context);
#else
  LOG_EWK_API_MOCKUP();
  return NULL;
#endif
}

Ewk_Autofill_Profile* ewk_context_form_autofill_profile_get(Ewk_Context* context, unsigned id)
{
  if (NULL == context) {
    return NULL;
  }
#if !defined(EWK_BRINGUP)
  return EwkContextFormAutofillProfileManager::priv_form_autofill_profile_get(context, id);
#else
  LOG_EWK_API_MOCKUP();
  return NULL;
#endif
}

Eina_Bool ewk_context_form_autofill_profile_set(Ewk_Context* context, unsigned id, Ewk_Autofill_Profile* profile)
{
  if (NULL == context || NULL == profile) {
    return EINA_FALSE;
  }
#if !defined(EWK_BRINGUP)
  return EwkContextFormAutofillProfileManager::priv_form_autofill_profile_set(context, id, profile);
#else
  LOG_EWK_API_MOCKUP();
  return NULL;
#endif
}

Eina_Bool ewk_context_form_autofill_profile_add(Ewk_Context* context, Ewk_Autofill_Profile* profile)
{
  if (NULL == context || NULL == profile) {
    return EINA_FALSE;
  }
#if !defined(EWK_BRINGUP)
  return EwkContextFormAutofillProfileManager::priv_form_autofill_profile_add(context, profile);
#else
  LOG_EWK_API_MOCKUP();
  return NULL;
#endif
}

Eina_Bool ewk_context_form_autofill_profile_remove(Ewk_Context* context, unsigned id)
{
  if (NULL == context) {
    return EINA_FALSE;
  }
#if !defined(EWK_BRINGUP)
  return EwkContextFormAutofillProfileManager::priv_form_autofill_profile_remove(context, id);
#else
  LOG_EWK_API_MOCKUP();
  return NULL;
#endif
}

void ewk_context_vibration_client_callbacks_set(Ewk_Context* context,
                                                Ewk_Vibration_Client_Vibrate_Cb vibrate,
                                                Ewk_Vibration_Client_Vibration_Cancel_Cb cancel,
                                                void* data)
{
  VibrationProviderClient* vibra_client = VibrationProviderClient::GetInstance();
  vibra_client->SetVibrationClientCallbacks(vibrate, cancel, data);
}

Eina_Bool ewk_context_tizen_extensible_api_string_set(Ewk_Context* ewkContext,  const char* extensibleAPI, Eina_Bool enable)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

Eina_Bool ewk_context_tizen_extensible_api_set(Ewk_Context * /*context*/, Ewk_Extensible_API /*extensibleAPI*/, Eina_Bool /*enable*/)
{
  //This API will not be implemented with chromium engine based Ewk View
  NOTIMPLEMENTED();
  return EINA_FALSE;
}

Eina_Bool ewk_context_tizen_extensible_api_get(Ewk_Context * /*context*/, Ewk_Extensible_API /*extensibleAPI*/)
{
  //This API will not be implemented with chromium engine based Ewk View
  NOTIMPLEMENTED();
  return EINA_FALSE;
}

void ewk_context_storage_path_reset(Ewk_Context* /*context*/)
{
  //not supported in chromium to dynamically update the storage path
  NOTIMPLEMENTED();
}

Eina_Bool ewk_context_pixmap_set(Ewk_Context *context, int pixmap)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, EINA_FALSE);
  context->SetPixmap(pixmap);
  return true;
}

int ewk_context_pixmap_get(Ewk_Context *context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, 0);
  return context->Pixmap();
}

unsigned int ewk_context_inspector_server_start(Ewk_Context* ewkContext, unsigned int port)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

Eina_Bool ewk_context_inspector_server_stop(Ewk_Context* ewkContext)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

void ewk_send_widget_info(Ewk_Context *context,
                          int widget_handle,
                          double scale,
                          const char *theme,
                          const char *encodedBundle)
{
  EINA_SAFETY_ON_NULL_RETURN(theme);
  EINA_SAFETY_ON_NULL_RETURN(encodedBundle);
  EINA_SAFETY_ON_NULL_RETURN(context);
  context->SendWidgetInfo(widget_handle, scale, theme, encodedBundle);
}

#if defined(OS_TIZEN_TV)
Ewk_Application_Cache_Manager* ewk_context_application_cache_manager_get(const Ewk_Context* ewkContext)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
  return NULL;
}
#endif

Ewk_Favicon_Database* ewk_context_favicon_database_get(const Ewk_Context* ewkContext)
{
#if defined(OS_TIZEN_TV)
  EINA_SAFETY_ON_NULL_RETURN_VAL(ewkContext, 0);
  Ewk_Favicon_Database *favicon_database = Ewk_Favicon_Database::Instance();
  favicon_database->InitEwkFaviconDatabase(ewkContext);
  return favicon_database;
#else
  LOG_EWK_API_MOCKUP("Only for Tizen TV Browser");
  return NULL;
#endif
}

void ewk_context_resource_cache_clear(Ewk_Context* ewkContext)
{
  EINA_SAFETY_ON_NULL_RETURN(ewkContext);
  ewkContext->ClearNetworkCache();
  ewkContext->ClearWebkitCache();
}

#if defined(OS_TIZEN_TV)
Eina_Bool ewk_context_favicon_database_directory_set(Ewk_Context* ewkContext, const char* directoryPath)
{
  LOG_EWK_API_MOCKUP("Tizen TV Browser");
  return false;
}
#endif // OS_TIZEN_TV

void ewk_context_preferred_languages_set(Eina_List* languages)
{
  std::string preferredLanguages;

  Eina_List* it;
  void* data;

  EINA_LIST_FOREACH(languages, it, data) {
    if (data) {
      preferredLanguages.append(static_cast<char*>(data));
      if (it->next) {
        preferredLanguages.append(",");
      }
    }
  }
  std::transform(preferredLanguages.begin(), preferredLanguages.end(), preferredLanguages.begin(), ::tolower);
  std::replace(preferredLanguages.begin(), preferredLanguages.end(), '_', '-');
  HttpUserAgentSettingsEfl::SetAcceptLanguage(preferredLanguages);
}

#if defined(OS_TIZEN_TV)
Ewk_Storage_Manager* ewk_context_storage_manager_get(const Ewk_Context* ewkContext)
{
  LOG_EWK_API_MOCKUP("Tizen TV Browser");
  return NULL;
}
#endif // OS_TIZEN_TV
