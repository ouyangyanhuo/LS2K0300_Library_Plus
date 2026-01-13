#include "zf_driver_udp.hpp"

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 构造函数
// 参数说明 无
// 返回参数 无
// 使用示例 zf_driver_udp udp_client;
// 备注信息 初始化套接字句柄为无效值，地址结构体清零，初始化地址结构体长度
//-------------------------------------------------------------------------------------------------------------------
zf_driver_udp::zf_driver_udp()
{
    m_socket = -1;
    m_server_addr_size  = sizeof(m_server_addr);
    memset(&m_server_addr, 0, sizeof(m_server_addr));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 析构函数
// 参数说明 无
// 返回参数 无
// 使用示例 自动调用，无需手动调用
// 备注信息 自动关闭已打开的UDP套接字句柄，释放网络资源，防止句柄资源泄露
//-------------------------------------------------------------------------------------------------------------------
zf_driver_udp::~zf_driver_udp()
{
    if (m_socket >= 0)
    {
        close(m_socket);
        m_socket = -1;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 UDP客户端初始化，创建套接字并配置目标服务器地址
// 参数说明 ip_addr  目标服务器IP地址字符串
// 参数说明 port     目标服务器UDP端口号
// 返回参数 int8     0-初始化成功  -1-初始化失败
// 使用示例 udp_client.init("192.168.1.10", 8000);
// 备注信息 创建数据报套接字，初始化网络地址结构体，无连接过程
//-------------------------------------------------------------------------------------------------------------------
int8 zf_driver_udp::init(const char *ip_addr, uint32 port)
{
    // 创建 UDP 套接字
    m_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (m_socket == -1) 
    {
        printf("Failed to create udp socket\r\n");
        return -1;
    }

    // 配置服务器地址
    memset(&m_server_addr, 0, sizeof(m_server_addr));
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_addr.s_addr = inet_addr(ip_addr);
    m_server_addr.sin_port = htons(port);

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 UDP发送数据到目标服务器
// 参数说明 buff    待发送数据缓冲区指针
// 参数说明 length  待发送数据的字节长度
// 返回参数 uint32  成功返回实际发送字节数  失败返回0
// 使用示例 udp_client.send_data(send_buf, sizeof(send_buf));
// 备注信息 基于无连接的sendto实现数据发送，错误时返回错误码信息
//-------------------------------------------------------------------------------------------------------------------
uint32 zf_driver_udp::send_data(const uint8 *buff, uint32 length)
{
    ssize_t send_len = sendto(m_socket, buff, length, 0, 
                             (sockaddr*)&m_server_addr, sizeof(m_server_addr));
    if (send_len == -1)
    {
        printf("udp sendto() error, errno:%d\r\n", errno);
        return 0;
    }
    return send_len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介 UDP非阻塞读取数据
// 参数说明 buff    接收数据缓冲区指针
// 参数说明 length  接收缓冲区最大可存储字节长度
// 返回参数 uint32  成功返回实际接收字节数  无数据/读取失败返回0
// 使用示例 udp_client.read_data(recv_buf, sizeof(recv_buf));
// 备注信息 带MSG_DONTWAIT非阻塞标志，EAGAIN/EWOULDBLOCK为正常无数据状态
//-------------------------------------------------------------------------------------------------------------------
uint32 zf_driver_udp::read_data(uint8 *buff, uint32 length)
{
    m_server_addr_size = sizeof(m_server_addr);

    // 接收客户端消息, 非阻塞式读取 MSG_DONTWAIT 特性不变
    ssize_t str_len = recvfrom(m_socket, buff, length, MSG_DONTWAIT, 
                              (struct sockaddr *)&m_server_addr, &m_server_addr_size);
    if (str_len == -1) 
    {
        // 非阻塞无数据时不打印error，减少日志污染
        if(errno != EAGAIN && errno != EWOULDBLOCK)
        {
            printf("recvfrom() error, errno:%d\r\n", errno);
        }
        return 0;
    }

    return str_len;
}