#include "./inc/comm_task.h"
#include "./inc/nearlink_kit.h"

#define SLE_SEEK_INTERVAL_DEFAULT       100
#define SLE_SEEK_WINDOW_DEFAULT         100

sle_announce_seek_callbacks_t seek_cbk = { 0 };
sle_addr_t device_addr = { 0 };

void sle_start_scan(void)
{
    sle_seek_param_t param = { 0 };
    param.own_addr_type = SLE_ADDRESS_TYPE_PUBLIC; // 本端地址类型
    param.filter_duplicates = 0; // 重复过滤开关，0：关闭，1：开启
    param.seek_filter_policy = SLE_SEEK_FILTER_ALLOW_ALL; // 设备发现过滤类型：允许所有人/白名单发现广播包
    param.seek_phys = SLE_SEEK_PHY_1M; // 设备发现PHY类型
    param.seek_type[0] = SLE_SEEK_ACTIVE; // 设备发现：主动/被动
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT; // 扫描间隔 Time = N * 0.125ms
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT; // 扫描窗口 Time = N * 0.125ms
    sle_set_seek_param(&param);
    sle_start_seek();
}

/* 扫描使能的回调函数 */
static void sle_seek_enable_cbk(errcode_t status)
{
    if (status != ERRCODE_SUCC) 
    {
        osal_printk ("sle seek enable failed, errcode = %d\r\n", status);
    }
}

/* 扫描结果上报的回调函数 */
static void sle_seek_result_info_cbk(sle_seek_result_info_t *result)
{
    if (result == NULL) {
        osal_printk("result is null.\r\n");
        return -1;
    }
    if (strstr((const char *)result->data, SERVER_NAME) != NULL) 
    {
        osal_printk("find server name in result.\r\n");
        memcpy_s(&device_addr, sizeof(sle_addr_t), &result->addr, sizeof(sle_addr_t));
        sle_stop_seek();
    }
}

/* 扫描关闭的回调函数 */
static void sle_seek_disable_cbk(errcode_t status)
{
    if (status != ERRCODE_SUCC)
    {
        osal_printk("sle seek disable failed, errcode = %d\r\n", status);
        return -1;
    }
    sle_connect_remote_device(&device_addr);
}

/* 注册SLE设备发现回调函数 */
errcode_t sle_seek_cbk_register(void) 
{
    seek_cbk.seek_enable_cb = sle_seek_enable_cbk;
    seek_cbk.seek_disable_cb = sle_seek_disable_cbk;
    seek_cbk.seek_result_cb = sle_seek_result_info_cbk;
    return sle_announce_seek_register_callbacks(&seek_cbk);
}