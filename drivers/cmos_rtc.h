#ifndef KERNEL_H
#define KERNEL_H

struct cmos_rtc_val read_rtc();
struct cmos_rtc_val
{
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned int year;
};
#endif