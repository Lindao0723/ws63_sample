#include "comm_task.h"
#include "nearlink_kit.h"

static void sle_connect_state_changed_cbk(uint16_t conn_id,
                                          const sle_addr_t *addr,
                                          sle_acb_state_t conn_state,
                                          sle_pair_state_t pair_state,
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

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
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

errcode_t sle_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    return sle_connection_register_callbacks(&conn_cbks);
}