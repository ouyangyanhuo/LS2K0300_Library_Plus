#include "zf_driver_file_buffer.hpp"
#include <fcntl.h>
#include <unistd.h>

#define FILE_DEFAULT_PERM    0664

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 构造函数
// 参数说明 path  文件路径
// 参数说明 flags 文件打开标志位
// 返回参数 无
// 使用示例 zf_driver_file_buffer file_obj("/dev/data", O_RDWR);
// 备注信息 写/读写模式自动创建文件并清空内容，读模式仅打开不创建
//-------------------------------------------------------------------------------------------------------------------
zf_driver_file_buffer::zf_driver_file_buffer(const char* path, int flags)
    : m_path(path), m_fd(-1)
{
    if(m_path != NULL)
    {
        if(flags & O_WRONLY || flags & O_RDWR)
        {
            m_fd = open(m_path, flags | O_CREAT | O_TRUNC, FILE_DEFAULT_PERM);
        }
        else
        {
            m_fd = open(m_path, flags);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 析构函数
// 参数说明 无
// 返回参数 无
// 使用示例 自动调用，无需手动调用
// 备注信息 自动关闭文件句柄，防止句柄资源泄露
//-------------------------------------------------------------------------------------------------------------------
zf_driver_file_buffer::~zf_driver_file_buffer()
{
    if(m_fd >= 0)
    {
        close(m_fd);
        m_fd = -1;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 重新设置文件路径并打开文件
// 参数说明 path  文件路径
// 参数说明 flags 文件打开标志位
// 返回参数 无
// 使用示例 file_obj.set_path("/dev/new_data", O_WRONLY);
// 备注信息 先关闭原文件句柄，再重新打开新路径文件
//-------------------------------------------------------------------------------------------------------------------
void zf_driver_file_buffer::set_path(const char* path, int flags)
{
    if(m_fd >= 0) close(m_fd);
    m_path = path;
    m_fd = -1;
    if(m_path != NULL)
    {
        if(flags & O_WRONLY || flags & O_RDWR)
        {
            m_fd = open(m_path, flags | O_CREAT | O_TRUNC, FILE_DEFAULT_PERM);
        }
        else
        {
            m_fd = open(m_path, flags);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 从文件读取指定长度数据
// 参数说明 buf  数据接收缓冲区指针
// 参数说明 len  需要读取的数据长度
// 返回参数 int8 0-读取成功  -1-读取失败
// 使用示例 file_obj.read_buff(data_buf, sizeof(data_buf));
// 备注信息 必须读取满指定长度才算成功，否则判定为失败
//-------------------------------------------------------------------------------------------------------------------
int8 zf_driver_file_buffer::read_buff(uint8 *buf, int32 len)
{
    if(m_fd < 0 || buf == NULL || len <= 0) return -1;
    ssize_t r_len = read(m_fd, buf, len);
    return (r_len > 0 && r_len == len) ? 0 : -1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 向文件写入指定长度数据
// 参数说明 buf  待写入数据缓冲区指针
// 参数说明 len  需要写入的数据长度
// 返回参数 int8 0-写入成功  -1-写入失败
// 使用示例 file_obj.write_buff(data_buf, sizeof(data_buf));
// 备注信息 必须写入满指定长度才算成功，否则判定为失败
//-------------------------------------------------------------------------------------------------------------------
int8 zf_driver_file_buffer::write_buff(uint8 *buf, int32 len)
{
    if(m_fd < 0 || buf == NULL || len <= 0) return -1;
    ssize_t w_len = write(m_fd, buf, len);
    return (w_len > 0 && w_len == len) ? 0 : -1;
}