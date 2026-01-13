/*---------------------------------------------------------------------
 * @file     zf_device_uvc.cpp
 * @brief    UVC摄像头驱动实现文件
 * @date     2026/01/11
 *---------------------------------------------------------------------
 * @author   Magneto
 *---------------------------------------------------------------------
 * @brief    UVC摄像头驱动类实现文件
 *           基于V4L2 API实现，包含摄像头初始化、参数配置、
 *           图像采集和畸变矫正功能的实现，使用MMAP零拷贝机制
 *---------------------------------------------------------------------
 */

#include "zf_device_uvc.hpp"

using namespace cv;

// 全局数据实例
CamData cam_data;                           // UVC摄像头全局数据实例

// 静态成员变量定义
int CamSet::fd = -1;                        // 摄像头文件描述符
bool CamSet::bufExist = false;              // 缓冲区是否存在
std::vector<CamSet::Buffer> CamSet::buffers; // 缓冲区列表

/*---------------------------------------------------------------------
 * @brief    配置摄像头（使用默认参数和自动曝光）
 * @param    camera_id 摄像头ID（0或1）
 * @param    debug 是否开启调试模式，开启后打印当前参数
 * @return   配置是否成功，true表示成功，false表示失败
 * @example  bool success = CamSet::configureCamera(0, true);
 *---------------------------------------------------------------------
 */
bool CamSet::configureCamera(uint16_t camera_id, bool debug)
{
    return configureCamera(camera_id, UVC_WIDTH_DEFAULT, UVC_HEIGHT_DEFAULT,
                          UVC_FPS_DEFAULT, debug);
}

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
bool CamSet::configureCamera(uint16_t camera_id, uint16_t width, uint16_t height,
                              uint16_t fps, bool debug)
{
    // 构建设备路径
    std::string device = "/dev/video" + std::to_string(camera_id);

    // 打开摄像头设备
    fd = open(device.c_str(), O_RDWR);
    if(fd < 0) {
        std::cerr << "无法打开" << device << std::endl;
        return false;
    }

    // 设置图像格式
    struct v4l2_format fmt = {};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = UVC_PIXELFORMAT;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;

    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        std::cerr << "摄像头格式设置失败" << std::endl;
        return false;
    }

    // 设置帧率
    struct v4l2_streamparm setparm = {};
    setparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setparm.parm.capture.timeperframe.numerator = 1;  // 分子
    setparm.parm.capture.timeperframe.denominator = fps;  // 分母，fps = 分母/分子

    if (ioctl(fd, VIDIOC_S_PARM, &setparm) == -1) {
        std::cerr << "警告: 设置帧率失败，使用默认帧率" << std::endl;
        return false;
    }

    // 设置自动曝光模式
    struct v4l2_control ctrl = {};
    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    ctrl.value = V4L2_EXPOSURE_APERTURE_PRIORITY;  // 自动曝光模式

    if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) == -1) {
        std::cerr << "警告: 设置自动曝光失败" << std::endl;
    }

    // 输出当前设置参数
    if(debug) {
        v4l2_format get_fmt = {};
        get_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (ioctl(fd, VIDIOC_G_FMT, &get_fmt) == 0) {
            std::cout << "摄像头输出尺寸: " <<
                    get_fmt.fmt.pix.width << 'x' << get_fmt.fmt.pix.height
                    << std::endl;
        }

        struct v4l2_streamparm getparm = {};
        getparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (ioctl(fd, VIDIOC_G_PARM, &getparm) == 0) {
            double actual_fps = (double)getparm.parm.capture.timeperframe.denominator /
                                getparm.parm.capture.timeperframe.numerator;
            std::cout << "帧率: " << actual_fps << " fps" << std::endl;
        }

        struct v4l2_control get_ctrl = {};
        get_ctrl.id = V4L2_CID_EXPOSURE_AUTO;
        if (ioctl(fd, VIDIOC_G_CTRL, &get_ctrl) == 0) {
            std::cout << "曝光模式: " << (get_ctrl.value == V4L2_EXPOSURE_MANUAL ? "手动" : "自动")
                      << std::endl;
        }
    }

    // 请求缓冲区
    if(requestBuffers(3) < 0) {
        return false;
    }

    // 开始采集
    if(startCapturing() < 0) {
        return false;
    }

    return true;
}

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
bool CamSet::configureCamera(uint16_t camera_id, uint16_t width, uint16_t height,
                              uint16_t fps, int32_t exposure, bool debug)
{
    // 构建设备路径
    std::string device = "/dev/video" + std::to_string(camera_id);

    // 打开摄像头设备
    fd = open(device.c_str(), O_RDWR);
    if(fd < 0) {
        std::cerr << "无法打开" << device << std::endl;
        return false;
    }

    // 设置图像格式
    struct v4l2_format fmt = {};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = UVC_PIXELFORMAT;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;

    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        std::cerr << "摄像头格式设置失败" << std::endl;
        return false;
    }

    // 设置帧率
    struct v4l2_streamparm setparm = {};
    setparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setparm.parm.capture.timeperframe.numerator = 1;  // 分子
    setparm.parm.capture.timeperframe.denominator = fps;  // 分母，fps = 分母/分子

    if (ioctl(fd, VIDIOC_S_PARM, &setparm) == -1) {
        std::cerr << "警告: 设置帧率失败，使用默认帧率" << std::endl;
        return false;
    }

    // 设置手动曝光模式
    struct v4l2_control ctrl = {};
    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    ctrl.value = V4L2_EXPOSURE_MANUAL;  // 手动曝光模式

    if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) == -1) {
        std::cerr << "警告: 设置手动曝光模式失败" << std::endl;
    }

    // 设置曝光值
    struct v4l2_control exp_ctrl = {};
    exp_ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    exp_ctrl.value = exposure;

    if (ioctl(fd, VIDIOC_S_CTRL, &exp_ctrl) == -1) {
        std::cerr << "警告: 设置曝光值失败" << std::endl;
    }

    // 输出当前设置参数
    if(debug) {
        v4l2_format get_fmt = {};
        get_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (ioctl(fd, VIDIOC_G_FMT, &get_fmt) == 0) {
            std::cout << "摄像头输出尺寸: " <<
                    get_fmt.fmt.pix.width << 'x' << get_fmt.fmt.pix.height
                    << std::endl;
        }

        struct v4l2_streamparm getparm = {};
        getparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (ioctl(fd, VIDIOC_G_PARM, &getparm) == 0) {
            double actual_fps = (double)getparm.parm.capture.timeperframe.denominator /
                                getparm.parm.capture.timeperframe.numerator;
            std::cout << "帧率: " << actual_fps << " fps" << std::endl;
        }

        struct v4l2_control get_ctrl = {};
        get_ctrl.id = V4L2_CID_EXPOSURE_AUTO;
        if (ioctl(fd, VIDIOC_G_CTRL, &get_ctrl) == 0) {
            std::cout << "曝光模式: " << (get_ctrl.value == V4L2_EXPOSURE_MANUAL ? "手动" : "自动")
                      << std::endl;
        }

        struct v4l2_control get_exp = {};
        get_exp.id = V4L2_CID_EXPOSURE_ABSOLUTE;
        if (ioctl(fd, VIDIOC_G_CTRL, &get_exp) == 0) {
            std::cout << "当前曝光值: " << get_exp.value << std::endl;
        }
    }

    // 请求缓冲区
    if(requestBuffers(3) < 0) {
        return false;
    }

    // 开始采集
    if(startCapturing() < 0) {
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------
 * @brief    等待刷新并获取图像帧
 * @details  从摄像头读取一帧图像，进行畸变矫正后生成灰度图和彩色图
 * @return   是否成功获取图像帧，true表示成功，false表示失败
 * @example  if (CamSet::waitRefresh())
 *---------------------------------------------------------------------
 */
bool CamSet::waitRefresh(void)
{
    // 捕获一帧MJPG数据
    if(!captureFrame(cam_data.frame)) {
        return false;
    }

    // 摄像头内参矩阵 (标定结果)
    Mat camera_matrix = (Mat_<double>(3, 3) <<
        109.915595, 0.000000, 148.328795,
        0.000000, 110.012567, 96.916432,
        0.000000, 0.000000, 1.000000
    );
    // 畸变系数 (k1, k2, p1, p2, k3)
    Mat dist_coeffs = (Mat_<double>(1, 5) <<
        -0.036486,
        -0.021205,
        -0.000749,
        0.001006,
        0.003599
    );

    // 应用畸变矫正
    undistort(cam_data.frame, cam_data.frame_rgb, camera_matrix, dist_coeffs);
    // 转换为灰度图
    cvtColor(cam_data.frame_rgb, cam_data.frame_gray, COLOR_BGR2GRAY);

    return true;
}

/*---------------------------------------------------------------------
 * @brief    获取灰度图像数据指针
 * @return   灰度图像首地址指针
 * @example  uint8_t *p_img = CamSet::getGrayImagePtr();
 *---------------------------------------------------------------------
 */
uint8_t* CamSet::getGrayImagePtr(void)
{
    cam_data.gray_image = reinterpret_cast<uint8_t*>(cam_data.frame_gray.ptr(0));
    return cam_data.gray_image;
}

/*---------------------------------------------------------------------
 * @brief    获取RGB彩色图像数据指针
 * @return   RGB彩色图像首地址指针
 * @example  uint8_t *p_img = CamSet::getRgbImagePtr();
 *---------------------------------------------------------------------
 */
uint8_t* CamSet::getRgbImagePtr(void)
{
    cam_data.rgb_image = reinterpret_cast<uint8_t*>(cam_data.frame_rgb.ptr(0));
    return cam_data.rgb_image;
}

/*---------------------------------------------------------------------
 * @brief    获取摄像头当前的打开状态
 * @return   true-已打开，false-未打开
 * @example  bool status = CamSet::isCameraOpened();
 *---------------------------------------------------------------------
 */
bool CamSet::isCameraOpened(void)
{
    return (fd != -1 && bufExist);
}

/*---------------------------------------------------------------------
 * @brief    释放摄像头资源
 * @details  停止采集、释放缓冲区并关闭文件描述符
 * @example  CamSet::release();
 *---------------------------------------------------------------------
 */
void CamSet::release(void)
{
    stopCapturing();
    destroyBuffers();
    if(fd != -1) {
        close(fd);
        fd = -1;
    }
    std::cout << "摄像头资源已释放" << std::endl;
}

/*---------------------------------------------------------------------
 * @brief    捕获一帧MJPG数据
 * @param    frame 输出Mat图像
 * @return   true-成功，false-失败
 * @note     默认使用MJPG解码
 *---------------------------------------------------------------------
 */
bool CamSet::captureFrame(cv::Mat &frame)
{
    if(bufExist == false) {
        std::cerr << "摄像头未初始化" << std::endl;
        return false;
    }

    struct v4l2_buffer buf = {};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    // 出队一帧
    if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
        std::cerr << "出队缓冲区失败" << std::endl;
        return false;
    }

    // MJPG解码
    std::vector<uchar> jpeg_data(static_cast<uchar*>(buffers[buf.index].data),
                                  static_cast<uchar*>(buffers[buf.index].data) + buf.bytesused);
    frame = cv::imdecode(jpeg_data, cv::IMREAD_COLOR);
    if(frame.empty()) {
        return false;
    }

    // 重新入队
    if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
        std::cerr << "入队缓冲区失败" << std::endl;
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------
 * @brief    注册内存缓冲队列并映射地址
 * @param    count 内存缓冲队列大小
 * @return   0-成功，-1-失败
 * @note     向V4L2注册缓冲队列，用户空间映射内存地址实现零拷贝
 *           count = 3 减少图像延迟
 *---------------------------------------------------------------------
 */
int CamSet::requestBuffers(int count)
{
    struct v4l2_requestbuffers req = {};
    req.count = count;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        std::cerr << "请求缓冲区失败" << std::endl;
        return -1;
    }

    if (req.count < 2) {
        std::cerr << "缓冲区不足" << std::endl;
        return -1;
    }

    bufExist = true;
    buffers.resize(req.count);

    for(size_t i = 0; i < buffers.size(); i++)
    {
        struct v4l2_buffer buf = {};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
            std::cerr << "查询缓冲区失败" << std::endl;
            return -1;
        }

        buffers[i].size = buf.length;
        buffers[i].data = mmap(NULL, buf.length,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                fd, buf.m.offset);

        if (buffers[i].data == MAP_FAILED) {
            std::cerr << "内存映射失败" << std::endl;
            return -1;
        }

        // 入队缓冲区
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            std::cerr << "初始入队缓冲区失败" << std::endl;
            return -1;
        }
    }

    return 0;
}

/*---------------------------------------------------------------------
 * @brief    取消内存映射并释放缓冲队列
 *---------------------------------------------------------------------
 */
void CamSet::destroyBuffers(void)
{
    if(bufExist == false) return;

    for(size_t i = 0; i < buffers.size(); i++) {
        if(buffers[i].data != MAP_FAILED) {
            munmap(buffers[i].data, buffers[i].size);
        }
    }

    struct v4l2_requestbuffers req = {};
    req.count = 0;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        std::cerr << "警告: 释放缓冲区失败: " << strerror(errno) << std::endl;
    } else {
        std::cout << "缓冲区已释放" << std::endl;
    }

    bufExist = false;
}

/*---------------------------------------------------------------------
 * @brief    开启摄像头采集
 * @return   0-成功，-1-失败
 *---------------------------------------------------------------------
 */
int CamSet::startCapturing(void)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        std::cout << "采集开启失败" << std::endl;
        return -1;
    }

    return 0;
}

/*---------------------------------------------------------------------
 * @brief    停止摄像头采集
 *---------------------------------------------------------------------
 */
void CamSet::stopCapturing(void)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMOFF, &type);
}
