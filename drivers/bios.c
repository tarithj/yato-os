#include "bios.h"

/**********************************************************
 * Public Kernel API functions                            *
 **********************************************************/


uint16_t detect_bios_area_hardware(void)
{
    const uint16_t* bda_detected_hardware_ptr = (const uint16_t*) 0x410; // bios data area ptr
    return *bda_detected_hardware_ptr;
}

enum video_type get_bios_area_video_type(void)
{
    return (enum video_type) (detect_bios_area_hardware() & 0x30);
}