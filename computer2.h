/*
 * computer2.h
 *
 * simplified representations of the cpu and memory systems
 * now with virtual memory management
 *
 */

#ifndef COMPUTER_H
#define COMPUTER_H

#include "memsys2.h"
#include "frisc2.h"

extern FriscCPU cpu;
extern MAIN_MEM mainMem;
extern SEC_MEM secMem;

#define TRUE 1
#define FALSE 0

#define CPU_NOISE_MASK 1
#define CPU_NOISE (noise & CPU_NOISE_MASK)

#define MEM_NOISE_MASK 2
#define MEM_NOISE (noise & MEM_NOISE_MASK)

#define OS_NOISE_MASK 4
#define OS_NOISE (noise & OS_NOISE_MASK)

#define VMEM_NOISE_MASK 8
#define VMEM_NOISE (noise & VMEM_NOISE_MASK)

int getVMemNoise();

/* 
 * hardware
 *
 * non-cpu elements: clock and machine word
 *
 */

long clock;

#define WORD long

/*
 * cpu
 *
 * details of cpu are in frisc.c
 * those details are not needed for first project
 */

#define RUN_LIMIT 6

/* 
 * this typedef struct is defined elsewhere
 * included here for (easy) reference
 *
typedef struct {
  int  pid;
  WORD pc;
  WORD psw;
  WORD sp;
  WORD reg[10];
  INST_REG inst;
  INST_REG addr;
} FriscCPU;
 *
 */

// cpu methods

// initialize the cpu - reset register values
// void initCPU();


/*
 * memory system
 */

/*
 * do this before using main memory
 */
int createMainMem(int size);

/*
 * do this before using secondary memory
 */
int createSecMem(int size);

/*
 * copy from secondary memory to main memory
 * param: sStart - starting address in secondary memory
 * param: mStart - starting address in main memory
 * param: words - number of words to copy
 */
int copySecToMain(int sStart, int mStart, int words);

/*
 * copy from main memory to secondary memory
 * param: mStart - starting address in main memory
 * param: sStart - starting address in secondary memory
 * param: words - number of words to copy
 */
int copyMainToSec(int mStart, int sStart, int words);


#endif
