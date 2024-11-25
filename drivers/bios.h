#ifndef BIOS_H
#define BIOS_H
#include <stdint.h>
enum video_type {
    VIDEO_TYPE_NONE = 0x00,
    VIDEO_TYPE_COLOR = 0x20,
    VIDEO_TYPE_MONOCHROME = 0x30,
};
void walk_sdts(uint32_t rsdt_address);

struct ISDTHeader {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
};


// Structure representing the RSDP
struct RSDPDescriptor {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
};
uint16_t detect_bios_area_hardware(void);
struct RSDPDescriptor *find_rsdp(uintptr_t start, int length);
enum video_type get_bios_area_video_type(void);

uint32_t walk_mcfg(uint32_t addr);



struct MCFGHeader {
    uint32_t signature;      // Signature (must be "MCFG")
    uint32_t length;         // Length of the table, including the header
    uint8_t revision;        // Revision of the table format
    uint8_t checksum;        // Entire table must sum to zero
    uint8_t oem_id[6];       // OEM ID (6 ASCII characters)
    uint64_t oem_table_id;   // OEM table ID (8 ASCII characters)
    uint32_t oem_revision;   // OEM revision number
    uint32_t creator_id;     // Vendor ID of utility that created the table
    uint32_t creator_revision; // Revision of utility that created the table
    // Additional fields may follow, depending on the ACPI specification version
};

struct MCFGBAS {
    uint64_t base_address;
    uint16_t segment_group_no;
    uint8_t start_bus_no;
    uint8_t end_bus_no;
    uint32_t reserved;
};


#endif


