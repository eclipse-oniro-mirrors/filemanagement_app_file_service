# 应用文件服务

## **简介**
应用文件服务是为应用提供文件分享和管理能力的服务，包含应用间文件分享、跨设备同应用文件分享以及跨设备跨应用文件分享的能力。
当前已具备基于分布式文件系统的跨设备同应用文件分享能力。

## **目录**
```
/foundation/filemanagement/app_file_service
├── interfaces                      // 对外接口
│   └── kits                        // 对外接口代码
```

## **说明**
### 接口说明
**表1**应用文件服务接口说明
| **接口名** | **说明** |
| --- | --- |
| createSharePath(fd: number, cid: string, callback: AsyncCallback\<string>): void \| Promise\<string> | 将文件fd与设备cid传递给分布式文件系统，创建跨设备分享路径 |
### 使用说明
- createSharePath
```
import remotefileshare from'@ohos.remotefileshare'

remotefileshare.createSharePath(fd, cid).then(function(path) {
    // promise
});

remotefileshare.createSharePath(fd, cid, function(err, path) {
    // aysnc
})
```

## **相关仓**