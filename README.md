# LS2K0300 Library Plus

## 项目简介

LS2K0300 Library Plus 是逐飞派 LS2K0300 开源库的第三方维护版本。相比逐飞官方版本，本库拥有更强的拓展性和更丰富的功能支持，特别针对算法开发和机器视觉应用进行了优化。

## 主要特性

### 核心优势

- **第三方维护版本**：基于逐飞官方库，由社区维护更新
- **更丰富的第三方库支持**：内置 jsoncpp 和 ncnn 深度学习推理框架
- **免配置 OpenCV**：预编译 OpenCV 库，无需复杂配置即可使用
- **wuwu 库功能适配**：集成 wuwu 库的优秀功能，如摄像头图传服务
- **模块化重构**：重构了多个核心模块，代码结构更清晰

## 与官方版本的差异

| 特性 | 官方版本 | 本版本 |
|------|----------|--------|
| jsoncpp 支持 | ❌ | ✅ |
| ncnn 支持 | ❌ | ✅ |
| OpenCV 配置 | 手动配置 | 预编译免配置 |
| wuwu 库适配 | ❌ | ✅ |
| 模块化程度 | 基础 | 重构优化 |
| 代码结构 | 标准 | 更清晰的分层 |
| 摄像头图传 | 基础 | HTTP 图传服务 |

## 开发环境

### 环境需求

- **操作系统**：Ubuntu 24.04
- **编译工具链**：LoongArch64 交叉编译工具链
- **CMake 版本**：>= 3.5.0
- **C++ 标准**：C++17

### 预先配置完成的环境下载

可以使用预先配置好的虚拟机环境：

下载地址：[https://fmcf.cc](https://fmcf.cc)

密码：`1`

## jsoncpp 配置

如需使用 jsoncpp 库，请执行以下操作：

### 步骤一：上传库文件

将根目录下的 `jsoncpp.tar` 文件上传到下位机。
```
scp -O jsoncpp.tar root@[IP地址]:/opt
```

### 步骤二：解压到目标目录

```bash
cd /opt
tar -xf jsoncpp.tar
```

### 步骤三：配置环境变量

```bash
echo 'export LD_LIBRARY_PATH=/opt/jsoncpp/lib:$LD_LIBRARY_PATH' >> /etc/profile
```

完成后重启终端或执行以下命令使环境变量生效：

```bash
source /etc/profile
```

## 注意事项

### 摄像头畸变矫正参数配置

使用 USB 摄像头功能时，**开发者需要根据自己使用的摄像头自行修改畸变矫正参数**。

相关文件：`libraries/zf_device/zf_device_uvc.cpp` 的第 268-280 行

```cpp
// 摄像头内参矩阵 (标定结果)
Mat camera_matrix = (Mat_<double>(3, 3) <<
    109.915595, 0.000000, 148.328795,
    0.000000, 110.012567, 96.916432,
    0.000000, 0.000000, 1.000000
);
// 畸变系数 (k1, k2, p1, p2, k3)
Mat dist_coeffs = (Mat_<double>(1, 5) <<
    -0.036486,
    -0.021205,
    -0.000749,
    0.001006,
    0.003599
);
```

**矫正工具**：https://gitee.com/Magnetokuwan/cam_distortion_correction

请使用上述工具对您的摄像头进行标定，获取准确的内参矩阵和畸变系数，然后替换代码中的默认值。

## 许可证

本项目基于 **GPL-3.0 协议** 开源。详见 [LICENSE](LICENSE) 文件。

## 贡献

欢迎提交 Issue 和 Pull Request 来完善本项目。

## 致谢

- 逐飞派团队提供的原始开源库
- [wuwu 库](https://gitee.com/Wuwu129/SmartCar_99Pai_OpenSource)作者 Wuwusama 及其贡献者
- ncnn 作者 Tencent
- OpenCV 开发社区
- 所有贡献者和使用者

## 其它
### 支持的第三方库

| 库名称 | 版本 | 用途 |
|--------|------|------|
| OpenCV | 4.10.0 | 图像处理、计算机视觉 |
| ncnn | - | 高性能神经网络推理框架 |
| jsoncpp | 1.9.6 | JSON 数据处理 |
| wuwu | - | 摄像头图传服务等实用功能 |

### 项目结构

```
LS2K0300_LibraryPlus/
├── cross_lib/              # 交叉编译第三方库
│   ├── jsoncpp/           # jsoncpp 库
│   │   ├── include/       # 头文件
│   │   └── lib/           # 库文件
│   ├── ncnn/             # ncnn 深度学习推理框架
│   │   ├── include/       # 头文件
│   │   └── lib/           # 库文件和配置
│   ├── opencv/            # OpenCV 图像处理库
│   │   ├── bin/           # 可执行文件
│   │   ├── include/       # 头文件
│   │   └── lib/           # 库文件
│   └── wuwu/             # wuwu 库适配
│       ├── ww_camera_server.cpp
│       └── ww_camera_server.hpp
├── libraries/             # 逐飞核心库
│   ├── zf_common/        # 公共模块
│   │   ├── zf_common_fifo.hpp      # FIFO 队列
│   │   ├── zf_common_font.hpp      # 字体资源
│   │   ├── zf_common_function.hpp  # 常用函数
│   │   ├── zf_common_headfile.hpp  # 统一头文件
│   │   └── zf_common_typedef.hpp   # 类型定义
│   ├── zf_driver/        # 硬件驱动层
│   │   ├── zf_driver_adc.hpp       # ADC 驱动
│   │   ├── zf_driver_delay.hpp     # 延时函数
│   │   ├── zf_driver_encoder.hpp   # 编码器驱动
│   │   ├── zf_driver_gpio.hpp      # GPIO 驱动
│   │   ├── zf_driver_pit.hpp      # 定时器驱动
│   │   ├── zf_driver_pwm.hpp      # PWM 驱动
│   │   ├── zf_driver_udp.hpp      # UDP 通信
│   │   └── zf_driver_tcp_client.hpp # TCP 客户端
│   ├── zf_device/        # 外设设备驱动
│   │   ├── zf_device_dl1x.hpp     # DL1X 激光雷达
│   │   ├── zf_device_imu.hpp      # IMU 惯性测量单元
│   │   ├── zf_device_ips200_fb.hpp # IPS200 屏幕
│   │   └── zf_device_uvc.hpp      # USB 摄像头
│   └── zf_components/    # 应用组件
│       ├── seekfree_assistant.hpp      # 逐飞助手
│       └── seekfree_assistant_interface.hpp # 助手接口
└── project/               # 用户项目
    ├── code/             # 用户代码目录（自定义）
    ├── out/              # 编译输出目录
    └── user/             # 项目配置和主程序
        ├── CMakeLists.txt    # CMake 构建配置
        ├── build.sh          # 一键编译上传脚本
        ├── cross.cmake       # 交叉编译配置
        └── main.cpp          # 主程序入口
```

## 联系方式

如有问题或建议，请通过以下方式联系：

- 项目地址：[Github](https://github.com/ouyangyanhuo/LS2K0300_Library_Plus) & [Gitee](https://gitee.com/magneto110/LS2K0300_Library_Plus)
- 问题反馈：提交 GitHub Issue

---

**LS2K0300 Library Plus** - 让嵌入式开发更简单！
