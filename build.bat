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

set OBJDIR=obj
set BINDIR=bin
set KERNEL=%BINDIR%\kernel.bin
set ISO=%BINDIR%/GiperbolaOS.iso

:: ============================
:: CLEAN PREVIOUS BUILD
:: ============================
if exist %ISO% del /q %ISO%
if not exist %OBJDIR% mkdir %OBJDIR%
if not exist %BINDIR% mkdir %BINDIR%

:: ============================
:: BUILD ASM FILES
:: ============================
echo [ASM] Building boot and low-level
%NASM% -f elf32 arch\x86\boot\multiboot.asm   -o %OBJDIR%\multiboot.o
%NASM% -f elf32 arch\x86\boot\gdt_flush.asm   -o %OBJDIR%\gdt_flush.o
%NASM% -f elf32 arch\x86\boot\idt_load.asm    -o %OBJDIR%\idt_load.o
%NASM% -f elf32 arch\x86\interrupts\isr.asm   -o %OBJDIR%\isr_asm.o
%NASM% -f elf32 arch\x86\interrupts\irq.asm   -o %OBJDIR%\irq_asm.o

:: ============================
:: BUILD C++ FILES
:: ============================
echo [C++] Compiling kernel and modules
%GCC% %CFLAGS% -c kernel\Kernel.cpp              -o %OBJDIR%\kernel.o
%GCC% %CFLAGS% -c kernel\Panic.cpp               -o %OBJDIR%\panic.o

%GCC% %CFLAGS% -c arch\x86\gdt\GDT.cpp           -o %OBJDIR%\gdt.o
%GCC% %CFLAGS% -c arch\x86\interrupts\IDT.cpp    -o %OBJDIR%\idt.o
%GCC% %CFLAGS% -c arch\x86\io\LowLevel_io.cpp    -o %OBJDIR%\lowlevel_io.o
%GCC% %CFLAGS% -c arch\x86\interrupts\Pic.cpp    -o %OBJDIR%\pic.o
%GCC% %CFLAGS% -c arch\x86\interrupts\isr.cpp    -o %OBJDIR%\isr.o
%GCC% %CFLAGS% -c arch\x86\interrupts\irq.cpp    -o %OBJDIR%\irq.o

%GCC% %CFLAGS% -c drivers\VGA.cpp                -o %OBJDIR%\vga.o
%GCC% %CFLAGS% -c drivers\Keyboard.cpp           -o %OBJDIR%\keyboard.o

%GCC% %CFLAGS% -c lib\String.cpp                 -o %OBJDIR%\string.o
%GCC% %CFLAGS% -c shell\Shell.cpp                -o %OBJDIR%\shell.o

:: ============================
:: LINK KERNEL
:: ============================
echo [LD] Linking kernel
%LD% -T linker\linker.ld -nostdlib -o %KERNEL% ^
    %OBJDIR%\multiboot.o ^
    %OBJDIR%\gdt_flush.o ^
    %OBJDIR%\idt_load.o ^
    %OBJDIR%\isr_asm.o ^
    %OBJDIR%\irq_asm.o ^
    %OBJDIR%\kernel.o ^
    %OBJDIR%\gdt.o ^
    %OBJDIR%\idt.o ^
    %OBJDIR%\lowlevel_io.o ^
    %OBJDIR%\pic.o ^
    %OBJDIR%\isr.o ^
    %OBJDIR%\irq.o ^
    %OBJDIR%\panic.o ^
    %OBJDIR%\string.o ^
    %OBJDIR%\vga.o ^
    %OBJDIR%\keyboard.o ^
    %OBJDIR%\shell.o

if errorlevel 1 goto :error

:: ============================
:: CREATE ISO
:: ============================
echo [ISO] Building bootable image

if not exist %BINDIR%\iso mkdir %BINDIR%\iso
if not exist %BINDIR%\iso\boot mkdir %BINDIR%\iso\boot
if not exist %BINDIR%\iso\boot\grub mkdir %BINDIR%\iso\boot\grub

copy %KERNEL% %BINDIR%\iso\boot\kernel.bin > nul

(
    echo set default=0
    echo set timeout=1
    echo menuentry "GiperbolaOS" { multiboot /boot/kernel.bin }
) > %BINDIR%\iso\boot\grub\grub.cfg

wsl grub-mkrescue -o %ISO% %BINDIR%/iso
if errorlevel 1 goto :error

:: ============================
:: RUN IN QEMU
:: ============================
echo [QEMU] Starting emulator
%QEMU% -cdrom %ISO%
goto :end

:: ============================
:: ERROR HANDLER
:: ============================
:error
echo.
echo [!] Build failed!
pause

:end