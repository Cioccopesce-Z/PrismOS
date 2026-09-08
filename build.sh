#/bin/bash
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
clear && make clean && echo "hehehe you want see this" && make all && qemu-system-i386 -hda bin/os.bin -m 8G

