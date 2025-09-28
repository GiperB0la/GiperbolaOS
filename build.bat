@echo off
setlocal enabledelayedexpansion

:: ============================
:: CONFIG
:: ============================
set NASM=D:\Assembler\Nasm\nasm
set GCC=D:\Cpp\i686-elf-tools\bin\i686-elf-g++
set LD=D:\Cpp\i686-elf-tools\bin\i686-elf-ld
set QEMU=C:\msys64\mingw64\bin\qemu-system-i386
set CFLAGS=-ffreestanding -fno-exceptions -fno-rtti -fno-threadsafe-statics -O2 -Wall -Wextra

:: ============================
:: CLEAN PREVIOUS BUILD
:: ============================
if not exist obj mkdir obj
if not exist bin mkdir bin

set BINDIR=bin
set ISODIR=%BINDIR%/iso
set ISO=%BINDIR%\GiperbolaOS.iso

:: ============================
:: BUILD ASM FILES
:: ============================
echo [ASM] Building boot and low-level
%NASM% -f elf32 arch\x86\boot\multiboot.asm   -o obj\multiboot.o
%NASM% -f elf32 arch\x86\boot\gdt_flush.asm   -o obj\gdt_flush.o
%NASM% -f elf32 arch\x86\boot\idt_load.asm    -o obj\idt_load.o
%NASM% -f elf32 arch\x86\interrupts\isr.asm   -o obj\isr_asm.o
%NASM% -f elf32 arch\x86\interrupts\irq.asm   -o obj\irq_asm.o

:: ============================
:: BUILD C++ FILES
:: ============================
echo [C++] Compiling kernel and modules
%GCC% %CFLAGS% -c kernel\Kernel.cpp                     -o obj\kernel.o
%GCC% %CFLAGS% -c kernel\Panic.cpp                      -o obj\panic.o

%GCC% %CFLAGS% -c arch\x86\gdt\GDT.cpp                  -o obj\gdt.o
%GCC% %CFLAGS% -c arch\x86\interrupts\IDT.cpp           -o obj\idt.o
%GCC% %CFLAGS% -c arch\x86\io\LowLevel_io.cpp           -o obj\lowlevel_io.o
%GCC% %CFLAGS% -c arch\x86\interrupts\Pic.cpp           -o obj\pic.o
%GCC% %CFLAGS% -c arch\x86\interrupts\isr.cpp           -o obj\isr.o
%GCC% %CFLAGS% -c arch\x86\interrupts\irq.cpp           -o obj\irq.o
%GCC% %CFLAGS% -c arch\x86\memory\Memory.cpp            -o obj\memory.o

%GCC% %CFLAGS% -c drivers\VGA.cpp                       -o obj\vga.o
%GCC% %CFLAGS% -c drivers\Keyboard.cpp                  -o obj\keyboard.o
%GCC% %CFLAGS% -c drivers\Serial.cpp                    -o obj\serial.o

%GCC% %CFLAGS% -c lib\String.cpp                        -o obj\string.o
%GCC% %CFLAGS% -c shell\Shell.cpp                       -o obj\shell.o

:: ============================
:: LINK KERNEL
:: ============================
echo [LD] Linking kernel
%LD% -T linker\linker.ld -nostdlib -o bin\kernel.bin ^
    obj\multiboot.o ^
    obj\gdt_flush.o ^
    obj\idt_load.o ^
    obj\isr_asm.o ^
    obj\irq_asm.o ^
    obj\kernel.o ^
    obj\gdt.o ^
    obj\idt.o ^
    obj\lowlevel_io.o ^
    obj\pic.o ^
    obj\isr.o ^
    obj\irq.o ^
    obj\panic.o ^
    obj\string.o ^
    obj\vga.o ^
    obj\keyboard.o ^
    obj\shell.o ^
    obj\serial.o ^
    obj\memory.o

if errorlevel 1 goto :error

:: ============================
:: UEFI ISO
:: ============================
echo [BUILD] Copy kernel
:; =====================================================================================================
if not exist bin\iso mkdir bin\iso
if not exist bin\iso\boot mkdir bin\iso\boot
if not exist bin\iso\boot\grub mkdir bin\iso\boot\grub

copy bin\kernel.bin bin\iso\boot\kernel.bin > nul

(
    echo set default=0
    echo set timeout=1
    echo menuentry "GiperbolaOS" { multiboot2 /boot/kernel.bin }
) > bin/iso/boot/grub/grub.cfg

wsl grub-mkrescue -o bin/GiperbolaOS.iso bin/iso

wsl qemu-system-i386 -cdrom bin/GiperbolaOS.iso -serial stdio -m 200M
:; =====================================================================================================

:: ============================
:: ERROR HANDLER
:: ============================
:error
echo.
echo [!] Build failed!
pause

:end