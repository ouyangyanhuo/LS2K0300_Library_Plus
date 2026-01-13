#ifndef ZF_COMMON_HEADFILE_HPP
#define ZF_COMMON_HEADFILE_HPP

//==================================================== 第三方库 ====================================================
#include <net.h>
#include <json/json.h>
#include <opencv2/opencv.hpp>
//==================================================== 系统层 ====================================================
#include <sys/socket.h>
#include <arpa/inet.h>  
#include <unistd.h>     
#include <iostream>
#include <chrono>
#include <pthread.h>
#include <time.h>
#include <fstream>
#include <cmath>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <cstdint>
#include <thread>
#include <mutex>
#include <linux/input.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <cstring>
#include <stdint.h>
#include <algorithm>
//====================================================z系统层====================================================

//====================================================开源库公共层====================================================
#include "zf_common_font.hpp"
#include "zf_common_function.hpp"
#include "zf_common_fifo.hpp"
#include "zf_common_typedef.hpp"
//====================================================开源库公共层====================================================

//===================================================芯片外设驱动层===================================================
#include "zf_driver_delay.hpp"
#include "zf_driver_file_string.hpp"
#include "zf_driver_file_buffer.hpp"
#include "zf_driver_encoder.hpp"
#include "zf_driver_gpio.hpp"
#include "zf_driver_pwm.hpp"
#include "zf_driver_pit.hpp"
#include "zf_driver_adc.hpp"
#include "zf_driver_udp.hpp"
#include "zf_driver_tcp_client.hpp"
//===================================================芯片外设驱动层===================================================

//===================================================外接设备驱动层===================================================
#include "zf_device_imu.hpp"
#include "zf_device_ips200_fb.hpp"
#include "zf_device_uvc.hpp"
//===================================================外接设备驱动层===================================================


//===================================================应用组件层===================================================
#include "seekfree_assistant.hpp"
#include "seekfree_assistant_interface.hpp"
#include "ww_camera_server.hpp"
//===================================================应用组件层===================================================

//===================================================用户自定义文件===================================================


//===================================================用户自定义文件===================================================

#endif