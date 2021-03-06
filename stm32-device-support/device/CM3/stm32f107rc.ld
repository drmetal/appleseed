/*
*****************************************************************************
**
**  File        : stm32_flash.ld
**
**  Abstract    : Linker script for STM32F107RC Device with
**                256KByte FLASH, 64KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Environment : Atollic TrueSTUDIO(R)
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
**
**  Not so minor modifications by Martin Thomas (not Attolic) inspired
**  by the default linker-script (arm-none-eabi-ld --verbose), the CS3 linker-
**  scripts, binutils-documentations, GNU Tools for ARM Embedded Processors
**  readme-file and examples for other targets.
**  No extra copyright on modifications.
**
*****************************************************************************
*/

/* Specify the memory areas */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 256K
  CCRAM (xrw)     : ORIGIN = 0x10000000, LENGTH = 0K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K
  MEMORY_B1 (rx)  : ORIGIN = 0x60000000, LENGTH = 0K
}

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x3000; /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Highest address of the user mode stack */
/* STM32F1 use SYSTEM RAM */
_estack = ORIGIN(RAM) + LENGTH(RAM);

INCLUDE stm32.ld