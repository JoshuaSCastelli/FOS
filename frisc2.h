/*
 * frisc2.h
 * v1.0
 */

#ifndef FRISC_H
#define FRISC_H


long clock;

#define WORD long
#define WORDSIZE sizeof(WORD)

#define LODM 'm'
#define LOIM 'l'
#define STDM 's'
#define STIM 't'
#define INCR 'i'
#define DECR 'r'
#define ADDR 'a'
#define SUBR 'u'
#define COMP 'c'
#define BRAN 'b'
#define BRNN 'n'
#define CLER 'z'
#define DISC 'd'
#define DISM 'o'
#define EXIT 'e'
#define NOP  'x'
#define TX2N 'T'
#define N2TX 'N'
#define PUSH 'P'
#define POP  'O'
#define GOSU 'S'
#define RETU 'R'

typedef union inst {
  WORD w;
  char s[5];
} INST_REG;

typedef struct {
  int  pid;
  WORD pc;
  WORD psw;
  WORD sp;
  WORD reg[10];
  INST_REG inst;
  INST_REG addr;
} FriscCPU;

FriscCPU cpu;

// reasons why the CPU returns control to OS
typedef enum {CLOCK_TICK,  // reached run limit
                PROCESS_END, // reached end of user program
                BAD_INSTR,   // instruction is malformed
                BUS_ERROR    // illegal acccess to memory
               }CPU_STATE;

void initCPU();

#include "fos-kernel2.h"

// CPU_STATE startProcess(Process *processState);

// void saveProcessState(Process *processState);

CPU_STATE runCPU();

#endif
