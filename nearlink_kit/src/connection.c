#include "./inc/nearlink_kit.h"
#include "./inc/comm_task.h"

/* SERVER */

uint16_t sle_conn_id = 0;

static void sle_ser_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                             sle_acb_state_t conn_state, sle_pair_state_t pair_state,
                                             sle_disc_reason_t disc_reason)
{
    osal_printk(
        "sle_connect_state_changed_cbk conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x, "
        "addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
        conn_id, conn_state, pair_state, disc_reason,
        addr->addr[0],  // 打印地址0
        addr->addr[1],  // 打印地址1
        addr->addr[2],  // 打印地址2
        addr->addr[3],  // 打印地址3
        addr->addr[4],  // 打印地址4
        addr->addr[5]); // 打印地址5

    if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
        osal_printk("sle_start_announce after disconnect\r\n");
    }
}

static void sle_ser_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("sle_pair_complete_cbk conn_id:0x%02x, status:0x%x, addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n", conn_id,
                status,
                addr->addr[0],  // 打印地址0
                addr->addr[1],  // 打印地址1
                addr->addr[2],  // 打印地址2
                addr->addr[3],  // 打印地址3
                addr->addr[4],  // 打印地址4
                addr->addr[5]); // 打印地址5
}

errcode_t sle_ser_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_ser_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_ser_pair_complete_cbk;
    return sle_connection_register_callbacks(&conn_cbks);
}

/* CLIENT */

static void sle_cli_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr, 
                                             sle_acb_state_t conn_state, sle_pair_state_t pair_state, 
                                             sle_disc_reason_t disc_reason)
{
    unused(pair_state);
    osal_printk("sle conn state changed disc_reason:0x%x\r\n", disc_reason);
    sle_conn_id = conn_id;
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("SLE_ACB_STATE_CONNECTED\r\n");
        sle_pair_remote_device(addr);
        /* ssap 信息交换结构体 */
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(0, conn_id, &info);

    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("SLE_ACB_STATE_NONE\r\n");
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("SLE_ACB_STATE_DISCONNECTED\r\n");
        sle_remove_paired_remote_device(addr);
        sle_start_scan();
    } else {
        osal_printk("sle status error\r\n");
    }
}

static void sle_cli_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("sle pair complete conn_id:0x%02x, status:0x%x\r\n", conn_id, status);
    osal_printk("sle pair complete addr:%02x:**:**:**:%02x:%02x\r\n", addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);
}
                                             
errcode_t sle_cli_connect_cbk_register(void)
{
    sle_connection_callbacks_t connect_cbk = { 0 };
    connect_cbk.connect_state_changed_cb = sle_cli_connect_state_changed_cbk;
    connect_cbk.pair_complete_cb = sle_cli_pair_complete_cbk;
    return sle_connection_register_callbacks(&connect_cbk);
}