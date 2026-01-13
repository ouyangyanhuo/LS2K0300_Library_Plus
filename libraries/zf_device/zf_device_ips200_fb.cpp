/*---------------------------------------------------------------------
 * @file     zf_device_ips200_fb.cpp
 * @brief    IPS200显示屏驱动实现文件
 * @date     2026/01/09
 *---------------------------------------------------------------------
 * @author   Magneto
 *---------------------------------------------------------------------
 * @brief    IPS200显示屏驱动类实现文件
 *           包含显示屏初始化、绘图、显示字符、显示数字、显示图像等功能的实现
 *---------------------------------------------------------------------
 */

#include "zf_device_ips200_fb.hpp"
#include "zf_common_font.hpp"
#include "zf_common_function.hpp"

// DisplayIps200类的static成员变量定义
uint16 DisplayIps200::pen_color = DEFAULT_PENCOLOR;
uint16 DisplayIps200::bg_color = DEFAULT_BGCOLOR;
int DisplayIps200::width = 0;
int DisplayIps200::height = 0;
unsigned short *DisplayIps200::screen_base = nullptr;

/*---------------------------------------------------------------------
 * @brief    清屏函数
 * @return   无
 * @example  DisplayIps200::clear();
 *---------------------------------------------------------------------
 */
void DisplayIps200::clear(void)
{
    full(DEFAULT_BGCOLOR);
}

/*---------------------------------------------------------------------
 * @brief    屏幕填充函数
 * @param    color 颜色格式RGB565或rgb565_color_enum枚举值
 * @return   无
 * @example  DisplayIps200::full(RGB565_BLACK);
 *---------------------------------------------------------------------
 */
void DisplayIps200::full(const uint16 color)
{
    uint16 i,j;
    for(i=0;i<240; i++)
    {
        for(j=0; j<320; j++)
        {
            drawPoint(i, j, color);
        }
    }
}

/*---------------------------------------------------------------------
 * @brief    画点函数
 * @param    x 坐标x方向的点位置 [0, width-1]
 * @param    y 坐标y方向的点位置 [0, height-1]
 * @param    color 颜色格式RGB565或rgb565_color_enum枚举值
 * @return   无
 * @example  DisplayIps200::drawPoint(10, 20, RGB565_BLUE);
 *---------------------------------------------------------------------
 */
void DisplayIps200::drawPoint(uint16 x, uint16 y, const uint16 color)
{
    if(nullptr != screen_base)
    {
        screen_base[y * width + x] = color;
    }
}

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
void DisplayIps200::drawLine (uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end, const uint16 color)
{
    int16 x_dir = (x_start < x_end ? 1 : -1);
    int16 y_dir = (y_start < y_end ? 1 : -1);
    float temp_rate = 0;
    float temp_b = 0;

    do
    {
        if(x_start != x_end)
        {
            temp_rate = (float)(y_start - y_end) / (float)(x_start - x_end);
            temp_b = (float)y_start - (float)x_start * temp_rate;
        }
        else
        {
            while(y_start != y_end)
            {
                drawPoint(x_start, y_start, color);
                y_start += y_dir;
            }
            drawPoint(x_start, y_start, color);
            break;
        }
        if(func_abs(y_start - y_end) > func_abs(x_start - x_end))
        {
            while(y_start != y_end)
            {
                drawPoint(x_start, y_start, color);
                y_start += y_dir;
                x_start = (int16)(((float)y_start - temp_b) / temp_rate);
            }
            drawPoint(x_start, y_start, color);
        }
        else
        {
            while(x_start != x_end)
            {
                drawPoint(x_start, y_start, color);
                x_start += x_dir;
                y_start = (int16)((float)x_start * temp_rate + temp_b);
            }
            drawPoint(x_start, y_start, color);
        }
    }while(0);
}

/*---------------------------------------------------------------------
 * @brief    显示单个字符
 * @param    x 坐标x方向的起点 [0, width-1]
 * @param    y 坐标y方向的起点 [0, height-1]
 * @param    dat 需要显示的单个字符
 * @return   无
 * @example  DisplayIps200::showChar(10, 20, 'A');
 *---------------------------------------------------------------------
 */
void DisplayIps200::showChar(uint16 x, uint16 y, const char dat)
{
    uint8 i = 0, j = 0;
    for(i = 0; 8 > i; i ++)
    {
        uint8 temp_top = ascii_font_8x16[dat - 32][i];
        uint8 temp_bottom = ascii_font_8x16[dat - 32][i + 8];
        for(j = 0; 8 > j; j ++)
        {
            if(temp_top & 0x01)
            {
                drawPoint(x + i, y + j, pen_color);
            }
            else
            {
                drawPoint(x + i, y + j, bg_color);
            }
            temp_top >>= 1;
        }

        for(j = 0; 8 > j; j ++)
        {
            if(temp_bottom & 0x01)
            {
                drawPoint(x + i, y + j + 8, pen_color);
            }
            else
            {
                drawPoint(x + i, y + j + 8, bg_color);
            }
            temp_bottom >>= 1;
        }
    }
}

/*---------------------------------------------------------------------
 * @brief    显示字符串
 * @param    x 坐标x方向的起点 [0, width-1]
 * @param    y 坐标y方向的起点 [0, height-1]
 * @param    dat 需要显示的字符串首地址
 * @return   无
 * @example  DisplayIps200::showString(10, 20, "SEEKFREE");
 *---------------------------------------------------------------------
 */
void DisplayIps200::showString(uint16 x, uint16 y, const char dat[])
{
    uint16 j = 0;

    while('\0' != dat[j])
    {
        showChar(x + 8 * j,  y, dat[j]);
        j ++;
    }
}

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
void DisplayIps200::showInt(uint16 x, uint16 y, const int32 dat, uint8 num)
{
    int32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12] = {0};

    std::memset(data_buffer, 0, 12);
    std::memset(data_buffer, ' ', num+1);

    if(10 > num)
    {
        for(; 0 < num; num --)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }
    func_int_to_str(data_buffer, dat_temp);
    showString(x, y, (const char *)&data_buffer);
}

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
void DisplayIps200::showUint(uint16 x, uint16 y, const uint32 dat, uint8 num)
{
    uint32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12] = {0};
    std::memset(data_buffer, 0, 12);
    std::memset(data_buffer, ' ', num);

    if(10 > num)
    {
        for(; 0 < num; num --)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }
    func_uint_to_str(data_buffer, dat_temp);
    showString(x, y, (const char *)&data_buffer);
}

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
void DisplayIps200::showFloat (uint16 x, uint16 y, const double dat, uint8 num, uint8 pointnum)
{
    double dat_temp = dat;
    double offset = 1.0;
    char data_buffer[17] = {0};
    std::memset(data_buffer, 0, 17);
    std::memset(data_buffer, ' ', num+pointnum+2);

    for(; 0 < num; num --)
    {
        offset *= 10;
    }
    dat_temp = dat_temp - ((int)dat_temp / (int)offset) * offset;
    func_double_to_str(data_buffer, dat_temp, pointnum);
    showString(x, y, data_buffer);
}

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
void DisplayIps200::showGrayImage(uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height)
{
    uint32 x_start = 0, y_start = 0;
    uint16 color = 0;

    for(y_start = y; y_start < (y + height); y_start++)
    {
        for(x_start = x; x_start < (x + width); x_start++)
        {
            uint8 grayValue = image[(x_start - x) + (y_start- y) * width];

            uint16 r = (grayValue >> 3) & 0b11111;
            uint16 g = (grayValue >> 2) & 0b111111;
            uint16 b = (grayValue >> 3) & 0b11111;
            color = (r << 11) | (g << 5) | (b << 0);

            drawPoint(x_start, y_start,  color);
        }
    }
}

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
void DisplayIps200::showRgbImage(uint16 x, uint16 y, const uint16 *image, uint16 width, uint16 height)
{
    uint32 x_start = 0, y_start = 0;
    for(y_start = y; y_start < (y + height); y_start++)
    {
        for(x_start = x; x_start < (x + width); x_start++)
        {
            uint16 rgbValue = image[(x_start - x) + (y_start- y) * width];
            drawPoint(x_start, y_start,  rgbValue);
        }
    }
}

/*---------------------------------------------------------------------
 * @brief    显示屏初始化函数
 * @param    path framebuffer设备节点路径，如"/dev/fb0"
 * @param    is_reload_driver 是否重新加载驱动，默认为1
 * @return   无
 * @example  DisplayIps200::init("/dev/fb0");
 *---------------------------------------------------------------------
 */
void DisplayIps200::init(const char *path, uint8 is_reload_driver)
{
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;
    unsigned int screen_size;
    int fd;

    if (is_reload_driver)
    {
        std::cout << "ips200: 卸载fb_st7789v驱动..." << std::endl;
        // 卸载fb_st7789v驱动模块，忽略卸载失败(比如驱动本就没加载的情况)
        system("rmmod fb_st7789v > /dev/null 2>&1");
        usleep(200*1000); // 延时200ms，保证驱动卸载干净，避免加载失败
        std::cout << "ips200: 加载fb_st7789v驱动..." << std::endl;
        // 重新加载fb_st7789v驱动模块，必须保证驱动文件路径正确
        if(system("insmod /lib/modules/4.19.190/fb_st7789v.ko") != 0)
        {
            std::cerr << "insmod fb_st7789v驱动失败" << std::endl;
            exit(EXIT_FAILURE);
        }
        usleep(200*1000); // 延时300ms，驱动加载完成+硬件初始化完成，关键延时
    }

    if (0 > (fd = open(path, O_RDWR))) {
        std::cerr << "打开framebuffer设备失败" << std::endl;
        exit(EXIT_FAILURE);
    }

    ioctl(fd, FBIOGET_VSCREENINFO, &fb_var);
    ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix);

    screen_size = fb_fix.line_length * fb_var.yres;
    DisplayIps200::width = fb_var.xres;
    DisplayIps200::height = fb_var.yres;

    screen_base = (unsigned short *)mmap(nullptr, screen_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == (void *)screen_base) {
        std::cerr << "mmap映射显存失败" << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }

    for(uint16 i=0;i<240;i++)
    {
        for(uint16 j=0;j<320;j++)
        {
            drawPoint(i, j, DEFAULT_BGCOLOR);
        }
    }
}
