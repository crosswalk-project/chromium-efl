#include "ewk_ipc_message.h"

#include <tizen_webview/public/tw_wrt.h>

#include "private/ewk_context_private.h"
#include "private/ewk_wrt_private.h"

// TODO(sns.park):
// The API description is unclear about who will delete it (via eina_stringshare_del()).
// Better to give clear description about the ownership/lifetime of the returned string.
// More Better is to replace Eina_Stringshare* with simple const char* (like webkit2),
// and give documentation saying "returned string is valid as long as |data| is valid"

Eina_Stringshare *ewk_ipc_wrt_message_data_type_get(const Ewk_IPC_Wrt_Message_Data *data)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(data, NULL);
    return eina_stringshare_add(data->GetType());
}

Eina_Bool ewk_ipc_wrt_message_data_type_set(Ewk_IPC_Wrt_Message_Data *data, const char *type)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(data, false);
    data->SetType(type);
    return true;
}

Eina_Stringshare *ewk_ipc_wrt_message_data_value_get(const Ewk_IPC_Wrt_Message_Data *data)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(data, NULL);
    return eina_stringshare_add(data->GetValue());
}

Eina_Bool ewk_ipc_wrt_message_data_value_set(Ewk_IPC_Wrt_Message_Data *data, const char *value)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(data, false);
    data->SetValue(value);
    return true;
}

Eina_Stringshare *ewk_ipc_wrt_message_data_id_get(const Ewk_IPC_Wrt_Message_Data *data)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(data, NULL);
    return eina_stringshare_add(data->GetId());
}

Eina_Bool ewk_ipc_wrt_message_data_id_set(Ewk_IPC_Wrt_Message_Data *data, const char *id)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(data, false);
    data->SetId(id);
    return true;
}

Eina_Stringshare *ewk_ipc_wrt_message_data_reference_id_get(const Ewk_IPC_Wrt_Message_Data *data)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(data, NULL);
    return eina_stringshare_add(data->GetReferenceId());
}

Eina_Bool ewk_ipc_wrt_message_data_reference_id_set(Ewk_IPC_Wrt_Message_Data *data, const char *reference_id)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(data, false);
    data->SetReferenceId(reference_id);
    return true;
}

Eina_Bool ewk_ipc_plugins_message_send(int routingId, const Ewk_IPC_Wrt_Message_Data* data)
{
    return tizen_webview::WrtPluginsIpc::SendMessage(routingId, *data);
}

Eina_Bool ewk_ipc_wrt_message_send(Ewk_Context* context, const Ewk_IPC_Wrt_Message_Data* data)
{
    return tizen_webview::WrtIpc::SendMessage(context, *data);
}

Eina_Bool ewk_ipc_plugins_sync_message_send(int routingId, Ewk_IPC_Wrt_Message_Data* data)
{
    return tizen_webview::WrtPluginsIpc::SendSyncMessage(routingId, data);
}


Ewk_IPC_Wrt_Message_Data *ewk_ipc_wrt_message_data_new()
{
    return new Ewk_IPC_Wrt_Message_Data();
}

void ewk_ipc_wrt_message_data_del(Ewk_IPC_Wrt_Message_Data *data)
{
    delete data;
}
