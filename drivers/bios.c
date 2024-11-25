#include "bios.h"
#include <stddef.h>
#include "../libc/string.h"
#include "../libc/mem.h"
#include "screen.h"
#include "../libc/convert.h"
/**********************************************************
 * Public Kernel API functions                            *
 **********************************************************/

// Signature of RSDP
#define RSDP_SIGNATURE "RSD PTR "
#define RSDP_SIGNATURE_LEN 8
#define MCFG_SIGNATURE "MCFG"      // MCFG signature

struct RSDPDescriptor *find_rsdp(uintptr_t start, int length) {
    for (int i = 0; i < length; i += 16) {
        struct RSDPDescriptor *rsdp = (struct RSDPDescriptor *)(start + i);
        if (memory_compare(rsdp->signature, RSDP_SIGNATURE, 8) == 0) {
            // Signature found, return the pointer
            // Checksum validation can be performed here
            return rsdp;
        }
    }
    kprint("coudlnt find rsdp");
    // Signature not found
    return NULL;
}

#define bake(a, b , c, d)  a | (b << 8) | (c << 16) | (d << 24)

// parse all tables
#define APIC_t bake('A', 'P', 'I', 'C')
#define HPET_t bake('H', 'P', 'E', 'T')
#define FACP_t bake('F', 'A', 'C', 'P')
#define MCFG_t bake('M', 'C', 'F', 'G')

uint16_t detect_bios_area_hardware(void)
{
    const uint16_t* bda_detected_hardware_ptr = (const uint16_t*) 0x410; // bios data area ptr
    return *bda_detected_hardware_ptr;
}



enum video_type get_bios_area_video_type(void)
{
    return (enum video_type) (detect_bios_area_hardware() & 0x30);
}

  void walk_sdts(uint32_t rsdt_address){
    struct ISDTHeader* isdt = (struct ISDTHeader*) rsdt_address;

    // find total number of SDTs
    int  total = (isdt->Length - sizeof(struct ISDTHeader)) / 4;
    // go past rsdt
    const char* addr = (const char*) isdt;
    addr += sizeof(struct ISDTHeader);

    while (total > 0)
    {
      // create SDT pointer from 32-bit address
      // NOTE: don't touch!
      struct ISDTHeader* sdt = (struct ISDTHeader*) (uintptr_t) (*(uint32_t*) addr);

      addr += 4; total--;

      // some entries seems to be null depending on hypervisor
      if(sdt == NULL)
        continue;

      // find out which SDT it is
      switch (*(uint32_t*) sdt->Signature) {
      case MCFG_t:
              kprint("MCFG found\n\0");
              walk_mcfg((uint32_t) sdt);
              break;
      case APIC_t:
        kprint("APIC found\n\0");
        //debug("APIC found: P=%p L=%u\n", sdt, sdt->Length);
        //walk_madt((char*) sdt);
        break;
      case HPET_t:
        kprint("HPET_t found\n\0");
        //this->hpet_base = (uintptr_t) addr + sizeof(SDTHeader);
        break;
      case FACP_t:
        kprint("FACP_t found\n\0");
        //walk_facp((char*) sdt);
        break;
      default:
        kprint(sdt->Signature);
        kprint("not found\n\0");
        //debug("Signature: %.*s (u=%u)\n", 4, sdt->Signature, sdt->sigint());
      }
    kprint("Finished walking SDTs\n\0");
    }
  }



// Define PCI Header structure
typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    // Add more fields as needed
} PCI_Header;

// Function to parse PCI headers from a memory address
PCI_Header parse_pci_header(uint32_t *mem_address) {
    PCI_Header header;

    // Read vendor ID and device ID from memory address
    header.vendor_id = (uint16_t)(*mem_address & 0xFFFF);
    header.device_id = (uint16_t)((*mem_address >> 16) & 0xFFFF);

    // Parse other fields as needed

    return header;
}

uint32_t walk_mcfg(uint32_t addr) {
  struct MCFGHeader* mcfg = (struct MCFGHeader*)(addr);
  int c = (mcfg->length-44)/16;
  uint32_t arr = kmalloc(sizeof(struct MCFGHeader*)*c, 1, 0);

  for (int i = 0; i < c; i++) {
    struct MCFGBAS* m = (struct MCFGBAS*)(addr+44+(c*i));
    char *v = "";
    convert_int_to_string(m->base_address, v);
    kprint(v);
    kprint(" \0");
    //convert_int_to_string(parse_pci_header((uint64_t*)m->base_address).vendor_id, v);
    //kprint("vendor:");
    //  kprint(v);
    kprint(" \0");
    memory_set((uint32_t *)arr, (uint32_t) &m, sizeof(struct MCFGHeader*));
  }
  return arr;
}

