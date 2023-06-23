# What is YATO-OS
Yato OS is a hobby os that I developed following [osdev](https://wiki.osdev.org) and a pretty old [tutorial](https://github.com/cfenollosa/os-tutorial) in GitHub

# What can it do
- [ ] Shutdown/Restart (APIC) 
- [x] CMOS RTC
- [x] Keyboard support
- [ ] USB support
- [x] Interupt handling
- [x] PC Speaker support

# Building
1. Download [nasm](https://www.nasm.us/) using `sudo apt install nasm`
2. Download i386 gcc toolkit script `wget https://gist.github.com/tarithj/218a45456344d9d478ce21a9d9951c23 -O install_i386gcc.sh`
3. Run using `chmod +x ./install_i386gcc.sh && ./install_i386gcc.sh`
4. Build using `make os-image.bin` or `make run` to run YATO using qemu
