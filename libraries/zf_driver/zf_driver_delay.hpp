#ifndef _zf_driver_delay_h
#define _zf_driver_delay_h


#include "zf_common_typedef.hpp"

#define system_delay_ms(time)  (usleep(time*1000))
#define system_delay_us(time)  (usleep(time))


#endif
