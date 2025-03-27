SRCS=gpioctrl.c 
KMOD=gpioctrl

CFLAGS+= -I/usr/obj/usr/src/arm64.aarch64/sys/GENERIC

# Uncomment the following lines to cross compile to ARM64 architecture for the Raspberry Pi
#CC=clang --target=aarch64-unknown-freebsd
#MACHINE_ARCH=aarch64
#MACHINE=arm64

.include <bsd.kmod.mk>
