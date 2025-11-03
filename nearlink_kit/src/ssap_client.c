#include "./inc/nearlink_kit.h"
#include "./inc/comm_task.h"

ssapc_find_service_result_t find_service = { 0 };
ssapc_write_param_t send_param = { 0 };
ssapc_callbacks_t ssapc_cbk = { 0 };

static void sle_exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
                                                     errcode_t status)
{
    osal_printk("sle exchange_info_cbk,pair complete client id:%d status:%d\r\n",
                 client_id, status);
    osal_printk("sle exchange mtu, mtu size: %d, version: %d.\r\n", 
                param->mtu_size, param->version);
    ssapc_find_structure_param_t find_param = { 0 };
    find_param.type = SSAP_FIND_TYPE_PROPERTY;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}

static void sle_find_structure_cbk(uint8_t client_id, uint16_t conn_id,
                                                      ssapc_find_service_result_t *service,
                                                      errcode_t status)
{
    osal_printk("sle find structure cbk client: %d conn_id:%d status: %d \r\n", 
                client_id, conn_id, status);
    osal_printk("sle find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n", 
                service->start_hdl, service->end_hdl, service->uuid.len);
    find_service.start_hdl = service->start_hdl;
    find_service.end_hdl = service->end_hdl;
    memcpy_s(&find_service.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
}

static void sle_find_property_cbk(uint8_t client_id, uint16_t conn_id,
                                                     ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("sle sle_find_property_cbk, client id: %d, conn id: %d, operate ind: %d, "
                "descriptors count: %d status:%d property->handle %d\r\n", 
                client_id, conn_id, property->operate_indication,
                property->descriptors_count, status, property->handle);
    send_param.handle = property->handle;
    send_param.type = SSAP_PROPERTY_TYPE_VALUE;
}

static void sle_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
                                                          ssapc_find_structure_result_t *structure_result,
                                                          errcode_t status)
{
    unused(conn_id);
    osal_printk("sle sle_find_structure_cmp_cbk,client id:%d status:%d type:%d uuid len:%d \r\n",
                 client_id, status, structure_result->type, structure_result->uuid.len);
}

static void sle_write_cfm_cb(uint8_t client_id, uint16_t conn_id,
                                                ssapc_write_result_t *write_result, errcode_t status)
{
    osal_printk("sle sle_write_cfm_cb, conn_id:%d client id:%d status:%d handle:0x%02x type:\
        0x%02x\r\n",  conn_id, client_id, status, write_result->handle, write_result->type);
}

errcode_t sle_ssapc_cbk_register(void)
{
    ssapc_cbk.exchange_info_cb = sle_exchange_info_cbk;
    ssapc_cbk.find_structure_cb = sle_find_structure_cbk;
    ssapc_cbk.ssapc_find_property_cbk = sle_find_property_cbk;
    ssapc_cbk.find_structure_cmp_cb = sle_find_structure_cmp_cbk;
    ssapc_cbk.write_cfm_cb = sle_write_cfm_cb;
    ssapc_cbk.notification_cb = sle_notification_cb;
    ssapc_cbk.indication_cb = sle_indication_cb;
    return ssapc_register_callbacks(&ssapc_cbk);
}