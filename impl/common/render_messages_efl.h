// Multiply-included file, no traditional include guard.

#include "base/values.h"
#include "ipc/ipc_message_macros.h"
#include "ipc/ipc_channel_handle.h"
#include "content/public/common/common_param_traits.h"
#include "content/public/common/referrer.h"
#include "ui/gfx/ipc/gfx_param_traits.h"
#include "API/ewk_hit_test_private.h"
#include "API/ewk_text_style_private.h"
#include "API/ewk_ipc_message_private.h"
#include "cache_params_efl.h"
#include "public/ewk_csp_header_type.h"
#include "public/ewk_hit_test.h"
#include "navigation_policy_params.h"
#include "public/ewk_ipc_message.h"
#include "renderer/print_pages_params.h"
#include "third_party/WebKit/public/web/WebNavigationPolicy.h"
#include "third_party/WebKit/public/web/WebNavigationType.h"
#include "url/gurl.h"

#include <string>

//-----------------------------------------------------------------------------
// RenderView messages
// These are messages sent from the browser to the renderer process.

// these messages belong to "chrome messages" in chromium
// we can add our own class for them but then we need to patch ipc/ipc_message_start.h
// so using same message class for these messages.
// but actual messages are different so they shouldn't mess with chrome messages
#define IPC_MESSAGE_START ChromeMsgStart

IPC_STRUCT_TRAITS_BEGIN(CacheParamsEfl)
  IPC_STRUCT_TRAITS_MEMBER(cache_total_capacity)
  IPC_STRUCT_TRAITS_MEMBER(cache_min_dead_capacity)
  IPC_STRUCT_TRAITS_MEMBER(cache_max_dead_capacity)
IPC_STRUCT_TRAITS_END()

IPC_ENUM_TRAITS(Ewk_CSP_Header_Type)

IPC_ENUM_TRAITS(blink::WebNavigationPolicy)
IPC_ENUM_TRAITS(blink::WebNavigationType)

IPC_STRUCT_TRAITS_BEGIN(NavigationPolicyParams)
  IPC_STRUCT_TRAITS_MEMBER(render_view_id)
  IPC_STRUCT_TRAITS_MEMBER(url)
  IPC_STRUCT_TRAITS_MEMBER(referrer)
  IPC_STRUCT_TRAITS_MEMBER(policy)
  IPC_STRUCT_TRAITS_MEMBER(type)
  IPC_STRUCT_TRAITS_MEMBER(should_replace_current_entry)
  IPC_STRUCT_TRAITS_MEMBER(is_main_frame)
  IPC_STRUCT_TRAITS_MEMBER(is_redirect)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(SelectionColor)
  IPC_STRUCT_TRAITS_MEMBER(r)
  IPC_STRUCT_TRAITS_MEMBER(g)
  IPC_STRUCT_TRAITS_MEMBER(b)
  IPC_STRUCT_TRAITS_MEMBER(a)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(SelectionStylePrams)
  IPC_STRUCT_TRAITS_MEMBER(underline_state)
  IPC_STRUCT_TRAITS_MEMBER(italic_state)
  IPC_STRUCT_TRAITS_MEMBER(bold_state)
  IPC_STRUCT_TRAITS_MEMBER(bg_color)
  IPC_STRUCT_TRAITS_MEMBER(color)
  IPC_STRUCT_TRAITS_MEMBER(font_size)
  IPC_STRUCT_TRAITS_MEMBER(order_list_state)
  IPC_STRUCT_TRAITS_MEMBER(un_order_list_state)
  IPC_STRUCT_TRAITS_MEMBER(text_align_center_state)
  IPC_STRUCT_TRAITS_MEMBER(text_align_left_state)
  IPC_STRUCT_TRAITS_MEMBER(text_align_right_state)
  IPC_STRUCT_TRAITS_MEMBER(text_align_full_state)
  IPC_STRUCT_TRAITS_MEMBER(has_composition)
IPC_STRUCT_TRAITS_END()

IPC_ENUM_TRAITS(Ewk_Hit_Test_Mode)
IPC_ENUM_TRAITS(Ewk_Hit_Test_Result_Context)

IPC_STRUCT_TRAITS_BEGIN(Ewk_Hit_Test::Hit_Test_Node_Data)
  IPC_STRUCT_TRAITS_MEMBER(tagName)
  IPC_STRUCT_TRAITS_MEMBER(nodeValue)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(Ewk_Hit_Test::Hit_Test_Image_Buffer)
  IPC_STRUCT_TRAITS_MEMBER(fileNameExtension)
  IPC_STRUCT_TRAITS_MEMBER(imageBitmap)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(Ewk_Hit_Test)
  IPC_STRUCT_TRAITS_MEMBER(context)
  IPC_STRUCT_TRAITS_MEMBER(linkURI)
  IPC_STRUCT_TRAITS_MEMBER(linkLabel)
  IPC_STRUCT_TRAITS_MEMBER(linkTitle)
  IPC_STRUCT_TRAITS_MEMBER(imageURI)
  IPC_STRUCT_TRAITS_MEMBER(mediaURI)
  IPC_STRUCT_TRAITS_MEMBER(isEditable)
  IPC_STRUCT_TRAITS_MEMBER(mode)
  IPC_STRUCT_TRAITS_MEMBER(nodeData)
  IPC_STRUCT_TRAITS_MEMBER(imageData)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(DidPrintPagesParams)
  IPC_STRUCT_TRAITS_MEMBER(metafile_data_handle)
  IPC_STRUCT_TRAITS_MEMBER(data_size)
  IPC_STRUCT_TRAITS_MEMBER(document_cookie)
  IPC_STRUCT_TRAITS_MEMBER(filename)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(Ewk_IPC_Wrt_Message_Data)
  IPC_STRUCT_TRAITS_MEMBER(type)
  IPC_STRUCT_TRAITS_MEMBER(value)
  IPC_STRUCT_TRAITS_MEMBER(id)
  IPC_STRUCT_TRAITS_MEMBER(reference_id)
IPC_STRUCT_TRAITS_END()

// Tells the renderer to clear the cache.
IPC_MESSAGE_CONTROL0(EflViewMsg_ClearCache)
IPC_MESSAGE_ROUTED0(EwkViewMsg_UseSettingsFont)

IPC_MESSAGE_CONTROL1(EflViewMsg_SetCache,
                     CacheParamsEfl)

// Tells the renderer to dump as much memory as it can, perhaps because we
// have memory pressure or the renderer is (or will be) paged out.  This
// should only result in purging objects we can recalculate, e.g. caches or
// JS garbage, not in purging irreplaceable objects.
IPC_MESSAGE_CONTROL0(EwkViewMsg_PurgeMemory)

IPC_MESSAGE_CONTROL4(EwkViewMsg_SetWidgetInfo,
                     int,            // result: widgetHandle
                     double,         // result: scale
                     std::string,    // result: theme
                     std::string)    // result: encodedBundle

IPC_MESSAGE_CONTROL1(EwkViewMsg_SendWrtMessage,
                     Ewk_IPC_Wrt_Message_Data /* data */);

IPC_MESSAGE_ROUTED0(EwkViewMsg_GetSelectionStyle)

IPC_MESSAGE_ROUTED2(EwkViewMsg_SetCSP,
                    std::string, /* policy */
                    Ewk_CSP_Header_Type /* header type */)

IPC_MESSAGE_ROUTED2(EwkViewMsg_SetScroll,
                    int, /* horizontal position */
                    int /* vertical position */)

IPC_MESSAGE_ROUTED3(EwkViewMsg_DoHitTest,
                    int, /* horizontal position */
                    int, /* vertical position */
                    Ewk_Hit_Test_Mode /* mode */)

IPC_MESSAGE_ROUTED3(EwkViewMsg_PrintToPdf,
                    int, /* width */
                    int, /* height */
                    base::FilePath /* file name to save pdf*/)

// from renderer to browser

IPC_MESSAGE_ROUTED1(EwkHostMsg_DidPrintPagesToPdf,
                    DidPrintPagesParams /* pdf document parameters */)

IPC_SYNC_MESSAGE_CONTROL1_1(EwkHostMsg_DecideNavigationPolicy,
                           NavigationPolicyParams,
                           bool /*handled*/)

IPC_SYNC_MESSAGE_ROUTED0_2(EwkHostMsg_GetContentSecurityPolicy,
                           std::string, /* policy */
                           Ewk_CSP_Header_Type /* header type */)

IPC_SYNC_MESSAGE_ROUTED1_1(EwkHostMsg_WrtSyncMessage,
                           Ewk_IPC_Wrt_Message_Data /* data */,
                           std::string /*result*/);

IPC_MESSAGE_ROUTED3(EwkViewMsg_Scale,
                    double, /* scale factor */
                    int, /* center x */
                    int /* center y */)

IPC_MESSAGE_ROUTED1(EwkViewMsg_PlainTextGet,
                    int /* callback id */)

IPC_MESSAGE_ROUTED2(EwkHostMsg_PlainTextGetContents,
                    std::string, /* contentText */
                    int /* callback id */)

IPC_MESSAGE_ROUTED1(EwkHostMsg_WrtMessage,
                    Ewk_IPC_Wrt_Message_Data /* data */);

IPC_MESSAGE_ROUTED2(EwkHostMsg_DidChangeContentsSize,
                    int, /* width */
                    int /* height */)

IPC_MESSAGE_ROUTED1(EwkViewMsg_SelectionTextStyleState,
                    SelectionStylePrams /* params */)

IPC_MESSAGE_ROUTED2(EwkViewMsg_SelectClosestWord,
                    int, /* x */
                    int /* y */)

IPC_MESSAGE_ROUTED2(EwkHostMsg_DidChangeMaxScrollOffset,
                    int, /*max scrollX*/
                    int  /*max scrollY*/)

IPC_MESSAGE_CONTROL3(EwkViewHostMsg_HitTestReply,
                    int, /* render_view_id */
                    Ewk_Hit_Test, /* Ewk Hit test data without node map */
                    NodeAttributesMap /* node attributes */)

IPC_MESSAGE_ROUTED1(EwkViewMsg_GetMHTMLData,
                    int /* callback id */)

IPC_MESSAGE_ROUTED2(EwkHostMsg_ReadMHTMLData,
                    std::string, /* Mhtml text */
                    int /* callback id */)

IPC_MESSAGE_ROUTED1(EwkHostMsg_DidChangePageScaleFactor,
                    double /* page scale factor */)

IPC_MESSAGE_ROUTED2(EwkHostMsg_DidChangePageScaleRange,
                    double, /* minimum page scale factor */
                    double  /* maximum page scale factor */)

IPC_MESSAGE_ROUTED1(EwkViewMsg_SetDrawsTransparentBackground,
                    bool /* enabled */)

IPC_MESSAGE_ROUTED1(EwkHostMsg_OrientationChangeEvent,
                    int /* orientation */)
