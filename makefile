CC = /home/kime/Documents/cpp_projects/cross-compiler/x86_64-elf-cross/bin/x86_64-elf-gcc
CFLAGS = -g -fno-pic               \
    -mno-sse                       \
    -mno-sse2                      \
    -mno-mmx                       \
    -mno-80387                     \
    -mno-red-zone                  \
    -mcmodel=kernel                \
    -ffreestanding                 \
    -fno-stack-protector           \
    -O2                            \
    -fno-omit-frame-pointer

%.o: %.c
	${CC} ${CFLAGS} -I src -c $< -o $@

%.o: %.asm
	nasm -f elf64 -o $@ $<
C_SOURCES = $(shell find src/ -type f -name '*.c')
NASM_SOURCES = $(shell find src/ -type f -name '*.asm')
OBJ = ${C_SOURCES:.c=.o} ${NASM_SOURCES:.asm=.o}
SalieriOS: ${OBJ}
	${CC} -Wl,-z,max-page-size=0x1000 -nostdlib -o $@.elf -T linker.ld $^

run: SalieriOS
	rm -rf SalieriOS.img SalieriOS_image/
	mkdir SalieriOS_image
	dd if=/dev/zero bs=1M count=0 seek=64 of=SalieriOS.img
	parted -s SalieriOS.img mklabel gpt
	parted -s SalieriOS.img mkpart primary 2048s 6143s
	parted -s SalieriOS.img mkpart primary 6144s 131038s
	sudo losetup -Pf --show SalieriOS.img > loopback_dev
	sudo partprobe `cat loopback_dev`
	sudo mkfs.ext2 `cat loopback_dev`p2
	sudo mount `cat loopback_dev`p2 SalieriOS_image
	sudo mkdir SalieriOS_image/boot
	sudo cp ./SalieriOS.elf SalieriOS_image/boot/
	sudo cp ./limine.cfg SalieriOS_image/
	sync
	sudo umount SalieriOS_image/
	sudo losetup -d `cat loopback_dev`
	rm -rf SalieriOS_image loopback_dev
	../limine/limine-install ../limine/limine.bin SalieriOS.img 2048
	qemu-system-x86_64 -hda SalieriOS.img -serial stdio -d int -machine smm=off -no-reboot -no-shutdown
	
