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

### 支持的第三方库

| 库名称 | 版本 | 用途 |
|--------|------|------|
| OpenCV | 4.10.0 | 图像处理、计算机视觉 |
| ncnn | - | 高性能神经网络推理框架 |
| jsoncpp | 1.9.6 | JSON 数据处理 |
| wuwu | - | 摄像头图传服务等实用功能 |

## 开发环境

### 推荐环境

- **操作系统**：Ubuntu 24.04
- **编译工具链**：LoongArch64 交叉编译工具链
- **CMake 版本**：>= 3.5.0
- **C++ 标准**：C++17

### 虚拟机环境

可以使用预先配置好的虚拟机环境：https://fmcf.cc

## 项目结构

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

## 快速开始

### 1. 基础编译

```bash
cd project/user
./build.sh
```

`build.sh` 脚本会自动完成以下操作：
- 清理 `out` 目录
- 执行 cmake 生成 Makefile
- 执行 make 编译项目
- 通过 SCP 上传可执行文件到目标设备

### 2. 手动编译

```bash
cd project/out
cmake ../user
make -j$(nproc)
```

### 3. 配置远程设备

编辑 `project/user/build.sh` 修改以下配置：

```bash
REMOTE_IP="192.168.31.161"   # 远程设备 IP
REMOTE_USER="root"            # 远程登录用户
REMOTE_PATH="/home/root/"     # 远程上传路径
```

## jsoncpp 配置

如需使用 jsoncpp 库，请执行以下操作：

### 步骤一：上传库文件

将根目录下的 `jsoncpp.tar` 文件上传到下位机。

### 步骤二：解压到目标目录

```bash
tar -xf jsoncpp.tar -C /etc
```

### 步骤三：配置环境变量

```bash
echo 'export LD_LIBRARY_PATH=/opt/jsoncpp/lib:$LD_LIBRARY_PATH' >> /etc/profile
```

完成后重启终端或执行以下命令使环境变量生效：

```bash
source /etc/profile
```

## 核心功能模块

### 硬件驱动层 (zf_driver)

| 模块 | 功能描述 |
|------|----------|
| ADC | 模数转换器驱动 |
| GPIO | 通用输入输出控制 |
| PWM | 脉宽调制输出 |
| PIT | 定时器/周期中断 |
| Encoder | 编码器接口 |
| UDP/ TCP | 网络通信协议 |
| Delay | 高精度延时 |

### 外设设备层 (zf_device)

| 设备 | 功能描述 |
|------|----------|
| DL1X | 激光雷达接口驱动 |
| IMU | 六轴/九轴传感器驱动 |
| IPS200_FB | LCD 屏幕帧缓冲驱动 |
| UVC | USB 摄像头驱动 |

### 应用组件层

#### 摄像头图传服务

集成自 wuwu 库，提供 HTTP 协议的实时视频流传输功能：

```cpp
#include "ww_camera_server.hpp"

// 创建图传服务器实例
CameraStreamServer camera_server;

// 启动服务器（默认端口 9595）
if (camera_server.start_server(9595) < 0) {
    return -1;
}

// 更新摄像头帧
cv::Mat frame = get_camera_frame();
camera_server.update_frame(frame);

// 浏览器访问 http://<开发板IP>:9595 即可查看实时画面
```

#### 逐飞助手

提供统一的硬件抽象接口，简化硬件使用流程。

## 编译配置

### 交叉编译配置

项目使用 `project/user/cross.cmake` 配置交叉编译环境：

```cmake
SET(CROSS_COMPILE 1)
SET(TOOLCHAIN_DIR "/opt/toolchain")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/loongarch64-linux-gnu-g++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/loongarch64-linux-gnu-gcc)
```

### 编译选项

- **优化级别**：-O3（最高优化）
- **线程支持**：-pthread（POSIX 线程）
- **并行计算**：-fopenmp（OpenMP 并行）
- **警告级别**：-Wall（显示所有警告）

## 代码示例

### 基础框架

```cpp
#include "zf_common_headfile.hpp"

int main(int, char**)
{
    // 初始化硬件设备
    // ...

    while (true)
    {
        // 主循环
        // ...
    }

    return 0;
}
```

### 使用 ncnn 进行推理

```cpp
#include <net.h>

// 加载模型
ncnn::Net net;
net.load_param("model.param");
net.load_model("model.bin");

// 推理
ncnn::Mat in = ncnn::Mat::from_pixels(image.data, ncnn::Mat::PIXEL_BGR, w, h);
ncnn::Mat out;
net.forward(in, out);
```

### 使用 OpenCV 处理图像

```cpp
#include <opencv2/opencv.hpp>

// 读取图像
cv::Mat img = cv::imread("image.jpg");

// 图像处理
cv::GaussianBlur(img, img, cv::Size(5, 5), 1.5);

// 显示图像（需要显示驱动支持）
cv::imshow("Image", img);
```

### 使用 jsoncpp 解析 JSON

```cpp
#include <json/json.h>

// 解析 JSON
Json::Value root;
Json::Reader reader;
reader.parse(json_string, root);

// 访问数据
std::string name = root["name"].asString();
int value = root["value"].asInt();
```

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

## 系统要求

### 开发机要求

- Ubuntu 24.04 或兼容系统
- 至少 8GB RAM
- 20GB 可用磁盘空间
- 网络连接（用于上传到目标设备）

### 目标设备要求

- LoongArch64 架构处理器
- Linux 内核支持帧缓冲设备 (fb0)
- 至少 512MB RAM
- 网络接口（用于远程调试）

## 常见问题

### Q: 编译时提示找不到 ncnn 头文件？

A: 确保已正确设置 `CMAKE_PREFIX_PATH` 或检查 `cross_lib/ncnn` 目录是否存在。

### Q: 运行时提示找不到 jsoncpp 动态库？

A: 请按照上文的 jsoncpp 配置步骤正确配置环境变量。

### Q: 如何添加自定义代码目录？

A: 在 `project/user/CMakeLists.txt` 中添加：

```cmake
include_directories(../code/CUSTOM_DIR)
aux_source_directory(../code/CUSTOM_DIR DIR_SRCS)
```

### Q: 如何修改远程设备 IP？

A: 编辑 `project/user/build.sh` 文件，修改 `REMOTE_IP` 变量。

## 许可证

本项目基于逐飞派 LS2K0300 开源库的第三方维护版本。

## 贡献

欢迎提交 Issue 和 Pull Request 来完善本项目。

## 致谢

- 逐飞派团队提供的原始开源库
- ncnn 作者 Tencent
- OpenCV 开发社区
- wuwu 库作者
- 所有贡献者和使用者

## 联系方式

如有问题或建议，请通过以下方式联系：

- 项目地址：https://fmcf.cc
- 问题反馈：提交 GitHub Issue

---

**LS2K0300 Library Plus** - 让嵌入式开发更简单！
