## ADVERTISING

广播包
```

```


扫描响应包
```

```


## SSAP_SERVER

结构：    
```
SLE Server    
    └── Service (一个服务器下可以有多个服务)    
        └── Property / Characteristic (每个服务下可以有多个特征)    
            └── Descriptor (每个特征下可以有多个描述符)    
```
| 名称  | 作用 | 是否必需 |
|---|---|---|
| Server | 设备角色，本地提供服务 | ✅ 必需 |
| Service | 分类特征的功能块，如 UART 服务、电量服务 | ✅ 必需 |
| Property (特征) | 数据本体：读写通知都在这里进行 | ✅ 必需 |
| Descriptor( 描述符) | Property 的附加信息：单位、格式、名称、Notify 配置等 | 可选（但 Notify 必需 CCCD） |
