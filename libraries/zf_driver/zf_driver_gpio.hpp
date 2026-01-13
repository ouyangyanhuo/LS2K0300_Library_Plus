#ifndef __ZF_DRIVER_GPIO_HPP__
#define __ZF_DRIVER_GPIO_HPP__

#include "zf_driver_file_buffer.hpp"
#include "zf_common_typedef.hpp"

// GPIO蜂鸣器设备节点
#define ZF_GPIO_BEEP               "/dev/zf_gpio_beep"

// GPIO霍尔检测设备节点
#define ZF_GPIO_HALL_DETECTION     "/dev/zf_gpio_hall_detection"

// GPIO按键设备节点
#define ZF_GPIO_KEY_1              "/dev/zf_gpio_key_1"
#define ZF_GPIO_KEY_2              "/dev/zf_gpio_key_2"
#define ZF_GPIO_KEY_3              "/dev/zf_gpio_key_3"
#define ZF_GPIO_KEY_4              "/dev/zf_gpio_key_4"

// GPIO电机设备节点
#define ZF_GPIO_MOTOR_1            "/dev/zf_gpio_motor_1"
#define ZF_GPIO_MOTOR_2            "/dev/zf_gpio_motor_2"

class zf_driver_gpio : public zf_driver_file_buffer
{
private:
    zf_driver_gpio(const zf_driver_gpio&) = delete;
    zf_driver_gpio& operator=(const zf_driver_gpio&) = delete;

public:
//-------------------------------------------------------------------------------------------------------------------
// 函数简介 构造函数
// 参数说明 path  GPIO设备文件路径
// 参数说明 flags 文件打开标志位，默认O_RDWR读写模式
// 返回参数 无
// 使用示例 zf_driver_gpio gpio_obj("/dev/gpio_pin");
// 备注信息 继承文件缓存操作类，实现GPIO电平读写控制
//-------------------------------------------------------------------------------------------------------------------
    zf_driver_gpio(const char* path, int flags = O_RDWR);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 设置GPIO电平状态
// 参数说明 level  GPIO电平值 0-低电平  1-高电平
// 返回参数 无
// 使用示例 gpio_obj.gpio_set_level(1);
// 备注信息 数值转ASCII码写入设备文件，0->0x30,1->0x31
//-------------------------------------------------------------------------------------------------------------------
    void set_level(uint8 level);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 获取GPIO当前电平状态
// 参数说明 无
// 返回参数 uint8 GPIO电平值 0-低电平  1-高电平
// 使用示例 uint8 val = gpio_obj.gpio_get_level();
// 备注信息 从设备文件读取ASCII码并直接返回对应数值
//-------------------------------------------------------------------------------------------------------------------
    uint8 get_level(void);

};

#endif