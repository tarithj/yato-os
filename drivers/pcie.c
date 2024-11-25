#include "pcie.h"
#include "../libc/function.h"
#include "screen.h"
void checkFunction(uint8_t bus, uint8_t device, uint8_t function) {
    UNUSED(bus);
    UNUSED(device);
    UNUSED(function);
    return;
 }
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    port_word_out(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((port_word_in(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

uint16_t pciCheckVendor(uint8_t bus, uint8_t slot) {
    uint16_t vendor, device;
    UNUSED(device);
    /* Try and read the first configuration register. Since there are no
     * vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor = pciConfigReadWord(bus, slot, 0, 0)) != 0xFFFF) {
       device = pciConfigReadWord(bus, slot, 0, 2);
    } return (vendor);
}

struct PCI_Config_Header {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t  revision_id;
    uint8_t  prog_interface;
    uint8_t  subclass;
    uint8_t  class_code;
    uint8_t  cache_line_size;
    uint8_t  latency_timer;
    uint8_t  header_type;
    uint8_t  bist;
};

// Function to check the Vendor ID of a PCI device
uint16_t getVendorID(uint8_t bus, uint8_t device, uint8_t function) {
    // Vendor ID is located at offset 0 in the PCI configuration space
    return pciConfigReadWord(bus, device, function, 0) & 0xFFFF;
}

// Function to check the Header Type of a PCI device
uint8_t getHeaderType(uint8_t bus, uint8_t device, uint8_t function) {
    // Header Type is located at offset 0xE in the PCI configuration space
    return (pciConfigReadWord(bus, device, function, 0xE) >> 16) & 0xFF;
}



void read_pci_configuration(uint8_t *config_data, struct PCI_Config_Header *pci_header) {
    // Assuming config_data is an array containing the PCI configuration space data
    // and each entry is 1 byte
    
    // Copy data from config_data array to pci_header structure
    pci_header->vendor_id = (config_data[1] << 8) | config_data[0];
    pci_header->device_id = (config_data[3] << 8) | config_data[2];
    pci_header->command = (config_data[5] << 8) | config_data[4];
    pci_header->status = (config_data[7] << 8) | config_data[6];
    pci_header->revision_id = config_data[8];
    pci_header->prog_interface = config_data[9];
    pci_header->subclass = config_data[10];
    pci_header->class_code = config_data[11];
    pci_header->cache_line_size = config_data[12];
    pci_header->latency_timer = config_data[13];
    pci_header->header_type = config_data[14];
    pci_header->bist = config_data[15];
}

void checkDevice(uint8_t bus, uint8_t device) {
     uint8_t function = 0;
 
     int vendorID = getVendorID(bus, device, function);
     if (vendorID == 0xFFFF) return; // Device doesn't exist
     checkFunction(bus, device, function);
     int headerType = getHeaderType(bus, device, function);
     if( (headerType & 0x80) != 0) {
         // It's a multi-function device, so check remaining functions
         for (function = 1; function < 8; function++) {
             if (getVendorID(bus, device, function) != 0xFFFF) {
                 checkFunction(bus, device, function);
             }
         }
     }
 }
 
