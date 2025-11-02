#ifndef _NEARLINK_KIT_H_
#define _NEARLINK_KIT_H_

#include "sle_device_discovery.h"
#include "sle_device_manager.h"
#include "sle_connection_manager.h"
#include "sle_ssap_server.h"
#include "sle_common.h"
#include "sle_errcode.h"

/* Advertising */

#define NAME_MAX_LENGTH 7                          // sle device name
#define SLE_CONN_INTV_MIN_DEFAULT 0x64             // 连接调度间隔12.5ms，单位125us
#define SLE_CONN_INTV_MAX_DEFAULT 0x64             // 连接调度间隔12.5ms，单位125us
#define SLE_ADV_INTERVAL_MIN_DEFAULT 0xC8          // 连接调度间隔25ms，单位125us
#define SLE_ADV_INTERVAL_MAX_DEFAULT 0xC8          // 连接调度间隔25ms，单位125us
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT 0x1F4 // 超时时间5000ms，单位10ms
#define SLE_CONN_MAX_LATENCY 0x1F3                 // 超时时间4990ms，单位10ms
#define SLE_ADV_TX_POWER 10                        // 广播发送功率
#define SLE_ADV_HANDLE_DEFAULT 1                   // 广播ID
#define SLE_ADV_DATA_LEN_MAX 251                   // 最大广播数据长度

// SLE 广播普通数据结构
struct sle_adv_common_value {
    uint8_t length;
    uint8_t type;
    uint8_t value;
};

// SLE 广播信道映射
typedef enum {
    SLE_ADV_CHANNEL_MAP_77 = 0x01,
    SLE_ADV_CHANNEL_MAP_78 = 0x02,
    SLE_ADV_CHANNEL_MAP_79 = 0x04,
    SLE_ADV_CHANNEL_MAP_DEFAULT = 0x07
} sle_adv_channel_map;

// SLE 广播数据类型
typedef enum {
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL = 0x01,                         /*!< 发现等级 */
    SLE_ADV_DATA_TYPE_ACCESS_MODE = 0x02,                             /*!< 接入层能力 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID = 0x03,                 /*!< 标准服务数据信息 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_128BIT_UUID = 0x04,                /*!< 自定义服务数据信息 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS = 0x05,    /*!< 完整标准服务标识列表 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_128BIT_SERVICE_UUIDS = 0x06,   /*!< 完整自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_16BIT_SERVICE_UUIDS = 0x07,  /*!< 部分标准服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_128BIT_SERVICE_UUIDS = 0x08, /*!< 部分自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_SERVICE_STRUCTURE_HASH_VALUE = 0x09,            /*!< 服务结构散列值 */
    SLE_ADV_DATA_TYPE_SHORTENED_LOCAL_NAME = 0x0A,                    /*!< 设备缩写本地名称 */
    SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME = 0x0B,                     /*!< 设备完整本地名称 */
    SLE_ADV_DATA_TYPE_TX_POWER_LEVEL = 0x0C,                          /*!< 广播发送功率 */
    SLE_ADV_DATA_TYPE_SLB_COMMUNICATION_DOMAIN = 0x0D,                /*!< SLB通信域域名 */
    SLE_ADV_DATA_TYPE_SLB_MEDIA_ACCESS_LAYER_ID = 0x0E,               /*!< SLB媒体接入层标识 */
    SLE_ADV_DATA_TYPE_EXTENDED = 0xFE,                                /*!< 数据类型扩展 */
    SLE_ADV_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA = 0xFF               /*!< 厂商自定义信息 */
} sle_adv_data_type;

extern void sle_announce_register_cbks(void);

extern errcode_t sle_server_adv_init(void);

/* Connection */

extern errcode_t sle_conn_register_cbks(void);

/* Ssap_Server */

#define SLE_UUID_SERVER_SERVICE 0xAAAA  // Service UUID
#define SLE_UUID_SERVER_PROPERTY 0xBBBB // Property UUID

#define PROPERTY_BIT_LEN 81
#define UUID_LEN_2 2

#define encode2byte_little(_ptr, data)                     \
    do {                                                   \
        *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 8); \
        *(uint8_t *)(_ptr) = (uint8_t)(data);              \
    } while (0)

extern errcode_t sle_server_add(void);

extern errcode_t sle_ssaps_register_cbks(void);

extern errcode_t sle_server_notify_data(const uint8_t *data, uint8_t len);

#endif