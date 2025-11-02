#include "comm_task.h"
#include "nearlink_kit.h"

#define TASK_PRIO     24
#define STACK_SIZE    0x400

static int sle_server_task(const char *arg)
{
    unused(arg);

    (void)osal_msleep(5000); /* 延时5000=5s，等待SLE初始化完毕 */

    osal_printk("sle_server_task\r\n");

    /* 使能SLE */
    if (enable_sle() != ERRCODE_SUCC) {
        osal_printk("sle enbale fail !\r\n");
        return -1;
    }

    /* 注册连接管理回调函数 */
    if (sle_conn_register_cbks() != ERRCODE_SUCC) {
        osal_printk("sle conn register cbks fail !\r\n");
        return -1;
    }

    /* 注册 SSAP server 回调函数 */
    if (sle_ssaps_register_cbks() != ERRCODE_SUCC) {
        osal_printk("sle ssaps register cbks fail !\r\n");
        return -1;
    }

    /* 注册Server, 添加Service和property, 启动Service */
    if (sle_server_add() != ERRCODE_SUCC) {
        osal_printk("sle server add fail !\r\n");
        return -1;
    }

    /* 设置设备公开，并公开设备 */
    if (sle_server_adv_init() != ERRCODE_SUCC) {
        osal_printk("sle server adv fail !\r\n");
        return -1;
    }

    // uint8_t pressCount = 0;
    // while (1) {
    //     osal_msleep(500); // 延时500毫秒
    //     if (buttonPressed) {
    //         uint8_t temp[MAX_BUTTON_MESSAGE_LEN] = {0};
    //         temp[0] = pressCount;
    //         pressCount = (pressCount + 1) % MAX_BUTTON_COUNT;
    //         sle_server_notify_data(temp, MAX_BUTTON_MESSAGE_LEN);
    //         buttonPressed = false;
    //     }
    // }

    return 0;
}

static void nearlink_kit_sample(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_server_task, 0, "SLE_SERVER_TASK",
                                      STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the sle_sudoku_server_entry. */
app_run(nearlink_kit_sample);