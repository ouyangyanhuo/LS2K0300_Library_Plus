/*---------------------------------------------------------------------
 * @file     zf_device_dl1x.hpp
 * @brief    DL1X测距传感器驱动头文件
 * @date     2026/01/09
 *---------------------------------------------------------------------
 * @author   Seekfree
 *---------------------------------------------------------------------
 * @brief    DL1X测距传感器驱动类，基于Linux IIO框架实现
 *           包含DL1A/DL1B型号自动识别、距离数据采集等功能
 *---------------------------------------------------------------------
 */

#ifndef __zf_device_dl1x_hpp__
#define __zf_device_dl1x_hpp__

#include "zf_driver_file_string.hpp"
#include "zf_common_typedef.hpp"

#define DL1X_EVENT_PATH         "/sys/bus/iio/devices/iio:device2/events/in_voltage_change_en"     // DL1X事件文件路径
#define DL1X_DISTANCE_PATH      "/sys/bus/iio/devices/iio:device2/events/in_distance_raw"          // DL1X距离数据路径

// DL1X设备类型枚举
enum dl1x_device_type_enum {
    NO_FIND_DEVICE = 0,
    ZF_DEVICE_DL1A = 1,
    ZF_DEVICE_DL1B = 2,
};

/*---------------------------------------------------------------------
 * @brief    DL1X测距传感器驱动类
 * @details  继承字符串文件操作类，适配Linux IIO框架读取测距数据
 *           支持DL1A/DL1B型号自动识别，无重复IO操作，性能最优
 *---------------------------------------------------------------------
 */
class zf_device_dl1x : public zf_driver_file_string
{
private:
    enum dl1x_device_type_enum  dl1x_dev_type;      // 设备类型
    int        fd_distance;                          // 距离数据文件句柄

    // 私有化核心函数：通过文件句柄读取原始数据并转int16
    int16 dl1x_read_fd_data(int fd);
    // 私有化函数：关闭所有打开的文件句柄
    void dl1x_close_all_fd(void);

    // 禁用拷贝构造和赋值重载，规范约束
    zf_device_dl1x(const zf_device_dl1x&) = delete;
    zf_device_dl1x& operator=(const zf_device_dl1x&) = delete;

public:
    /*---------------------------------------------------------------------
     * @brief    构造函数
     * @return   无
     * @example  zf_device_dl1x dl1x_dev;
     *---------------------------------------------------------------------
     */
    zf_device_dl1x(void);

    /*---------------------------------------------------------------------
     * @brief    析构函数
     * @return   无
     * @example  自动调用
     *---------------------------------------------------------------------
     */
    ~zf_device_dl1x(void);

    /*---------------------------------------------------------------------
     * @brief    获取当前识别到的DL1X设备类型
     * @return   设备类型枚举值
     * @example  enum dl1x_device_type_enum type = dl1x_dev.get_dev_type();
     *---------------------------------------------------------------------
     */
    enum dl1x_device_type_enum get_dev_type(void);

    /*---------------------------------------------------------------------
     * @brief    读取测距传感器原始距离数据
     * @return   距离原始值，未初始化返回0
     * @example  int16 distance = dl1x_dev.get_distance();
     *---------------------------------------------------------------------
     */
    int16 get_distance(void);

    /*---------------------------------------------------------------------
     * @brief    DL1X设备初始化
     * @details  核心流程：写1初始化→读该文件获型号→打开距离文件
     * @return   识别到的DL1X设备类型，值对应枚举定义
     * @example  enum dl1x_device_type_enum type = dl1x_dev.init();
     *---------------------------------------------------------------------
     */
    enum dl1x_device_type_enum init(void);

};

#endif
