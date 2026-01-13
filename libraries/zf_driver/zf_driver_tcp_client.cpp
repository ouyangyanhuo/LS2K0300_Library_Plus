#include "zf_driver_tcp_client.hpp"

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 设置文件句柄为非阻塞模式
// 参数说明 fd    文件/套接字句柄
// 返回参数 int   0-设置成功  -1-设置失败
// 使用示例 内部调用，无需外部调用
// 备注信息 基于fcntl修改句柄属性，实现非阻塞的TCP数据收发
//-------------------------------------------------------------------------------------------------------------------
int zf_driver_tcp_client::set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 构造函数
// 参数说明 无
// 返回参数 无
// 使用示例 zf_driver_tcp_client tcp_client;
// 备注信息 初始化套接字句柄为无效值，地址结构体内存清零初始化
//-------------------------------------------------------------------------------------------------------------------
zf_driver_tcp_client::zf_driver_tcp_client()
{
    // 初始化socket句柄为无效值，地址结构体清零
    m_socket = -1;
    memset(&m_server_addr, 0, sizeof(m_server_addr));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 析构函数
// 参数说明 无
// 返回参数 无
// 使用示例 自动调用，无需手动调用
// 备注信息 自动关闭已打开的套接字句柄，释放网络资源，防止句柄泄漏
//-------------------------------------------------------------------------------------------------------------------
zf_driver_tcp_client::~zf_driver_tcp_client()
{
    if (m_socket >= 0)
    {
        close(m_socket);
        m_socket = -1;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 TCP客户端初始化，创建套接字并连接服务器
// 参数说明 ip_addr  服务器IP地址字符串
// 参数说明 port     服务器TCP端口号
// 返回参数 int8     0-初始化连接成功  -1-初始化连接失败
// 使用示例 tcp_client.init("192.168.1.100", 8080);
// 备注信息 创建流式套接字，配置服务器地址，连接成功后设置为非阻塞模式
//-------------------------------------------------------------------------------------------------------------------
int8 zf_driver_tcp_client::init(const char *ip_addr, uint32 port)
{
    // 创建TCP套接字 SOCK_STREAM
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        printf("Failed to create socket\r\n");
        return -1;
    }

    // 配置服务器地址
    memset(&m_server_addr, 0, sizeof(m_server_addr));
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_addr.s_addr = inet_addr(ip_addr);
    m_server_addr.sin_port = htons(port);

    printf("Wait connect tcp server\r\n");

    // 连接到服务器
    int ret = connect(m_socket, (struct sockaddr*)&m_server_addr, sizeof(m_server_addr));
    if (ret < 0)
    {
        perror("connect() error");
        close(m_socket);
        m_socket = -1;
        return -1;
    }

    // 设置非阻塞模式
    if (set_nonblocking(m_socket) < 0) {
        perror("set_nonblocking");
        close(m_socket);
        m_socket = -1;
        return -1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 TCP发送数据
// 参数说明 buff    待发送数据缓冲区指针
// 参数说明 length  待发送数据的字节长度
// 返回参数 uint32  成功返回实际发送字节数  失败返回0
// 使用示例 tcp_client.send_data(send_buf, sizeof(send_buf));
// 备注信息 
//-------------------------------------------------------------------------------------------------------------------
uint32 zf_driver_tcp_client::send_data(const uint8 *buff, uint32 length)
{
    int32 str_len;
    str_len = send(m_socket, buff, length, 0);
    
    if (str_len == -1)
    {
        printf("send() error");
        return 0; 
    }
    return str_len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 TCP读取数据
// 参数说明 buff    接收数据缓冲区指针
// 参数说明 length  最大接收的字节长度
// 返回参数 uint32  成功返回实际接收字节数  无数据返回0  错误返回-1
// 使用示例 tcp_client.read_data(recv_buf, sizeof(recv_buf));
// 备注信息 非阻塞模式读取，EAGAIN/EWOULDBLOCK为正常无数据状态
//-------------------------------------------------------------------------------------------------------------------
uint32 zf_driver_tcp_client::read_data(uint8 *buff, uint32 length)
{
    int32 str_len;
    str_len = recv(m_socket, buff, length, 0);

    if (str_len == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {   // 非阻塞模式：暂时无数据可读，正常情况
            return 0;
        }
        else
        {   // 真正的读取错误
            printf("recv() error");
            return -1;
        }
    }

    return str_len;
}