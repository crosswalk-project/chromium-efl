// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_context_h
#define ewk_context_h

#include <Evas.h>

#include "ewk_autofill_profile.h"
#include "ewk_cookie_manager.h"
#include "ewk_export.h"
#include "ewk_security_origin.h"
#include "ewk_application_cache_manager.h"
#include "ewk_favicon_database.h"
#include "ewk_storage_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Defines extensible api
enum Ewk_Extensible_API {
    EWK_EXTENSIBLE_API_BACKGROUND_MUSIC,
    EWK_EXTENSIBLE_API_CSP,
    EWK_EXTENSIBLE_API_ENCRYPTION_DATABASE,
    EWK_EXTENSIBLE_API_FULL_SCREEN,
    EWK_EXTENSIBLE_API_FULL_SCREEN_FORBID_AUTO_EXIT,
    EWK_EXTENSIBLE_API_MEDIA_STREAM_RECORD,
    EWK_EXTENSIBLE_API_MEDIA_VOLUME_CONTROL,
    EWK_EXTENSIBLE_API_PRERENDERING_FOR_ROTATION,
    EWK_EXTENSIBLE_API_ROTATE_CAMERA_VIEW,
    EWK_EXTENSIBLE_API_ROTATION_LOCK,
    EWK_EXTENSIBLE_API_SOUND_MODE,
    EWK_EXTENSIBLE_API_SUPPORT_FULL_SCREEN,
    EWK_EXTENSIBLE_API_VISIBILITY_SUSPEND,
    EWK_EXTENSIBLE_API_XWINDOW_FOR_FULL_SCREEN_VIDEO,
    EWK_EXTENSIBLE_API_SUPPORT_MULTIMEDIA
};
/// Creates a type name for @a _Ewk_Extensible_API.
typedef enum Ewk_Extensible_API Ewk_Extensible_API;

typedef struct Ewk_Context Ewk_Context;
typedef struct Ewk_Context_Exceeded_Quota Ewk_Context_Exceeded_Quota;

/**
 * Deletes Ewk_Context.
 *
 * @param context Ewk_Context to delete
 */
EAPI void ewk_context_delete(Ewk_Context* context);

/**
 * Notify low memory to free unused memory.
 *
 * @param o context object to notify low memory.
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool ewk_context_notify_low_memory(Ewk_Context* ewkContext);

/**
 * Sets the given proxy URI to network backend of specific context.
 *
 * @param ewkContext context object to set proxy URI.
 * @param proxy URI to set
 */
EAPI void ewk_context_proxy_uri_set(Ewk_Context* ewkContext, const char* proxy);

/**
 * Gets the proxy URI from the network backend of specific context.
 *
 * It returns an internal string and should not
 * be modified. The string is guaranteed to be stringshared.
 *
 * @param ewkContext context object to get proxy URI.
 *
 * @return current proxy URI or @c 0 if it's not set
 */
EAPI const char* ewk_context_proxy_uri_get(Ewk_Context* ewkContext);

/**
 * @typedef Ewk_Local_File_System_Origins_Get_Callback Ewk_Local_File_System_Origins_Get_Callback
 * @brief Type definition for use with ewk_context_local_file_system_origins_get()
 */
typedef void (*Ewk_Local_File_System_Origins_Get_Callback)(Eina_List *origins, void *user_data);

/**
 * Callback for ewk_context_application_cache_origins_get
 *
 * @param origins web application cache origins
 * @param user_data user_data will be passsed when ewk_context_application_cache_origins_get is called
 */
typedef void (*Ewk_Web_Application_Cache_Origins_Get_Callback)(Eina_List* origins, void* user_data);

/**
 * Callback for ewk_context_application_cache_quota_get.
 *
 * @param quota web application cache quota
 * @param user_data user_data will be passsed when ewk_context_application_cache_quota_get is called
 */
typedef void (*Ewk_Web_Application_Cache_Quota_Get_Callback)(int64_t quota, void* user_data);

/**
 * Callback for ewk_context_application_cache_usage_for_origin_get.
 *
 * @param usage web application cache usage for origin
 * @param user_data user_data will be passsed when ewk_context_application_cache_usage_for_origin_get is called
 */
typedef void (*Ewk_Web_Application_Cache_Usage_For_Origin_Get_Callback)(int64_t usage, void* user_data);

/**
 * Callback for ewk_context_application_cache_path_get.
 *
 * @param path web application cache directory
 * @param user_data user_data will be passsed when ewk_context_application_cache_path_get is called
 */
//typedef void (*Ewk_Web_Application_Cache_Path_Get_Callback)(const char* path, void* user_data);

/**
 * Callback for ewk_context_web_database_origins_get.
 *
 * @param origins web database origins
 * @param user_data user_data will be passsed when ewk_context_web_database_origins_get is called
 */
typedef void (*Ewk_Web_Database_Origins_Get_Callback)(Eina_List* origins, void* user_data);

/**
 * Callback for ewk_context_web_database_quota_for_origin_get.
 *
 * @param quota web database quota
 * @param user_data user_data will be passsed when ewk_context_web_database_quota_for_origin_get is called
 */
typedef void (*Ewk_Web_Database_Quota_Get_Callback)(uint64_t quota, void* user_data);

/**
 * Callback for ewk_context_web_database_usage_for_origin_get.
 *
 * @param usage web database usage
 * @param user_data user_data will be passsed when ewk_context_web_database_usage_for_origin_get is called
 */
typedef void (*Ewk_Web_Database_Usage_Get_Callback)(uint64_t usage, void* user_data);

/**
 * Callback for ewk_context_web_database_path_get.
 *
 * @param path web database directory
 * @param user_data user_data will be passsed when ewk_context_web_database_path_get is called
 */
//typedef void (*Ewk_Web_Database_Path_Get_Callback)(const char* path, void* user_data);

/**
 * Callback for ewk_context_web_storage_origins_get.
 *
 * @param origins web storage origins
 * @param user_data user_data will be passsed when ewk_context_web_storage_origins_get is called
 */
typedef void (*Ewk_Web_Storage_Origins_Get_Callback)(Eina_List* origins, void* user_data);

/**
 * Callback for ewk_context_web_storage_usage_for_origin_get.
 *
 * @param usage usage of web storage
 * @param user_data user_data will be passsed when ewk_context_web_storage_usage_for_origin_get is called
 */
typedef void (*Ewk_Web_Storage_Usage_Get_Callback)(uint64_t usage, void* user_data);

/*
 * Callback for didStartDownload
 *
 * @param download_url url to download
 * @param user_data user_data will be passsed when download is started
 */
typedef void (*Ewk_Context_Did_Start_Download_Callback)(const char* download_url, void* user_data);

/*
 * Callback for overriding default mime type
 *
 * @param url url for which the mime type can be overriden
 * @param mime current mime type assumed by the web engine
 * @param new_mime string with a new mime type for content pointer by url. Should be allocated
 *                 dynamically, the browser will take ownership of the allocated memory and free
 *                 it when it's no longer needed
 * @return true in case mime should be override by the contents of new_mime, false otherwise
 */
typedef Eina_Bool (*Ewk_Context_Override_Mime_For_Url_Callback)(const char* url, const char *mime, char **new_mime);

/**
 * Requests for freeing origins.
 *
 * @param origins list of origins
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_origins_free(Eina_List* origins);

/**
 * Requests for deleting all web application caches.
 *
 * @param context context object
 *
 * @return @c EINA_TRUE on successful request or @c EINA FALSE on failure
 */
EAPI Eina_Bool ewk_context_application_cache_delete_all(Ewk_Context* context);

/**
 * Requests for deleting web application cache for origin.
 *
 * @param context context object
 * @param origin application cache origin
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_application_cache_delete(Ewk_Context* context, Ewk_Security_Origin* origin);

/**
 * Requests for getting web application cache origins.
 *
 * @param context context object
 * @param result_callback callback to get web application cache origins
 * @param user_data user_data will be passsed when result_callback is called
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 *
 * @see ewk_context_origins_free
 */
EAPI Eina_Bool ewk_context_application_cache_origins_get(Ewk_Context* context, Ewk_Web_Application_Cache_Origins_Get_Callback callback, void* user_data);

/**
 * Requests for setting application cache quota for origin.
 *
 * @param context context object
 * @param origin serucity origin
 * @param quota size of quota
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_application_cache_quota_for_origin_set(Ewk_Context* context, const Ewk_Security_Origin* origin, int64_t quota);

/**
 * Requests for getting application cache usage for origin.
 *
 * @param context context object
 * @param origin security origin
 * @param result_callback callback to get web database usage
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_application_cache_usage_for_origin_get(Ewk_Context* context, const Ewk_Security_Origin* origin, Ewk_Web_Application_Cache_Usage_For_Origin_Get_Callback callback, void* user_data);

/**
 * Requests setting of the favicon database path.
 *
 * @param context context object
 * @param path database path.
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_icon_database_path_set(Ewk_Context* context, const char* path);

/**
 * Requests to get image representing the given URL.
 *
 * @param context context object
 * @param url which url to query icon, must not be @c 0
 * @param canvas evas instance where to add resulting object, must not be @c 0
 *
 * @return newly allocated Evas_Object instance or @c 0 on
 *         errors. Delete the object with evas_object_del().
 */
EAPI Evas_Object* ewk_context_icon_database_icon_object_add(Ewk_Context* context, const char* uri, Evas* canvas);

/**
 * Requests for deleting all local file systems.
 *
 * @param context context object
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_local_file_system_all_delete(Ewk_Context *context);

/**
 * Requests for deleting local file system for origin.
 *
 * @param context context object
 * @param origin local file system origin
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_local_file_system_delete(Ewk_Context *context, Ewk_Security_Origin *origin);

 /**
 * Requests for getting local file system origins.
 *
 * @param context context object
 * @param result_callback callback to get local file system origins
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 *
 * @see ewk_context_origins_free
 */
EAPI Eina_Bool ewk_context_local_file_system_origins_get(const Ewk_Context *context, Ewk_Local_File_System_Origins_Get_Callback callback, void *user_data);

/**
 * Requests for deleting all web databases.
 *
 * @param context context object
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_web_database_delete_all(Ewk_Context* context);

/**
 * Requests for deleting web databases for origin.
 *
 * @param context context object
 * @param origin database origin
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_web_database_delete(Ewk_Context* context, Ewk_Security_Origin* origin);

/**
 * Requests for getting web database origins.
 *
 * @param context context object
 * @param result_callback callback to get web database origins
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 *
 * @see ewk_context_origins_free
 */
EAPI Eina_Bool ewk_context_web_database_origins_get( Ewk_Context* context, Ewk_Web_Database_Origins_Get_Callback callback, void* user_data);

/**
 * Requests for getting web database quota for origin.
 *
 * @param context context object
 * @param result_callback callback to get web database quota
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 * @param origin database origin
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_web_database_quota_for_origin_get(Ewk_Context* context, Ewk_Web_Database_Quota_Get_Callback callback, void* user_data, Ewk_Security_Origin* origin);

/**
 * Requests for setting web database quota for origin.
 *
 * @param context context object
 * @param origin database origin
 * @param quota size of quota
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_web_database_quota_for_origin_set(Ewk_Context* context, Ewk_Security_Origin* origin, uint64_t quota);

/**
 * Requests for getting web database usage for origin.
 *
 * @param context context object
 * @param result_callback callback to get web database usage
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 * @param origin database origin
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_web_database_usage_for_origin_get(Ewk_Context* context, Ewk_Web_Database_Usage_Get_Callback callback, void* user_data, Ewk_Security_Origin* origin);

/**
 * Requests for deleting all web indexed databases.
 *
 * @param context context object
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_web_indexed_database_delete_all(Ewk_Context* context);
/**
 * Deletes all web storage.
 *
 * @param context context object
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_web_storage_delete_all(Ewk_Context* context);

/**
 * Deletes origin that is stored in web storage db.
 *
 * @param context context object
 * @param origin origin of db
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_web_storage_origin_delete(Ewk_Context* context, Ewk_Security_Origin* origin);

/**
 * Gets list of origins that is stored in web storage db.
 *
 * This function allocates memory for context structure made from callback and user_data.
 *
 * @param context context object
 * @param result_callback callback to get web storage origins
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure
 *
 * @See ewk_context_origins_free()
 */
EAPI Eina_Bool ewk_context_web_storage_origins_get(Ewk_Context* context, Ewk_Web_Storage_Origins_Get_Callback callback, void* user_data);
/**
 * Gets usage of web storage for certain origin.
 *
 * This function allocates memory for context structure made from callback and user_data.
 *
 * @param context context object
 * @param origin security origin
 * @param callback callback to get web storage usage
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_web_storage_usage_for_origin_get(Ewk_Context* context, Ewk_Security_Origin* origin, Ewk_Web_Storage_Usage_Get_Callback callback, void* user_data);

/**
 * Requests for setting soup data path(soup data include cookie and cache).
 *
 * @param context context object
 * @param path soup data path to set
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_soup_data_directory_set(Ewk_Context* context, const char* path);

/**
 * \enum    Ewk_Cache_Model
 *
 * @brief   Contains option for cache model
 */
enum Ewk_Cache_Model {
    /// Use the smallest cache capacity.
    EWK_CACHE_MODEL_DOCUMENT_VIEWER,
    /// Use bigger cache capacity than EWK_CACHE_MODEL_DOCUMENT_VIEWER.
    EWK_CACHE_MODEL_DOCUMENT_BROWSER,
    /// Use the biggest cache capacity.
    EWK_CACHE_MODEL_PRIMARY_WEBBROWSER
};

/// Creates a type name for the Ewk_Cache_Model.
typedef enum Ewk_Cache_Model Ewk_Cache_Model;

/**
* Requests to set the cache model
*
* @param context context object
* @param model cache model
*
* @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
*/
EAPI Eina_Bool ewk_context_cache_model_set(Ewk_Context* context, Ewk_Cache_Model model);

/**
* Returns the cache model type
*
* @param context context object
*
* @return Ewk_Cache_Model
*/
EAPI Ewk_Cache_Model ewk_context_cache_model_get(Ewk_Context* context);

/**
* Toggles the cache enable and disable
*
* @param context context object
* @param enable or disable cache
*
* @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
*/
EAPI Eina_Bool ewk_context_cache_disabled_set(Ewk_Context* ewkContext, Eina_Bool cacheDisabled);

/**
* Queries if the cache is enabled
*
* @param context context object
*
* @return @c EINA_TRUE is cache is enabled or @c EINA_FALSE otherwise
*/
EAPI Eina_Bool ewk_context_cache_disabled_get(const Ewk_Context* ewkContext);

/**
 * Request to set certifcate file
 *
 * @param context context object
 * @param certificate_file is path where certificate file is stored
 *
 * @return @c EINA_TRUE is cache is enabled or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_context_certificate_file_set(Ewk_Context *context, const char *certificate_file);

/**
* Request to get certifcate file
*
* @param context context object
*
* @return @c certificate_file is path which is set during ewk_context_certificate_file_set or @c NULL otherwise
*/
EAPI const char* ewk_context_certificate_file_get(const Ewk_Context *context);

/**
 * Requests to clear cache
 *
 * @param context context object
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_context_cache_clear(Ewk_Context* ewkContext);

/**
 * Sets callback for started download.
 *
 * @param context context object
 * @param callback callback for started download
 * @param user_data user data
 */
EAPI void ewk_context_did_start_download_callback_set(Ewk_Context* context, Ewk_Context_Did_Start_Download_Callback callback, void* user_data);

/*
 * Sets callback for overriding mime type
 *
 * @param callback callback to be invoked whenver the mime type can be overriden
 */
EAPI void ewk_context_mime_override_callback_set(Ewk_Context_Override_Mime_For_Url_Callback callback);

/**
 * @typedef Ewk_Vibration_Client_Vibrate_Cb Ewk_Vibration_Client_Vibrate_Cb
 * @brief Type definition for a function that will be called back when vibrate
 * request receiveed from the vibration controller.
 */
typedef void (*Ewk_Vibration_Client_Vibrate_Cb)(uint64_t vibration_time, void *user_data);

/**
 * @typedef Ewk_Vibration_Client_Vibration_Cancel_Cb Ewk_Vibration_Client_Vibration_Cancel_Cb
 * @brief Type definition for a function that will be called back when cancel
 * vibration request receiveed from the vibration controller.
 */
typedef void (*Ewk_Vibration_Client_Vibration_Cancel_Cb)(void *user_data);

/**
 * Increases the reference count of the given object.
 *
 * @param context context object to increase the reference count
 *
 * @return Ewk_Context object on success or @c NULL on failure
 */
EAPI Ewk_Context *ewk_context_ref(Ewk_Context *context);

/**
 * Decreases the reference count of the given object, possibly freeing it.
 *
 * When the reference count it's reached 0, the Ewk_Context is freed.
 *
 * @param context context object to decrease the reference count
 */
EAPI void ewk_context_unref(Ewk_Context *context);
/**
 * Gets default Ewk_Context instance.
 *
 * The returned Ewk_Context object @b should not be unref'ed if application
 * does not call ewk_context_ref() for that.
 *
 * @return Ewk_Context object.
 */
EAPI Ewk_Context *ewk_context_default_get(void);

/**
 * Creates a new Ewk_Context.
 *
 * The returned Ewk_Context object @b should be unref'ed after use.
 *
 * @return Ewk_Context object on success or @c NULL on failure
 *
 * @see ewk_context_unref
 * @see ewk_context_new_with_injected_bundle_path
 */
EAPI Ewk_Context *ewk_context_new(void);

/**
 * Creates a new Ewk_Context.
 *
 * The returned Ewk_Context object @b should be unref'ed after use.
 *
 * Ewk view based on chromium engine does not support this API
 * Avoid using this API
 *
 * @param path path of injected bundle library
 *
 * @return Ewk_Context object on success or @c NULL on failure
 *
 * @see ewk_context_unref
 * @see ewk_context_new
 */
EAPI Ewk_Context *ewk_context_new_with_injected_bundle_path(const char *path);

/*
 * Gets the cookie manager instance for this @a context.
 *
 * @param context context object to query.
 *
 * @return Ewk_Cookie_Manager object instance or @c NULL in case of failure.
 */
EAPI Ewk_Cookie_Manager *ewk_context_cookie_manager_get(const Ewk_Context *context);

/**
 * Sets additional plugin directory.
 *
 * @param context context object
 * @param path the directory to be set
 *
 * @return @c EINA_TRUE if the directory was set, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_context_additional_plugin_path_set(Ewk_Context *context, const char *path);

/**
 * Sets vibration client callbacks to handle the tactile feedback in the form of
 * vibration in the client application when the content asks for vibration.
 *
 * To stop listening for vibration events, you may call this function with @c
 * NULL for the callbacks.
 *
 * @param context context object to set vibration client callbacks.
 * @param vibrate The function to call when the vibrate request received from the
 *        controller (may be @c NULL).
 * @param cancel The function to call when the cancel vibration request received
 *        from the controller (may be @c NULL).
 * @param data User data (may be @c NULL).
 */
EAPI void ewk_context_vibration_client_callbacks_set(Ewk_Context *context, Ewk_Vibration_Client_Vibrate_Cb vibrate, Ewk_Vibration_Client_Vibration_Cancel_Cb cancel, void *data);

/**
 * Sets memory saving mode.
 *
 * @param context context object
 * @param enable or disable memory saving mode
 *
 */
EAPI void ewk_context_memory_saving_mode_set(Ewk_Context *context, Eina_Bool mode);

struct Ewk_Password_Data {
    char* url;
    Eina_Bool useFingerprint;
};

/**
 * Clear password data
 *
 * @param context context object
 *
 */
EAPI void ewk_context_form_password_data_clear(Ewk_Context* context);

/**
 * Deletes password data from DB for given URL
 *
 * The API will delete the a password data from DB.
 *
 * @param const char* with url
 *
 * @see ewk_context_form_password_data_update
 * @see ewk_context_form_password_data_list_free
 * @see ewk_context_form_password_data_list_get
 */
EAPI void ewk_context_form_password_data_delete(Ewk_Context* context, const char* url);

/**
 * Get all password url list
 *
 * The API will delete the a password data list only from the memory.
 * To remove the password data for URL permenantly, use ewk_context_form_password_data_delete
 *
 * @param Eina_List with Ewk_Password_Data
 *
 * @see ewk_context_form_password_data_update
 * @see ewk_context_form_password_data_delete
 * @see ewk_context_form_password_data_list_free
 */
EAPI Eina_List* ewk_context_form_password_data_list_get(Ewk_Context* context);

/**
 * Deletes a given password data list
 *
 * The API will delete the a password data list only from the memory.
 * To remove the password data for URL permenantly, use ewk_context_form_password_data_delete
 *
 * @param Eina_List with Ewk_Password_Data
 *
 * @see ewk_context_form_password_data_update
 * @see ewk_context_form_password_data_delete
 * @see ewk_context_form_password_data_list_get
 */
EAPI void ewk_context_form_password_data_list_free(Ewk_Context* context, Eina_List* list);

/**
 * Clear candidate data
 *
 * @param context context object
 *
 */
EAPI void ewk_context_form_candidate_data_clear(Ewk_Context* context);

/**
 * Gets a list of all existing profiles
 *
 * The obtained profile must be deleted by ewk_autofill_profile_delete.
 * @param context context object
 *
 * @return @c Eina_List of Ewk_Autofill_Profile @c NULL otherwise
 * @see ewk_autofill_profile_delete
 */
EAPI Eina_List* ewk_context_form_autofill_profile_get_all(Ewk_Context* context);

/**
 * Gets the existing profile for given index
 *
 * The obtained profile must be deleted by ewk_autofill_profile_delete.
 *
 * @param context context object
 * @param profile id
 *
 * @return @c Ewk_Autofill_Profile if profile exists, @c NULL otherwise
 * @see ewk_autofill_profile_delete
 */
EAPI Ewk_Autofill_Profile* ewk_context_form_autofill_profile_get(Ewk_Context* context, unsigned id);

/**
 * Sets the given profile for the given id
 *
 * Data can be added to the created profile by ewk_autofill_profile_data_set.
 *
 * @param context context object
 * @param profile id
 * @param profile Ewk_Autofill_Profile
 *
 * @return @c EINA_TRUE if the profile data is set successfully, @c EINA_FALSE otherwise
 * @see ewk_autofill_profile_new
 * @see ewk_context_form_autofill_profile_add
 */
EAPI Eina_Bool ewk_context_form_autofill_profile_set(Ewk_Context* context, unsigned id, Ewk_Autofill_Profile* profile);

/**
 * Saves the created profile into permenant storage
 *
 * The profile used to save must be created by ewk_autofill_profile_new.
 * Data can be added to the created profile by ewk_autofill_profile_data_set.
 *
 * @param context context object
 * @param profile Ewk_Autofill_Profile
 *
 * @return @c EINA_TRUE if the profile data is saved successfully, @c EINA_FALSE otherwise
 * @see ewk_autofill_profile_new
 */
EAPI Eina_Bool ewk_context_form_autofill_profile_add(Ewk_Context* context, Ewk_Autofill_Profile* profile);

/**
 * Removes Autofill Form profile completely
 *
 * @param context context object
 * @param index profile id
 *
 * @return @c EINA_TRUE if the profile data is removed successfully, @c EINA_FALSE otherwise
 * @see ewk_context_form_autofill_profile_get_all
 */
EAPI Eina_Bool ewk_context_form_autofill_profile_remove(Ewk_Context* context, unsigned id);

 /**
 * Toggles tizen extensible api enable and disable
 *
 * @param context context object
 * @param extensible_api extensible API name of every kind
 * @param enable enable or disable tizen extensible api
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_context_tizen_extensible_api_string_set(Ewk_Context *context,  const char *extensible_api, Eina_Bool enable);

/**
 * Toggles tizen extensible api enable and disable
 *
 * @param context context object
 * @param extensibleAPI extensible API of every kind
 * @param enable enable or disable tizen extensible api
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_context_tizen_extensible_api_set(Ewk_Context *ewkContext, Ewk_Extensible_API extensibleAPI, Eina_Bool enable);

/**
 * Get tizen extensible api enable state
 *
 * @param context context object
 * @param extensibleAPI extensible API of every kind
 *  *
 * @return @c EINA_TRUE if the extensibleAPI set as true or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_context_tizen_extensible_api_get(Ewk_Context *ewkContext, Ewk_Extensible_API extensibleAPI);

/**
 * Reset storage path such as web storage, web database, application cache and so on
 *
 * @param context context object
 *
 */
EAPI void ewk_context_storage_path_reset(Ewk_Context* ewkContext);

/**
 * Sets the given id for the pixmap
 *
 * @param context context object
 * @param pixmap id
 *
 * @return @c EINA_TRUE if the pixmap set successfully, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_context_pixmap_set(Ewk_Context* context, int pixmap);

/**
 * Start the inspector server
 *
 * @param context context object
 * @param port number
 *
 * @return @c return the port number
 */
EAPI unsigned int ewk_context_inspector_server_start(Ewk_Context* context, unsigned int port);

/**
 * Stop the inspector server
 *
 * @param context context object
  *
 * @return @c EINA_TRUE if the inspector server stop set successfully, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_context_inspector_server_stop(Ewk_Context* context);


///------- belows are extension of chromium-ewk ---------------------------

/**
 * Gets the id for the pixmap
 *
 * @param context context object
 *
 * @return @c id for the pixmap. On error default return is 0.
 */
EAPI int ewk_context_pixmap_get(Ewk_Context *context);

EAPI void ewk_send_widget_info(Ewk_Context *context, int widget_handle, double scale, const char *theme, const char *encodedBundle);

/**
 * Gets the application cache manager instance for this @a context.
 *
 * @param context context object to query.
 *
 * @return Ewk_Cookie_Manager object instance or @c NULL in case of failure.
 */
EAPI Ewk_Application_Cache_Manager *ewk_context_application_cache_manager_get(const Ewk_Context *context);

/**
 * Gets the favicon database instance for this @a context.
 *
 * @param context context object to query.
 *
 * @return Ewk_Favicon_Database object instance or @c NULL in case of failure.
 */
EAPI Ewk_Favicon_Database *ewk_context_favicon_database_get(const Ewk_Context *context);

/**
 * Clears HTTP caches in local storage and all resources cached in memory
 * such as images, CSS, JavaScript, XSL, and fonts for @a context.
 *
 * @param context context object to clear all resource caches
 */
EAPI void ewk_context_resource_cache_clear(Ewk_Context *context);

/**
 * Sets the favicon database directory for this @a context.
 *
 * Sets the directory path to be used to store the favicons database
 * for @a context on disk. Passing @c NULL as @a directory_path will
 * result in using the default directory for the platform.
 *
 * Calling this method also means enabling the favicons database for
 * its use from the applications, it is therefore expected to be
 * called only once. Further calls for the same instance of
 * @a context will not have any effect.
 *
 * @param context context object to update
 * @param directory_path database directory path to set
 *
 * @return @c EINA_TRUE if successful, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_context_favicon_database_directory_set(Ewk_Context *context, const char *directory_path);

/**
 * Sets the list of preferred languages.
 *
 * Sets the list of preferred langages. This list will be used to build the "Accept-Language"
 * header that will be included in the network requests.
 * Client can pass @c NULL for languages to clear what is set before.
 *
 * @param languages the list of preferred languages (char* as data) or @c NULL
 *
 * @note all contexts will be affected.
 */
EAPI void ewk_context_preferred_languages_set(Eina_List *languages);

/**
 * Gets the storage manager instance for this @a context.
 *
 * @param context context object to query.
 *
 * @return Ewk_Storage_Manager object instance or @c NULL in case of failure.
 */
EAPI Ewk_Storage_Manager *ewk_context_storage_manager_get(const Ewk_Context *context);

#ifdef __cplusplus
}
#endif

#endif // ewk_context_h
