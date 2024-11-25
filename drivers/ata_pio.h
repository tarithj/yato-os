#ifndef ATA_PIO_H
#define ATA_PIO_H
#include "../cpu/type.h"
    struct StatusRegister {
        unsigned char BSY : 1;   // Bit 7 - Busy (BSY)
        unsigned char DRDY : 1;  // Bit 6 - Device Ready (DRDY)
        unsigned char DF : 1;    // Bit 5 - Device Fault (DF)
        unsigned char RESERVED1 : 1; // Bit 4 - Reserved
        unsigned char DRQ : 1;   // Bit 3 - Data Request (DRQ)
        unsigned char RESERVED2 : 2; // Bits 2-1 - Reserved
        unsigned char ERR : 1;   // Bit 0 - Error (ERR)
    };


    void ide_init();
    int ide_ready_check();
    void ide_get_status_register(struct StatusRegister *status);

#endif