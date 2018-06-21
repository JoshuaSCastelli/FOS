/*
 * memsys2.h
 *
 * now supports virtual memory management
 *
 */

#ifndef MEMSYS_H
#define MEMSYS_H

#include "frisc2.h"

#define MAIN_MEM WORD*
#define SEC_MEM  WORD*

extern int memSystemInit;

#define MEM_SYS_INIT if(memSystemInit!=3){fprintf(stderr,"memory system not initialized %d - exit\n",memSystemInit);exit(2);}

int getMainMemSize();

int getSecMemSize();

int mainMemDump(int sAddr, int eAddr);

// read a value from main memory
WORD readWordFromMain(WORD address);

// read value from mem[cpu.pc] to cpu.inst
int readInst();

// read value from mem[cpu.pc] to cpu.addr
int readAddr();

// read value from mem[cpu.pc] to cpu.reg[r]
int readImmed(int r);

// read value from mem[cpu.addr] to cpu.reg[r]
int readFromMainMemToReg(int r);

// write value to mem[cpu.addr] from cpu.reg[r]
int writeMem(int r);

int writeInstToSec(int addr, char instr[5]);

int writeDataToSec(int addr, WORD data);

int writeDataToMain(int addr, int dataValue);

// read value from mem[cpu.sp]
WORD pop(void);

// write value to mem[cpu.sp]
void push(WORD value);

int createMainMem(int size);

int createSecMem(int size);

int copySecToMain(int sStart, int mStart, int words);

int copyMainToSec(int mStart, int sStart, int words);

#endif
