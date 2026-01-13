/*---------------------------------------------------------------------
 * @file     zf_device_dl1x.cpp
 * @brief    DL1X测距传感器驱动实现文件
 * @date     2026/01/09
 *---------------------------------------------------------------------
 * @author   Seekfree
 *---------------------------------------------------------------------
 * @brief    DL1X测距传感器驱动类实现文件
 *           包含设备初始化、型号识别、距离数据采集等功能的实现
 *---------------------------------------------------------------------
 */

#include "zf_device_dl1x.hpp"

/*---------------------------------------------------------------------
 * @brief    构造函数
 * @return   无
 * @example  zf_device_dl1x dl1x_dev;
 *---------------------------------------------------------------------
 */
zf_device_dl1x::zf_device_dl1x(void) : zf_driver_file_string(NULL, "r")
{
    dl1x_dev_type = NO_FIND_DEVICE;
    fd_distance = -1;                // 距离文件句柄初始化为无效值
}

/*---------------------------------------------------------------------
 * @brief    析构函数
 * @return   无
 * @example  自动调用
 *---------------------------------------------------------------------
 */
zf_device_dl1x::~zf_device_dl1x(void)
{
    dl1x_close_all_fd();
}

/*---------------------------------------------------------------------
 * @brief    关闭所有打开的文件句柄
 * @return   无
 * @example  内部调用
 *---------------------------------------------------------------------
 */
void zf_device_dl1x::dl1x_close_all_fd(void)
{
    if(fd_distance > 0) close(fd_distance);
    fd_distance = -1;  // 重置为无效值，防止重复关闭
}

/*---------------------------------------------------------------------
 * @brief    通过文件句柄读取传感器原始数据并转int16
 * @param    fd 已打开的传感器文件句柄
 * @return   转换后的传感器原始值，句柄无效/未初始化返回0
 * @example  内部调用
 *---------------------------------------------------------------------
 */
int16 zf_device_dl1x::dl1x_read_fd_data(int fd)
{
    if(dl1x_dev_type == NO_FIND_DEVICE || fd < 0) return 0;
    char str[20] = {0};

    lseek(fd, 0, SEEK_SET);          // 关键：偏移到文件头，保证读最新值
    if(read(fd, str, sizeof(str)-1) <= 0) return 0;

    return atoi(str);
}

/*---------------------------------------------------------------------
 * @brief    DL1X设备初始化
 * @details  核心流程：写1初始化→读该文件获型号→打开距离文件
 * @return   识别到的DL1X设备类型，值对应枚举定义
 * @example  enum dl1x_device_type_enum type = dl1x_dev.init();
 *---------------------------------------------------------------------
 */
enum dl1x_device_type_enum zf_device_dl1x::init(void)
{
    int fd_event = -1;
    char read_buf[10] = {0};
    int read_val = NO_FIND_DEVICE;

    // 第一步：写入1 执行DL1X硬件初始化
    fd_event = open(DL1X_EVENT_PATH, O_RDWR);  // 读写模式打开：先写后读
    if(fd_event < 0)
    {
        std::cerr << "DL1X打开事件文件失败: " << DL1X_EVENT_PATH << ", errno:" << errno << std::endl;
        dl1x_dev_type = NO_FIND_DEVICE;
        return dl1x_dev_type;
    }
    // 写入字符'1'完成初始化，符合sysfs写入规范
    if(write(fd_event, "1", 1) != 1)
    {
        std::cerr << "DL1X写入事件文件失败, errno:" << errno << std::endl;
        close(fd_event);
        dl1x_dev_type = NO_FIND_DEVICE;
        return dl1x_dev_type;
    }

    // 第二步：读取该文件的值 获取设备型号
    lseek(fd_event, 0, SEEK_SET);  // 写后读，必须偏移到文件头
    if(read(fd_event, read_buf, sizeof(read_buf)-1) > 0)
    {
        read_val = atoi(read_buf); // 字符串转整型，直接映射枚举值
    }
    close(fd_event); // 立即关闭事件文件句柄，无泄漏
    fd_event = -1;

    // 校验读取的型号值有效性，赋值到设备类型
    if(read_val == ZF_DEVICE_DL1A || read_val == ZF_DEVICE_DL1B)
    {
        dl1x_dev_type = (enum dl1x_device_type_enum)read_val;
        // 打印数值+对应枚举名称
        switch(dl1x_dev_type)
        {
            case ZF_DEVICE_DL1A:
                std::cout << "DL1X初始化成功, 型号: " << dl1x_dev_type << " (ZF_DEVICE_DL1A)" << std::endl;
                break;
            case ZF_DEVICE_DL1B:
                std::cout << "DL1X初始化成功, 型号: " << dl1x_dev_type << " (ZF_DEVICE_DL1B)" << std::endl;
                break;
            default:
                std::cout << "DL1X初始化成功, 型号: " << dl1x_dev_type << " (未知)" << std::endl;
                break;
        }
    }
    else
    {
        std::cerr << "DL1X读取到无效型号值: " << read_val << ", 初始化失败" << std::endl;
        dl1x_dev_type = NO_FIND_DEVICE;
        return dl1x_dev_type;
    }

    // 第三步：打开距离数据文件
    fd_distance = open(DL1X_DISTANCE_PATH, O_RDONLY);
    if(fd_distance < 0)
    {
        std::cerr << "DL1X打开距离文件失败, errno:" << errno << std::endl;
        dl1x_close_all_fd();
        dl1x_dev_type = NO_FIND_DEVICE;
    }

    return dl1x_dev_type;
}

/*---------------------------------------------------------------------
 * @brief    获取当前识别到的DL1X设备类型
 * @return   设备类型枚举值
 * @example  enum dl1x_device_type_enum type = dl1x_dev.get_dev_type();
 *---------------------------------------------------------------------
 */
enum dl1x_device_type_enum zf_device_dl1x::get_dev_type(void)
{
    return dl1x_dev_type;
}

/*---------------------------------------------------------------------
 * @brief    读取测距传感器原始距离数据
 * @return   距离原始值，未初始化返回0
 * @example  int16 distance = dl1x_dev.get_distance();
 *---------------------------------------------------------------------
 */
int16 zf_device_dl1x::get_distance(void)
{
    return dl1x_read_fd_data(fd_distance);
}
