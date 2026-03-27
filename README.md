# unity_car

## 整体架构
前后端分离，unity只负责前端部分，计算全部给后端去做(所有路径规划、避障、调度计算都在 C++ 后端)

  ┌─────────────────────────────────────────────────────────┐
  │                     C++ 后端 (大脑)                      │
  │  ┌──────────┐  ┌──────────┐  ┌──────────────────────┐   │
  │  │ epoll    │  │ 调度算法  │  │ 路径规划 (A*/Dijkstra)│   │
  │  │ 网络I/O   │  │ 订单分配  │  │ 避障 / 多车协调       │   │
  │  └────┬─────┘  └────┬─────┘  └──────────┬───────────┘   │
  │       │              │                    │             │
  │       └──────────────┴────────────────────┘             │
  │                         │                               │
  │              每帧/每tick 计算控制指令                      │
  │              发送: CMD_VEL,carId,vx,vy,vz,yawRate        │
  │              发送: CMD_POS,carId,x,y,z                   │
  └─────────────────────────────┬───────────────────────────┘
                                │ TCP
                                ▼
  ┌─────────────────────────────────────────────────────────┐
  │                   Unity 前端 (躯体)                      │
  │  ┌──────────────┐    ┌──────────────┐                  │
  │  │ CarTcpClient │───▶│ 小车物理模型  │                   │
  │  │ (网络接收)    │    │ (Rigidbody/  │                   │
  │  │              │    │  WheelCollider│                 │
  │  └──────┬───────┘    └──────────────┘                  │
  │         │                                              │
  │         │ 每帧/固定频率                                  │
  │         ▼                                              │
  │  上报: STAT,carId,x,y,z,vx,vz,state                     │
  └─────────────────────────────────────────────────────────┘

## Delight points
1. single instance : made Sever and Manager single instance,so they can use their func everywhere instead of regist recallfunc or using static func with many unnecessary args
2. epoll_wait logic : 
   1. if sfd is sever_sfd(newr), we add it into epoll and wait for varify message to add to clients/unitys.and the verify part put in addressSfd func(sever.cpp) in case of newr don't send verify message and block the epoll_sever
   2. if sfd is not secer_sfd, we call addressSfd,which find whether sfd is in clients or unitys or newer need to be verify it's identify(man made protocol) 