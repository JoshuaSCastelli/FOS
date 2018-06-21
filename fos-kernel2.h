/*
 * fos-kernel2.h
 *
 * supports virtual memory management
 *
 */

#ifndef FOS_KERNEL_H
#define FOS_KERNEL_H

#include <stdio.h>

#include "frisc2.h"

extern int noise;

/*
 * call one of these first (before any other kernel function)
 */
void initFOSKernel2(int pSize, int q);

void initFOSKernel1(int pSize);

void initFOSKernel();

/*
 * getters
 */

int getPageSize();

int getNumSecPages();
int getNumMainPages();

int getQuantum();

/*
 * change the state of the kernel to turn on (or off) the diagnostic
 * messages of the KERNEL. 
 * side effect: if the noise has just been turned on, a message is produced
 * return: returns 1 if noise is on, 0 if noise is off
 */
int toggleOSNoise();

/*
 * change the state of the kernel to turn on (or off) the diagnostic
 * messages of the CPU. 
 * side effect: if the noise has just been turned on, a message is produced
 * return: returns 1 if noise is on, 0 if noise is off
 */
int toggleCPUNoise();

/*
 * change the state of the kernel to turn on (or off) the diagnostic
 * messages of the Memory System. 
 * side effect: if the noise has just been turned on, a message is produced
 * return: returns 1 if noise is on, 0 if noise is off
 */
int toggleMEMNoise();

/*
 * change the state of the kernel to turn on (or off) the diagnostic
 * messages of the Virtual Memory System. 
 * side effect: if the noise has just been turned on, a message is produced
 * return: returns 1 if noise is on, 0 if noise is off
 */
int toggleVMEMNoise();

/*
 * what is the state of a loaded process ...
 */
typedef enum {
   PROCESS_RUNING,
   PROCESS_READY,
   PROCESS_WAITING,
   PROESS_TERMINATED
   } PROCESS_STATE;

/*
 * a process table entry
 */
typedef struct {
   int valid;          // is this entry used
   int pid;            // process ID
   PROCESS_STATE state;// the state of the process
   int stackSize;      // how many words needed for stack
   int heapSize;       // how many words needed for heap
   int codeSize;       // how many words needed for code
   FriscCPU cpu;       // state of the cpu (all register values)
} Process;

// run a process given by the process state entry
// startProcess will update the process table entry
CPU_STATE startProcess(Process *processState);

/*
 * when a process stops, but not terminated, the state 
 * is usually needed later. this is how to keep it around
 * will change the process table entry with processor state
 */
void saveProcessState(Process *processState);

/*
 * initializes values in the process table
 * do this before using the process table
 */
void initProcessTable(Process pTable[], int pTableSize);

/*
 * this is the first step in creating a process
 * the second step is the next function
 *
 * param: fileName is the name of the program file to open
 * param: process is the process table entry to update
 *
 * return:
 *    on success, the file handle of the open fex file is returned
 *    on failure to open, NULL is returned
 */
FILE* openProgFile(char* fileName, Process *process);

/*
 * this is the second step in creating a process
 * this creates the image of the process in secondary memory
 *
 * param: fin - file handle created by openProgFile (function above)
 * param: process - process table entry is updated with new info
 * param: startAddr - starting address in secondary memory to load
 * param: pid - increments this value
 */
int loadProgFile(FILE* fin, Process *process, int startAddr, int *pid);

/*
 * this is the second step in creating a process in virtual memory
 * this creates the image of the process in ONE page of secondary memory
 * this function may need to be called multiple times to load one process
 *
 * param: 
 *     fin - file handle created by openProgFile (function above)
 *     process - process table entry is updated with new info
 *     pid - pid of the process; value is incremented IF the load is complete (return NULL)
 *     page - the number of the secondary page frame to load into
 *
 * return:
 *     if load completed: NULL
 *     if the load is not completed: the file handle of the program
 */
FILE* loadProgFileToPage(FILE* fin, Process *process, int page, int *pid);

#endif
