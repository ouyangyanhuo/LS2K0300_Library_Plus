/*---------------------------------------------------------------------
 * @file     zf_device_imu.hpp
 * @brief    IMU惯性测量单元驱动头文件
 * @date     2026/01/09
 *---------------------------------------------------------------------
 * @author   Seekfree
 *---------------------------------------------------------------------
 * @brief    IMU惯性测量单元驱动类，基于Linux IIO框架实现
 *           支持IMU660RA/RB/RC和IMU963RA型号自动识别
 *           包含加速度计、陀螺仪、磁力计数据采集功能
 *---------------------------------------------------------------------
 */

#ifndef __zf_device_imu_hpp__
#define __zf_device_imu_hpp__

#include "zf_driver_file_string.hpp"
#include "zf_common_typedef.hpp"

// IMU IIO框架路径宏定义
#define IMU_EVENT_PATH          "/sys/bus/iio/devices/iio:device1/events/in_voltage_change_en"     // IMU事件文件路径
#define IMU_ACC_X_PATH          "/sys/bus/iio/devices/iio:device1/in_accel_x_raw"                  // 加速度X轴路径
#define IMU_ACC_Y_PATH          "/sys/bus/iio/devices/iio:device1/in_accel_y_raw"                  // 加速度Y轴路径
#define IMU_ACC_Z_PATH          "/sys/bus/iio/devices/iio:device1/in_accel_z_raw"                  // 加速度Z轴路径
#define IMU_GYRO_X_PATH         "/sys/bus/iio/devices/iio:device1/in_anglvel_x_raw"                // 陀螺仪X轴路径
#define IMU_GYRO_Y_PATH         "/sys/bus/iio/devices/iio:device1/in_anglvel_y_raw"                // 陀螺仪Y轴路径
#define IMU_GYRO_Z_PATH         "/sys/bus/iio/devices/iio:device1/in_anglvel_z_raw"                // 陀螺仪Z轴路径
#define IMU_MAG_X_PATH          "/sys/bus/iio/devices/iio:device1/in_magn_x_raw"                   // 磁力计X轴路径
#define IMU_MAG_Y_PATH          "/sys/bus/iio/devices/iio:device1/in_magn_y_raw"                   // 磁力计Y轴路径
#define IMU_MAG_Z_PATH          "/sys/bus/iio/devices/iio:device1/in_magn_z_raw"                   // 磁力计Z轴路径

// IMU设备类型枚举
typedef enum
{
    DEV_NO_FIND    = 0,    // 未识别到设备
    DEV_IMU660RA   = 1,    // IMU660RA型号
    DEV_IMU660RB   = 2,    // IMU660RB型号
    DEV_IMU660RC   = 3,    // IMU660RC型号
    DEV_IMU963RA   = 4     // IMU963RA型号(带磁力计)
}imu_device_type_enum;

/*---------------------------------------------------------------------
 * @brief    IMU惯性测量单元驱动类
 * @details  继承字符串文件操作类，适配Linux IIO框架读取IMU数据
 *           支持IMU660系列和IMU963RA型号自动识别
 *           按型号动态打开文件，无重复IO操作，性能最优
 *---------------------------------------------------------------------
 */
class zf_device_imu : public zf_driver_file_string
{
private:
    int     fd_acc_x;               // 加速度X轴文件句柄
    int     fd_acc_y;               // 加速度Y轴文件句柄
    int     fd_acc_z;               // 加速度Z轴文件句柄
    int     fd_gyro_x;              // 角速度X轴文件句柄
    int     fd_gyro_y;              // 角速度Y轴文件句柄
    int     fd_gyro_z;              // 角速度Z轴文件句柄
    int     fd_mag_x;               // 磁力计X轴文件句柄
    int     fd_mag_y;               // 磁力计Y轴文件句柄
    int     fd_mag_z;               // 磁力计Z轴文件句柄

    // 私有化核心函数：通过文件句柄读取原始数据并转int16
    int16 imu_read_fd_data(int fd);
    // 私有化函数：关闭所有打开的文件句柄
    void imu_close_all_fd(void);

    // 禁用拷贝构造和赋值重载，规范约束
    zf_device_imu(const zf_device_imu&) = delete;
    zf_device_imu& operator=(const zf_device_imu&) = delete;

public:
    imu_device_type_enum   imu_type;

    /*---------------------------------------------------------------------
     * @brief    构造函数
     * @return   无
     * @example  zf_device_imu imu_dev;
     *---------------------------------------------------------------------
     */
    zf_device_imu(void);

    /*---------------------------------------------------------------------
     * @brief    析构函数
     * @return   无
     * @example  自动调用
     *---------------------------------------------------------------------
     */
    ~zf_device_imu(void);

    /*---------------------------------------------------------------------
     * @brief    获取当前识别到的IMU设备类型
     * @return   设备类型枚举值
     * @example  imu_device_type_enum type = imu_dev.get_dev_type();
     *---------------------------------------------------------------------
     */
    imu_device_type_enum get_dev_type(void);

    /*---------------------------------------------------------------------
     * @brief    读取加速度X轴原始数据
     * @return   加速度X轴原始值，未初始化返回0
     * @example  int16 ax = imu_dev.get_acc_x();
     *---------------------------------------------------------------------
     */
    int16 get_acc_x(void);

    /*---------------------------------------------------------------------
     * @brief    读取加速度Y轴原始数据
     * @return   加速度Y轴原始值，未初始化返回0
     * @example  int16 ay = imu_dev.get_acc_y();
     *---------------------------------------------------------------------
     */
    int16 get_acc_y(void);

    /*---------------------------------------------------------------------
     * @brief    读取加速度Z轴原始数据
     * @return   加速度Z轴原始值，未初始化返回0
     * @example  int16 az = imu_dev.get_acc_z();
     *---------------------------------------------------------------------
     */
    int16 get_acc_z(void);

    /*---------------------------------------------------------------------
     * @brief    读取角速度X轴原始数据
     * @return   角速度X轴原始值，未初始化返回0
     * @example  int16 gx = imu_dev.get_gyro_x();
     *---------------------------------------------------------------------
     */
    int16 get_gyro_x(void);

    /*---------------------------------------------------------------------
     * @brief    读取角速度Y轴原始数据
     * @return   角速度Y轴原始值，未初始化返回0
     * @example  int16 gy = imu_dev.get_gyro_y();
     *---------------------------------------------------------------------
     */
    int16 get_gyro_y(void);

    /*---------------------------------------------------------------------
     * @brief    读取角速度Z轴原始数据
     * @return   角速度Z轴原始值，未初始化返回0
     * @example  int16 gz = imu_dev.get_gyro_z();
     *---------------------------------------------------------------------
     */
    int16 get_gyro_z(void);

    /*---------------------------------------------------------------------
     * @brief    读取磁力计X轴原始数据
     * @return   磁力计X轴原始值，660系列返回0，未初始化返回0
     * @example  int16 mx = imu_dev.get_mag_x();
     *---------------------------------------------------------------------
     */
    int16 get_mag_x(void);

    /*---------------------------------------------------------------------
     * @brief    读取磁力计Y轴原始数据
     * @return   磁力计Y轴原始值，660系列返回0，未初始化返回0
     * @example  int16 my = imu_dev.get_mag_y();
     *---------------------------------------------------------------------
     */
    int16 get_mag_y(void);

    /*---------------------------------------------------------------------
     * @brief    读取磁力计Z轴原始数据
     * @return   磁力计Z轴原始值，660系列返回0，未初始化返回0
     * @example  int16 mz = imu_dev.get_mag_z();
     *---------------------------------------------------------------------
     */
    int16 get_mag_z(void);

    /*---------------------------------------------------------------------
     * @brief    IMU设备初始化
     * @details  核心流程：写1初始化→读该文件获型号→按型号开文件
     * @return   识别到的IMU设备类型，值对应枚举体
     * @example  imu_device_type_enum type = imu_dev.init();
     *---------------------------------------------------------------------
     */
    imu_device_type_enum init(void);
};

#endif
