/*---------------------------------------------------------------------
 * @file     zf_device_ips200_fb.hpp
 * @brief    IPS200显示屏驱动头文件
 * @date     2026/01/09
 *---------------------------------------------------------------------
 * @author   Magneto
 *---------------------------------------------------------------------
 * @brief    IPS200显示屏framebuffer驱动类
 *           240x320分辨率，RGB565格式，基于Linux framebuffer框架实现
 *           包含绘图、显示字符、显示数字、显示图像等功能
 *---------------------------------------------------------------------
 */

#ifndef __ZF_DEVICE_IPS200_FB_HPP__
#define __ZF_DEVICE_IPS200_FB_HPP__

#include "zf_common_typedef.hpp"

#define DEFAULT_PENCOLOR         (RGB565_RED)                          // 默认的画笔颜色
#define DEFAULT_BGCOLOR          (RGB565_WHITE)                        // 默认的背景颜色
#define FB_PATH                 "/dev/fb0"                             // framebuffer设备节点路径

/*---------------------------------------------------------------------
 * @brief    IPS200显示屏驱动类
 * @details  240x320分辨率，RGB565格式，基于Linux framebuffer框架实现
 *           提供绘图、显示字符、显示数字、显示图像等功能
 *---------------------------------------------------------------------
 */
class DisplayIps200
{
private:
    // 以下变量使用static修饰，因为所有成员函数都是static函数
    // static函数只能访问static成员变量，无法访问非static成员
    // 这样设计的目的是使用域操作符::调用函数（如DisplayIps200::clear()）
    // 而不是点运算符.（点运算符用于结构体成员访问）
    static uint16 pen_color;                   // 画笔颜色
    static uint16 bg_color;                    // 背景颜色
    static int width;                          // 屏幕宽度
    static int height;                         // 屏幕高度
    static unsigned short *screen_base;        // 映射后的显存基地址

public:

    /*---------------------------------------------------------------------
     * @brief    清屏函数
     * @return   无
     * @example  DisplayIps200::clear();
     *---------------------------------------------------------------------
     */
    static void clear(void);

    /*---------------------------------------------------------------------
     * @brief    屏幕填充函数
     * @param    color 颜色格式RGB565或rgb565_color_enum枚举值
     * @return   无
     * @example  DisplayIps200::full(RGB565_BLACK);
     *---------------------------------------------------------------------
     */
    static void full(const uint16 color);

    /*---------------------------------------------------------------------
     * @brief    画点函数
     * @param    x 坐标x方向的点位置 [0, width-1]
     * @param    y 坐标y方向的点位置 [0, height-1]
     * @param    color 颜色格式RGB565或rgb565_color_enum枚举值
     * @return   无
     * @example  DisplayIps200::drawPoint(10, 20, RGB565_BLUE);
     *---------------------------------------------------------------------
     */
    static void drawPoint(uint16 x, uint16 y, const uint16 color);

    /*---------------------------------------------------------------------
     * @brief    画线函数
     * @param    x_start 坐标x方向的起点 [0, width-1]
     * @param    y_start 坐标y方向的起点 [0, height-1]
     * @param    x_end 坐标x方向的终点 [0, width-1]
     * @param    y_end 坐标y方向的终点 [0, height-1]
     * @param    color 颜色格式RGB565或rgb565_color_enum枚举值
     * @return   无
     * @example  DisplayIps200::drawLine(0, 0, 10, 10, RGB565_RED);
     *---------------------------------------------------------------------
     */
    static void drawLine(uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end, const uint16 color);

    /*---------------------------------------------------------------------
     * @brief    显示单个字符
     * @param    x 坐标x方向的起点 [0, width-1]
     * @param    y 坐标y方向的起点 [0, height-1]
     * @param    dat 需要显示的单个字符
     * @return   无
     * @example  DisplayIps200::showChar(10, 20, 'A');
     *---------------------------------------------------------------------
     */
    static void showChar(uint16 x, uint16 y, const char dat);

    /*---------------------------------------------------------------------
     * @brief    显示字符串
     * @param    x 坐标x方向的起点 [0, width-1]
     * @param    y 坐标y方向的起点 [0, height-1]
     * @param    dat 需要显示的字符串首地址
     * @return   无
     * @example  DisplayIps200::showString(10, 20, "SEEKFREE");
     *---------------------------------------------------------------------
     */
    static void showString(uint16 x, uint16 y, const char dat[]);

    /*---------------------------------------------------------------------
     * @brief    显示有符号整型数
     * @param    x 坐标x方向的起点 [0, width-1]
     * @param    y 坐标y方向的起点 [0, height-1]
     * @param    dat 需要显示的变量
     * @param    num 整数显示长度，最高支持10位
     * @return   无
     * @example  DisplayIps200::showInt(10, 20, -123, 3);
     *---------------------------------------------------------------------
     */
    static void showInt(uint16 x, uint16 y, const int32 dat, uint8 num);

    /*---------------------------------------------------------------------
     * @brief    显示无符号整型数
     * @param    x 坐标x方向的起点 [0, width-1]
     * @param    y 坐标y方向的起点 [0, height-1]
     * @param    dat 需要显示的变量
     * @param    num 整数显示长度，最高支持10位
     * @return   无
     * @example  DisplayIps200::showUint(10, 20, 123, 3);
     *---------------------------------------------------------------------
     */
    static void showUint(uint16 x, uint16 y, const uint32 dat, uint8 num);

    /*---------------------------------------------------------------------
     * @brief    显示浮点数(去除整数部分无效的0)
     * @param    x 坐标x方向的起点 [0, width-1]
     * @param    y 坐标y方向的起点 [0, height-1]
     * @param    dat 需要显示的变量
     * @param    num 整数位显示长度，最高8位
     * @param    pointnum 小数位显示长度，最高6位
     * @return   无
     * @example  DisplayIps200::showFloat(0, 0, x, 2, 3);
     *---------------------------------------------------------------------
     */
    static void showFloat(uint16 x, uint16 y, const double dat, uint8 num, uint8 pointnum);

    /*---------------------------------------------------------------------
     * @brief    显示灰度图像
     * @param    x 坐标x方向的起点 [0, width-1]
     * @param    y 坐标y方向的起点 [0, height-1]
     * @param    image 灰度图像数据首地址
     * @param    width 图像宽度(像素)
     * @param    height 图像高度(像素)
     * @return   无
     * @example  DisplayIps200::showGrayImage(0, 0, gray_buf, 100, 80);
     *---------------------------------------------------------------------
     */
    static void showGrayImage(uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height);

    /*---------------------------------------------------------------------
     * @brief    显示RGB565格式图像
     * @param    x 坐标x方向的起点 [0, width-1]
     * @param    y 坐标y方向的起点 [0, height-1]
     * @param    image RGB565图像数据首地址
     * @param    width 图像宽度(像素)
     * @param    height 图像高度(像素)
     * @return   无
     * @example  DisplayIps200::showRgbImage(0, 0, rgb_buf, 100, 80);
     *---------------------------------------------------------------------
     */
    static void showRgbImage(uint16 x, uint16 y, const uint16 *image, uint16 width, uint16 height);

    /*---------------------------------------------------------------------
     * @brief    显示屏初始化函数
     * @param    path framebuffer设备节点路径，如"/dev/fb0"
     * @param    is_reload_driver 是否重新加载驱动，默认为1
     * @return   无
     * @example  DisplayIps200::init("/dev/fb0");
     *---------------------------------------------------------------------
     */
    static void init(const char *path = FB_PATH, uint8 is_reload_driver = 1);
};

#endif
