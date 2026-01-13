#ifndef __ZF_DRIVER_PWM_HPP__
#define __ZF_DRIVER_PWM_HPP__

#include "zf_driver_file_buffer.hpp"
#include "zf_common_typedef.hpp"


// PWM设备节点 (电调/电机/舵机)
#define ZF_PWM_ESC_1               "/dev/zf_pwm_esc_1"
#define ZF_PWM_MOTOR_1             "/dev/zf_pwm_motor_1"
#define ZF_PWM_MOTOR_2             "/dev/zf_pwm_motor_2"
#define ZF_PWM_SERVO_1             "/dev/zf_pwm_servo_1"

// GPIO电机设备节点
#define ZF_GPIO_MOTOR_1            "/dev/zf_gpio_motor_1"
#define ZF_GPIO_MOTOR_2            "/dev/zf_gpio_motor_2"

// PWM设备信息结构体
struct pwm_info
{
    uint32 freq;       // PWM频率
    uint32 duty;       // PWM占空比
    uint32 duty_max;   // PWM占空比最大值
    uint32 duty_ns;    // PWM高电平时间（纳秒）
    uint32 period_ns;  // PWM周期时间（纳秒）
    uint32 clk_freq;   // 时钟频率
};

class zf_driver_pwm : public zf_driver_file_buffer
{
private:
    zf_driver_pwm(const zf_driver_pwm&) = delete;
    zf_driver_pwm& operator=(const zf_driver_pwm&) = delete;

public:
//-------------------------------------------------------------------------------------------------------------------
// 函数简介 构造函数
// 参数说明 path  PWM设备文件路径
// 参数说明 flags 文件打开标志位，默认O_RDWR读写模式
// 返回参数 无
// 使用示例 zf_driver_pwm pwm_obj("/dev/pwm");
// 备注信息 继承文件缓存操作类，实现PWM设备数据读写控制
//-------------------------------------------------------------------------------------------------------------------
    zf_driver_pwm(const char* path, int flags = O_RDWR);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 获取PWM设备信息数据
// 参数说明 pwm_info PWM信息结构体指针，用于存储读取的数据
// 返回参数 无
// 使用示例 struct pwm_info info; pwm_obj.get_dev_info(&info);
// 备注信息 二进制读取PWM设备信息，指针为空则不执行读取操作
//-------------------------------------------------------------------------------------------------------------------
    void get_dev_info(pwm_info *pwm_info);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 设置PWM输出占空比值
// 参数说明 duty  PWM占空比值，数据类型uint16
// 返回参数 无
// 使用示例 pwm_obj.set_duty(500);
// 备注信息 二进制写入占空比数值到PWM设备文件，直接控制输出占空比
//-------------------------------------------------------------------------------------------------------------------
    void set_duty(uint16 duty);

};

#endif