#ifndef __ZF_DRIVER_ENCODER_HPP__
#define __ZF_DRIVER_ENCODER_HPP__

#include "zf_driver_file_buffer.hpp"
#include "zf_common_typedef.hpp"

// 正交编码器设备节点，默认使用该节点
#define ZF_ENCODER_QUAD_1          "/dev/zf_encoder_quad_1"
#define ZF_ENCODER_QUAD_2          "/dev/zf_encoder_quad_2"

// 方向编码器设备节点，如果需要使用该节点，请修改对应的设备树文件后重新编译内核
#define ZF_ENCODER_DIR_1           "/dev/zf_encoder_dir_1"
#define ZF_ENCODER_DIR_2           "/dev/zf_encoder_dir_2"

class zf_driver_encoder : public zf_driver_file_buffer
{
private:
    zf_driver_encoder(const zf_driver_encoder&) = delete;
    zf_driver_encoder& operator=(const zf_driver_encoder&) = delete;

public:
//-------------------------------------------------------------------------------------------------------------------
// 函数简介 构造函数
// 参数说明 path  设备文件路径
// 参数说明 flags 文件打开标识位，默认O_RDWR读写模式
// 返回参数 无
// 使用示例 zf_driver_encoder encoder_obj("/dev/encoder");
// 备注信息 继承文件缓存操作类，实现编码器设备文件读写
//-------------------------------------------------------------------------------------------------------------------
    zf_driver_encoder(const char* path, int flags = O_RDWR);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 获取编码器计数值
// 参数说明 无
// 返回参数 int16 编码器当前计数值
// 使用示例 int16 cnt = encoder_obj.get_count();
// 备注信息 读取设备文件数据并转换为整型计数值
//-------------------------------------------------------------------------------------------------------------------
    int16 get_count(void);

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 清零编码器计数值
// 参数说明 无
// 返回参数 无
// 使用示例 encoder_obj.clear_count();
// 备注信息 向设备文件写入0值，完成编码器计数清零操作
//-------------------------------------------------------------------------------------------------------------------
    void clear_count(void);

};

#endif