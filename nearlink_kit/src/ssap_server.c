#include "./inc/nearlink_kit.h"
#include "./inc/comm_task.h"

static char g_sle_uuid_app_uuid[UUID_LEN_2] = {0x0, 0x0}; // sle server app uuid
static uint8_t g_server_id = 0;                           // sle server id
static uint16_t g_service_handle = 0;                     // sle service handle
static uint16_t g_property_handle = 0;                    // sle ntf property handle

static uint8_t sle_uuid_base[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                  0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static void sle_uuid_set_base(sle_uuid_t *out)
{
    (void)memcpy_s(out->uuid, SLE_UUID_LEN, sle_uuid_base, SLE_UUID_LEN);
    out->len = UUID_LEN_2;
}

static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[14], u2); // 14代表前面不变的标准UUID定义
}

static void ssaps_read_request_cbk(uint8_t server_id,
                                   uint16_t conn_id,
                                   ssaps_req_read_cb_t *read_cb_para,
                                   errcode_t status)
{
    osal_printk("ssaps_read_request_cbk server_id:0x%x, conn_id:0x%x, handle:0x%x, type:0x%x, status:0x%x\r\n",
                server_id, conn_id, read_cb_para->handle, read_cb_para->type, status);
}

static void ssaps_write_request_cbk(uint8_t server_id,
                                    uint16_t conn_id,
                                    ssaps_req_write_cb_t *write_cb_para,
                                    errcode_t status)
{
    osal_printk("ssaps_write_request_cbk server_id:0x%x, conn_id:0x%x, handle:0x%x, status:0x%x\r\n", server_id,
                conn_id, write_cb_para->handle, status);

    osal_printk("ssaps_write_request_cbk value: ");
    for (uint16_t idx = 0; idx < write_cb_para->length; idx++) {
        osal_printk("%02x", write_cb_para->value[idx]);
    }
    osal_printk("( %d )\r\n", write_cb_para->length);
}

errcode_t sle_server_notify_data(const uint8_t *data, uint8_t len)
{
    errcode_t ret;
    ssaps_ntf_ind_by_uuid_t param = {0};
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.start_handle = g_service_handle;
    param.end_handle = g_property_handle;
    param.value_len = len;
    param.value = (uint8_t *)osal_vmalloc(len);
    if (param.value == NULL) {
        osal_printk("sle_server_notify_data value malloc fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("sle_server_notify_data memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    sle_uuid_setu2(SLE_UUID_SERVER_PROPERTY, &param.uuid);
    ret = ssaps_notify_indicate_by_uuid(g_server_id, 0xffff, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("sle_server_notify_data ssaps_notify_indicate_by_uuid fail :%x\r\n", ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, ssap_exchange_info_t *mtu_size, errcode_t status)
{
    osal_printk("ssaps_mtu_changed_cbk server_id:0x%x, conn_id:0x%x, mtu_size:0x%x, status:0x%x\r\n", server_id,
                conn_id, mtu_size->mtu_size, status);
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("ssaps_start_service_cbk server_id:0x%x, handle:0x%x, status:0x%x\r\n", server_id, handle, status);
}

errcode_t sle_ssaps_register_cbks(void)
{
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = ssaps_read_request_cbk;
    ssaps_cbk.write_request_cb = ssaps_write_request_cbk;
    return ssaps_register_callbacks(&ssaps_cbk);
}

static errcode_t sle_server_service_add(void)
{
    errcode_t ret = ERRCODE_FAIL;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, true, &g_service_handle);
    if (ret != ERRCODE_SUCC) {
        osal_printk(" sle_server_service_add fail, ret:0x%x\r\n", ret);
        return ERRCODE_FAIL;
    }

    osal_printk("sle_server_service_add service_handle: %u\r\n", g_service_handle);

    return ERRCODE_SUCC;
}

static errcode_t sle_server_property_add(void)
{
    errcode_t ret = ERRCODE_FAIL;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x0};

    property.permissions = SSAP_PERMISSION_WRITE | SSAP_PERMISSION_READ;
    property.operate_indication =
        SSAP_OPERATE_INDICATION_BIT_WRITE | SSAP_OPERATE_INDICATION_BIT_NOTIFY | SSAP_OPERATE_INDICATION_BIT_READ;
    sle_uuid_setu2(SLE_UUID_SERVER_PROPERTY, &property.uuid);

    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle);
    if (ret != ERRCODE_SUCC) {
        osal_printk("sle_server_property_add ret:0x%x\r\n", ret);
        osal_vfree(property.value);
        return ERRCODE_FAIL;
    }

    osal_printk("sle_server_property_add property_handle: %u\r\n", g_property_handle);

    descriptor.permissions = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
    descriptor.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    descriptor.value = osal_vmalloc(sizeof(ntf_value));
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE |
                                    SSAP_OPERATE_INDICATION_BIT_DESCRIPTOR_CLIENT_CONFIGURATION_WRITE;
    descriptor.value_len = sizeof(ntf_value);

    if (descriptor.value == NULL) {
        osal_printk("sle descriptor mem fail\r\n");
        osal_vfree(property.value);
        return ERRCODE_MALLOC;
    }
    if (memcpy_s(descriptor.value, sizeof(ntf_value), ntf_value, sizeof(ntf_value)) != EOK) {
        osal_printk("sle descriptor mem cpy fail\r\n");
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_MEMCPY;
    }
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SUCC) {
        osal_printk("ssaps_add_descriptor_sync fail, ret:0x%x\r\n", ret);
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_FAIL;
    }
    osal_vfree(property.value);
    osal_vfree(descriptor.value);
    return ERRCODE_SUCC;
}

errcode_t sle_server_add(void)
{
    errcode_t ret = ERRCODE_FAIL;
    sle_uuid_t app_uuid = {0};

    osal_printk("sle_server_add\r\n");
    app_uuid.len = sizeof(g_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_MEMCPY;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_server_service_add() != ERRCODE_SUCC) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_FAIL;
    }

    if (sle_server_property_add() != ERRCODE_SUCC) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_FAIL;
    }
    osal_printk("sle add service, server_id:0x%x, service_handle:0x%x, property_handle:0x%x\r\n", g_server_id,
                g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SUCC) {
        osal_printk("sle add service fail, ret:0x%x\r\n", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
