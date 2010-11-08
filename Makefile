# The targets we want to build are the root and boot disk images.
all: boot root run

# We make sure that we are using the standard GNU gcc and ld tools.
CC=gcc 
LD=ld

# These are immensely helpful in writing correct C code and should
# *not* be removed.
CFLAGS=-ffreestanding\
 -Wall -W  -Wno-div-by-zero -Wstrict-prototypes


#----------------------------------------------------------------------
# This part of the makefile is concerned with building disk images.

# We're using the minix file system
FS=minix

# This creates the boot disk. The boot disk is a standard 1.44MB
# floppy disk, which contains the GRUB bootloader and nothing
# else. This should not ever have to change.
boot:
	dd if=/dev/zero of=boot bs=512 count=2880 2>> dd.log
	dd if=grub/stage1 of=boot bs=512 count=1 2>> dd.log
	dd if=grub/stage2 of=boot bs=512 seek=1 2>> dd.log

# This creates the root disk. The root disk contains a minix file
# system with our kernel in it.
root: kernel
	dd if=/dev/zero of=root bs=512 count=2880 2>> dd.log
	/sbin/mkfs -t $(FS) root >> dd.log
	mount -o loop -t $(FS) root empty
	cp kernel empty
	umount empty

# This fixes the permissions on the run script.
run: run.sh
	cp run.sh run
	chmod +x run

#----------------------------------------------------------------------
# Here comes the *actual* build system for the kernel.

# Any .o files that are to be compiled from .c files will be compiled
# for us automatically. We may have to revise this, because I do not
# believe it will recompile if a random header somewhere changed,
# which is of course nasty.

OBJECTS=asm.o kernel.o vga.o util.o malloc.o term.o\
 i80386.o segments.o handlers.o interrupt.o\
 i8042.o i8253.o i8259.o exceptions.o kb.o

kernel: layout.ld $(OBJECTS)
	$(LD) -o kernel -T layout.ld $(OBJECTS)
	darcs changes > CHANGES

loader: loader.c
	gcc -o loader loader.c

clean:
	-rm -f $(OBJECTS) root boot

dist: clean
	DIR=`pwd | xargs basename` ;\
	ZIP=$$DIR-`date +%F-%H%M`.tar.gz ;\
	 ( cd ..; tar czvf $$ZIP $$DIR ; du -hs $$ZIP)

depend:
	makedepend *.c *.S

# Run make depend to automomatically update the dependency info below:
# NOTE! Do not use comments in .S files that start with a
# pound sign. makedepend does not like that.

# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# DO NOT DELETE

exceptions.o: std.h exceptions.h interrupt.h term.h vga.h kb.h i8042.h
i8042.o: std.h i80386.h i8042.h i8259.h term.h vga.h kb.h
i82077.o: std.h i80386.h i8259.h
i8253.o: std.h i8253.h i8259.h term.h vga.h kb.h i8042.h
i8259.o: std.h i80386.h interrupt.h term.h vga.h kb.h i8042.h i8259.h
interrupt.o: std.h vga.h i80386.h handlers.h segments.h interrupt.h term.h
interrupt.o: kb.h i8042.h
kb.o: kb.h i8042.h std.h
kernel.o: std.h vga.h util.h malloc.h multiboot.h segments.h interrupt.h
kernel.o: i80386.h i8259.h i8042.h exceptions.h term.h kb.h
malloc.o: std.h malloc.h
segments.o: std.h i80386.h segments.h
term.o: vga.h std.h term.h kb.h i8042.h
util.o: std.h util.h
vga.o: std.h vga.h i80386.h
asm.o: multiboot.h
i80386.o: i80386.h std.h segments.h
