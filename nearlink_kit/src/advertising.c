#include "nearlink_kit.h"
#include "comm_task.h"

static uint8_t local_addr[SLE_ADDR_LEN] = {
    0x02, 0x01, 0x06, 0x05, 0x02, 0x00
};
static char local_name[] = "216520";

/* -------------------- 广播数据定义 -------------------- */
#define SLE_ADV_DATA_LEN 3
#define SLE_ADV_RSP_DATA_LEN (2 + sizeof(local_name) - 1)

/* 广播包数据内容：
 * [类型=发现等级][长度=1][普通广播等级值]
 */
static uint8_t sle_adv_data[SLE_ADV_DATA_LEN] = {
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL, 1, SLE_ANNOUNCE_LEVEL_NORMAL
};

/* 扫描响应包内容：
 * [类型=缩写设备名][长度=名称长度][设备名...]
 */
static uint8_t sle_adv_rsp_data[SLE_ADV_RSP_DATA_LEN] = {
    SLE_ADV_DATA_TYPE_SHORTENED_LOCAL_NAME, sizeof(local_name) - 1,
    // 后面拷贝 local_name
};

/* ------------------------------------------------------ */

static void sle_set_addr(void)
{
    sle_addr_t sle_addr = {0};
    sle_addr.type = SLE_ADDRESS_TYPE_PUBLIC;
    if (memcpy_s(sle_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN) != EOK) {
        osal_printk("sle_set_addr fail\r\n");
    }
    if (sle_set_local_addr(&sle_addr) == ERRCODE_SLE_SUCCESS) {
        osal_printk("sle_set_addr success\r\n");
    }
}

/* -------------------- 广播参数设置 -------------------- */
static int sle_set_default_announce_param(void)
{
    sle_announce_param_t param = {0};
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_ADV_HANDLE_DEFAULT;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;

    if (memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN) != EOK) {
        osal_printk("[SLE Adv] set sle adv param addr memcpy fail\r\n");
        return ERRCODE_MEMCPY;
    }

    return sle_set_announce_param(param.announce_handle, &param);
}

/* -------------------- 广播数据设置 -------------------- */
static int sle_set_default_announce_data(void)
{
    /* 将 local_name 写入扫描响应包后半部分 */
    if (memcpy_s(&sle_adv_rsp_data[2],
                 SLE_ADV_RSP_DATA_LEN - 2,
                 local_name,
                 strlen(local_name)) != EOK) {
        osal_printk("[SLE Adv] memcpy local_name fail\r\n");
        return ERRCODE_MEMCPY;
    }

    sle_announce_data_t data = {0};
    data.announce_data = sle_adv_data;
    data.announce_data_len = SLE_ADV_DATA_LEN;
    data.seek_rsp_data = sle_adv_rsp_data;
    data.seek_rsp_data_len = 2 + strlen(local_name);

    errcode_t ret = sle_set_announce_data(SLE_ADV_HANDLE_DEFAULT, &data);
    if (ret == ERRCODE_SUCC) {
        osal_printk("sle_set_default_announce_data success.\r\n");
    } else {
        osal_printk("sle_set_default_announce_data fail.\r\n");
    }
    return ret;
}

/* -------------------- 回调函数与初始化 -------------------- */
void sle_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("sle_announce_enable_cbk id:%02x, state:%02x\r\n", announce_id, status);
}

void sle_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("sle_announce_disable_cbk id:%02x, state:%02x\r\n", announce_id, status);
}

void sle_announce_terminal_cbk(uint32_t announce_id)
{
    osal_printk("sle_announce_terminal_cbk id:%02x\r\n", announce_id);
}

void sle_announce_register_cbks(void)
{
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.announce_enable_cb = sle_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_announce_terminal_cbk;
    sle_announce_seek_register_callbacks(&seek_cbks);
}

errcode_t sle_server_adv_init(void)
{
    osal_printk("sle_server_adv_init\r\n");
    sle_announce_register_cbks();
    sle_set_default_announce_param();
    sle_set_default_announce_data();
    sle_set_addr();
    sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    osal_printk("sle_start_announce\r\n");
    return ERRCODE_SUCC;
}
