#ifndef __ZF_DRIVER_ADC_HPP__
#define __ZF_DRIVER_ADC_HPP__

#include "zf_common_typedef.hpp"


#define ADC_CH0_PATH                   "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define ADC_CH1_PATH                   "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define ADC_CH2_PATH                   "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define ADC_CH3_PATH                   "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"
#define ADC_CH4_PATH                   "/sys/bus/iio/devices/iio:device0/in_voltage4_raw"
#define ADC_CH5_PATH                   "/sys/bus/iio/devices/iio:device0/in_voltage5_raw"
#define ADC_CH6_PATH                   "/sys/bus/iio/devices/iio:device0/in_voltage6_raw"
#define ADC_CH7_PATH                   "/sys/bus/iio/devices/iio:device0/in_voltage7_raw"
#define ADC_SCALE_PATH                 "/sys/bus/iio/devices/iio:device0/in_voltage_scale"

// 继承关系保留，兼容原有框架，内部不再使用父类文件指针
class zf_driver_adc {
private:
    zf_driver_adc(const zf_driver_adc&) = delete;            // 禁止拷贝构造
    zf_driver_adc& operator=(const zf_driver_adc&) = delete; // 禁止赋值重载

    int     fd_convert;     // ADC原始值文件描述符 (对应原ADC通道路径)
    int     fd_scale;       // ADC校准系数文件描述符
    char    read_buf[64];   // 全局读取缓存，复用提升效率，防止栈内存反复申请
    const   char *adc_path; // 保存ADC通道路径

public:
//-------------------------------------------------------------------------------------------------------------------
// 函数简介 构造函数
// 参数说明 adc_path  ADC通道文件路径
// 参数说明 mode     兼容传参，无实际作用，保留为了不修改调用代码
// 返回参数 无
// 使用示例 zf_driver_adc battery_adc(ADC_CH7_PATH);
// 备注信息 初始化打开ADC原始值文件+校准系数文件，双文件描述符只打开一次，永久复用无重复开销
//-------------------------------------------------------------------------------------------------------------------
    zf_driver_adc(const char* adc_path, const char* mode = "r");

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 析构函数
// 参数说明 无
// 返回参数 无
// 使用示例 自动调用无需手动调用
// 备注信息 自动关闭双文件描述符(fd_convert + fd_scale)，无句柄泄漏/内存泄漏，资源自动释放
//-------------------------------------------------------------------------------------------------------------------
    ~zf_driver_adc(void);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 读取ADC通道原始数值
// 参数说明 无
// 返回参数 uint16 转换后的ADC原始整型数值，读取失败返回0
// 使用示例 adc_reg = battery_adc.convert();
// 备注信息 使用lseek重置文件偏移量，保证每次读取最新实时值，彻底解决值重复问题
//-------------------------------------------------------------------------------------------------------------------
    uint16 convert(void);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 读取ADC比例校准系数
// 参数说明 无
// 返回参数 float 转换后的浮点型比例系数值，读取失败返回0.0f
// 使用示例 adc_scale = battery_adc.get_scale();
// 备注信息 固定读取ADC_SCALE_PATH路径，重置偏移量，读取稳定无脏数据
//-------------------------------------------------------------------------------------------------------------------
    float get_scale(void);
};

#endif