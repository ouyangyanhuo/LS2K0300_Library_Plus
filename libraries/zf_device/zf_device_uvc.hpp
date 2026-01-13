/*---------------------------------------------------------------------
 * @file     zf_device_uvc.hpp
 * @brief    UVC摄像头驱动头文件
 * @date     2026/01/11
 *---------------------------------------------------------------------
 * @author   Magneto
 *---------------------------------------------------------------------
 * @brief    UVC摄像头驱动类，基于V4L2 API实现
 *           支持MJPG格式图像采集，提供灰度图和RGB图获取接口
 *           包含畸变矫正功能，使用MMAP零拷贝机制
 *---------------------------------------------------------------------
 */

#ifndef _ZF_DEIVCE_UVC_HPP__
#define _ZF_DEIVCE_UVC_HPP__

#include "zf_common_typedef.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define UVC_WIDTH_DEFAULT           160             // 摄像头默认分辨率宽度
#define UVC_HEIGHT_DEFAULT          120             // 摄像头默认分辨率高度
#define UVC_FPS_DEFAULT             60              // 摄像头默认帧率
#define UVC_DEVICE                  "/dev/video0"   // 摄像头设备路径
#define UVC_PIXELFORMAT             V4L2_PIX_FMT_MJPEG  // 摄像头像素格式

/*---------------------------------------------------------------------
 * @brief    UVC摄像头数据结构
 * @details  包含摄像头所有相关数据
 *---------------------------------------------------------------------
 */
struct CamData
{
    cv::Mat frame;                  // 原始图像
    cv::Mat frame_gray;             // 灰度图像
    cv::Mat frame_rgb;              // RGB彩色图像
    uint8_t *gray_image;            // 灰度图像数组指针
    uint8_t *rgb_image;             // 彩色图像数组指针
};

extern CamData cam_data;            // UVC摄像头全局数据

/*---------------------------------------------------------------------
 * @brief    UVC摄像头驱动类
 * @details  基于V4L2 API实现，支持MJPG格式图像采集
 *           提供灰度图和RGB图获取接口，包含畸变矫正功能
 *           使用MMAP零拷贝机制提升性能
 *---------------------------------------------------------------------
 */
class CamSet
{
public:
    /*---------------------------------------------------------------------
     * @brief    配置摄像头（使用默认参数和自动曝光）
     * @param    camera_id 摄像头ID（0或1）
     * @param    debug 是否开启调试模式，开启后打印当前参数
     * @return   配置是否成功，true表示成功，false表示失败
     * @example  bool success = CamSet::configureCamera(0, true);
     *---------------------------------------------------------------------
     */
    static bool configureCamera(uint16_t camera_id, bool debug = false);

    /*---------------------------------------------------------------------
     * @brief    配置摄像头（自定义分辨率和帧率，自动曝光）
     * @param    camera_id 摄像头ID（0或1）
     * @param    width 摄像头分辨率宽度
     * @param    height 摄像头分辨率高度
     * @param    fps 摄像头帧率
     * @param    debug 是否开启调试模式，开启后打印当前参数
     * @return   配置是否成功，true表示成功，false表示失败
     * @example  bool success = CamSet::configureCamera(0, 160, 120, 60, true);
     *---------------------------------------------------------------------
     */
    static bool configureCamera(uint16_t camera_id, uint16_t width, uint16_t height,
                                 uint16_t fps, bool debug = false);

    /*---------------------------------------------------------------------
     * @brief    配置摄像头（自定义分辨率、帧率和手动曝光）
     * @param    camera_id 摄像头ID（0或1）
     * @param    width 摄像头分辨率宽度
     * @param    height 摄像头分辨率高度
     * @param    fps 摄像头帧率
     * @param    exposure 曝光值（手动曝光模式），范围根据摄像头而定
     * @param    debug 是否开启调试模式，开启后打印当前参数
     * @return   配置是否成功，true表示成功，false表示失败
     * @example  bool success = CamSet::configureCamera(0, 160, 120, 60, 180, true);
     * @note     传入曝光值后将使用手动曝光模式
     *---------------------------------------------------------------------
     */
    static bool configureCamera(uint16_t camera_id, uint16_t width, uint16_t height,
                                 uint16_t fps, int32_t exposure, bool debug = false);

    /*---------------------------------------------------------------------
     * @brief    等待刷新并获取图像帧
     * @details  从摄像头读取一帧图像，进行畸变矫正后生成灰度图和彩色图
     * @return   是否成功获取图像帧，true表示成功，false表示失败
     * @example  if (CamSet::waitRefresh())
     *---------------------------------------------------------------------
     */
    static bool waitRefresh(void);

    /*---------------------------------------------------------------------
     * @brief    获取灰度图像数据指针
     * @return   灰度图像首地址指针
     * @example  uint8_t *p_img = CamSet::getGrayImagePtr();
     *---------------------------------------------------------------------
     */
    static uint8_t* getGrayImagePtr(void);

    /*---------------------------------------------------------------------
     * @brief    获取RGB彩色图像数据指针
     * @return   RGB彩色图像首地址指针
     * @example  uint8_t *p_img = CamSet::getRgbImagePtr();
     *---------------------------------------------------------------------
     */
    static uint8_t* getRgbImagePtr(void);

    /*---------------------------------------------------------------------
     * @brief    获取摄像头当前的打开状态
     * @return   true-已打开，false-未打开
     * @example  bool status = CamSet::isCameraOpened();
     *---------------------------------------------------------------------
     */
    static bool isCameraOpened(void);

    /*---------------------------------------------------------------------
     * @brief    释放摄像头资源
     * @details  停止采集、释放缓冲区并关闭文件描述符
     * @example  CamSet::release();
     *---------------------------------------------------------------------
     */
    static void release(void);

private:
    /*---------------------------------------------------------------------
     * @brief    V4L2缓冲区结构
     *---------------------------------------------------------------------
     */
    struct Buffer {
        void* data;
        size_t size;
    };

    static int fd;                          // 摄像头文件描述符
    static bool bufExist;                   // 缓冲区是否存在
    static std::vector<Buffer> buffers;     // 缓冲区列表

    /*---------------------------------------------------------------------
     * @brief    注册内存缓冲队列并映射地址
     * @param    count 内存缓冲队列大小
     * @return   0-成功，-1-失败
     * @note     向V4L2注册缓冲队列，用户空间映射内存地址实现零拷贝
     *           count = 3 减少图像延迟
     *---------------------------------------------------------------------
     */
    static int requestBuffers(int count);

    /*---------------------------------------------------------------------
     * @brief    取消内存映射并释放缓冲队列
     *---------------------------------------------------------------------
     */
    static void destroyBuffers(void);

    /*---------------------------------------------------------------------
     * @brief    开启摄像头采集
     * @return   0-成功，-1-失败
     *---------------------------------------------------------------------
     */
    static int startCapturing(void);

    /*---------------------------------------------------------------------
     * @brief    停止摄像头采集
     *---------------------------------------------------------------------
     */
    static void stopCapturing(void);

    /*---------------------------------------------------------------------
     * @brief    捕获一帧MJPG数据
     * @param    frame 输出Mat图像
     * @return   true-成功，false-失败
     * @note     默认使用MJPG解码
     *---------------------------------------------------------------------
     */
    static bool captureFrame(cv::Mat &frame);
};

#endif
