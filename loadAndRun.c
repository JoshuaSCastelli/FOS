/*
 * 	loadAndRun.c
 *	Joshua Castelli/Nathan Helmig
 * 	desription: demonstrates some of the features of the fos-kernel
 *
 */
 
/**************************************************************
	#includes
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fos-kernel2.h"
#include "computer2.h"
#include "vmm.h"

/**************************************************************
	#defines
**************************************************************/
//#define TRUE 1
//#define FALSE 0
#define MAX_PROCESS 10

/**************************************************************
	Global Variables
**************************************************************/
int pid;
int address;
int processSize;
Process pTableEntry[10];
FILE* progFile;
int pageSize;

/**************************************************************
	Prototypes
**************************************************************/
void initialize();
void getCommand();
void ps();
void runProg();
void loadProg();
void dpt();


/**************************************************************
	Functions
**************************************************************/


/****Initialize************************************************
	initializes the Kernel, process table, VMM, and PID
**************************************************************/
void initialize(){

	// prepare to use the FOS kernel
	initFOSKernel1(pageSize);

	// set values in the process table
	// second parameter is size of table
	initProcessTable(pTableEntry, 10);
	
	initVMM();
	// pid = process ID
	// not needed for this program, but needed to use fos-kernel
	pid = 0;
}
/****Command Prompt********************************************
	getCommand prompts the user for commands
	
	Commands include:
	load: 		loads a program into memory
	run:		runs a designated process to termination
	ps:			displays the process table
	dpt:		displays the page table
	osnoise:	toggles the OS debugging output
	cpunoise:	toggles the CPU debugging output
	memnoise:	toggles the main memory debugging output
	vmemnoise:	toggles the secondary memory debugging output
	noise:		toggles all debugging outputs
	exit: 		terminates the OS program
	
**************************************************************/
void getCommand(){
	/* Temporary variables */
	int commandFound = FALSE;
	char command[8];
	
	/* Loops until a valid command is entered */
	while(commandFound == FALSE){
		
		/* prompts user for command */
		printf("Enter a command: ");
        scanf("%s",command);
		
		/* Directs the program to the proper function based on the command */
		if(strcmp(command,"exit") == 0){
			exit(1);
		}else if(strcmp(command,"load") == 0){
			loadProg();
		}else if(strcmp(command,"run") == 0){
			runProg();
		}else if(strcmp(command,"ps") == 0){
			ps();
		}else if(strcmp(command,"osnoise") == 0){
			toggleOSNoise();
			printf("OS Noise toggled\n");
		}else if(strcmp(command,"cpunoise") == 0){
			toggleCPUNoise();
			printf("CPU Noise toggled\n");
		}else if(strcmp(command,"memnoise") == 0){
			toggleMEMNoise();
			printf("MEM Noise toggled\n");
		}else if(strcmp(command,"vmemnoise") == 0){
			toggleVMEMNoise();
		}else if(strcmp(command,"dpt") == 0){
			dpt();
		}else if(strcmp(command,"noise") == 0){
			toggleCPUNoise();
			toggleMEMNoise();
			toggleOSNoise();
			toggleVMEMNoise();
		}else{
			printf("please enter a valid command\n");	
		}	
	}
}

/****Process State*********************************************
	ps displays the contents of the process table
**************************************************************/
void ps(){
	/* Format of ps: */
	/* PID	Code	PC */
	
	int temp = 0;
	printf("===Process Table===\n");
	printf("PID\tCode\tPC\n");
	for(int i = 0;i < 10; i++){
		if(pTableEntry[i].pid>0){
			printf("%d\t%d\t%d\n",pTableEntry[i].pid,pTableEntry[i].codeSize,pTableEntry[i].cpu.pc);
			temp++;
		}
	}
	if(temp == 0){
		printf("   (EMPTY TABLE)\n");
	}
	printf("===================\n");
}

/****Display Page Table(dpt)***********************************
	dpt displays the conents of the page table
**************************************************************/
void dpt(){
	/*  Format of dpt: */
	/*	Page	PID		FREE	vPage 	Dirty 	lastRef		*/
	printf("=======================Page Table=======================\n");
	printf("Page\tPID\tFREE\tvPage\tmPage\tDirty\tlastRef\n");
	for(int i = 0; i < getNumSecPages();i++){
		if(pageTable[i].free == 0){
			printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n",i,pageTable[i].pid,pageTable[i].free,pageTable[i].vPage,pageTable[i].mainPageFrame,pageTable[i].dirty,pageTable[i].lastRef);
		}else{
			printf("%d\t(EMPTY PAGE)\n",i);
		}
	}
	printf("========================================================\n");
}

/****Run Program***********************************************
	runProg asks the user for a PID to run until termination
**************************************************************/
void runProg(){
	
	/* Local Variables */
	int tempPID=-1;
	int tempIndex = -1;
	
	/* Ask user for PID to run*/ 
	printf("Enter a PID to run: ");
    scanf("%d",&tempPID);
	
	/* Checks that the user input is valid */
	if(tempPID <=0 && tempPID >=pid){
		printf("please enter a valid PID\n");
		runProg();
	}
	
	/* Searches for PID in process table and saves its index */
	for(int i = 0; i < 10; i++){
		int found = -1;
		if(pTableEntry[i].pid == tempPID && found == -1){
			tempIndex = i;
			found = 0;
		}
	}
	
	/* If process wasn't found in process table, returns to command prompt */
	if(tempIndex == -1){
		printf("PID was not found\n");
		getCommand();
	}
	
	/* Initialize the CPU to run a process. */ 
	initCPU();
	
	/***************DEBUG************************/
	/*info about the process that is about to run.*/
	if(VMEM_NOISE){
		printf("tempIndex %d\n",tempIndex);						//the processes index in pTableEntry
		printf("pTable.pid %d\n",pTableEntry[tempIndex].pid);	//the PID of the process
		for(int i = 0; i < 10; i++){							//contents of pTableEntry
			printf("pTableEntry.pid: %d\n",pTableEntry[i].pid);
		}
	}
	/********************************************/
	
	/* Process will run to completion  */
	while(startProcess(&pTableEntry[tempIndex]) == CLOCK_TICK) {
		if(VMEM_NOISE) printf("Saving state\n");
		saveProcessState(&pTableEntry[tempIndex]);
	}
	
	/* The page table is cleaned up after a process is terminated */
	if(VMEM_NOISE) printf("PROCESS %d TERMINATED\n", pTableEntry[tempIndex].pid);
	pageTableProcessTerm(pTableEntry[tempIndex].pid);
	pTableEntry[tempIndex].pid = 0;
	
	
	/*Return to command prompt */
	getCommand();
}

/****Load Program**********************************************
	loadProg prompts for a filename of a program from the user
	and attempts to load the file into secondary memory.
**************************************************************/
void loadProg(){
	/* Temporary variables */
	char fileName[30];
	
	FILE* progFile;
	Process* ptEntry = NULL;
	
	/* Prompts user for filename */
	printf("enter a file name:");
	scanf("%s",fileName);
	
	/* Searches process table for empty slot and saves the address of the empty slot to ptEntry */ 
	for(int ptEntryIndex = 0; ptEntryIndex < MAX_PROCESS && ptEntry == NULL; ptEntryIndex++){
		if(pTableEntry[ptEntryIndex].valid == FALSE){
			ptEntry = &pTableEntry[ptEntryIndex];
		}
	}
	
	/* Opens the file designated by the user */
	progFile = openProgFile(fileName,ptEntry);
	
	/* On failure to open file, returns to command prompt with error */
	if(progFile == NULL){
		printf("failed to load\n");
		getCommand();
	}
	
	/* Calculates the size of the process trying to load */
	processSize = ptEntry->codeSize 
				   + ptEntry->heapSize 
				   + ptEntry->stackSize;
	
	
	
	/* Checks how many pages in secondary memory are FREE */
	int emptyPages = 0;
	for(int i = 0; i < getNumSecPages(); i++){
		if(pageTable[i].free == TRUE){
			emptyPages++;
		}
	}
	
	/* DEBUGGING ONLY Info about process and secondary memory */
	if(VMEM_NOISE) printf("processSIZE: %d words\n", processSize);
	if(VMEM_NOISE) printf("codeSIZE: %d words\n",ptEntry->codeSize);
	if(VMEM_NOISE) printf("heapSIZE: %d words\n",ptEntry->heapSize);
	if(VMEM_NOISE) printf("stackSIZE: %d words\n",ptEntry->stackSize);
	if(VMEM_NOISE) printf("Pages left in SecondaryMEM: %d\n",emptyPages);
	if(VMEM_NOISE) printf("Pages needed for process: %d\n", processSize/getPageSize());
	
	/* 
		If there are no free pages in secondary memory or process size 
		is larger than the empty space in secondary memory returns to 
		command prompt with error.
	*/
	if(emptyPages ==  0){
		printf("Cannot load file, secondary memory is full\n");
		getCommand();
	}else if(emptyPages*getPageSize() <= processSize){
		printf("Cannot load file, not enough space in secondary memory\n");
		getCommand();
	}else{
		if(VMEM_NOISE) printf("ProcessSize < remaining sMEM, loading process...\n");
	}
	
	FILE* notFullyLoaded = progFile;
	int vPage = 0;
	/* Loops until process is full loaded into secondary memory */
	do{
		int pageFrame;
		
		/* Finds a free page in secondary memory */
		pageFrame = pageTableGetFreeSecPage();
		
		/* On failure to find secondary page, returns to command prompt with error */
		if(pageFrame == -1){
			printf("failed to find secondary page, sMEM possibly full\n");
			getCommand();
		}
		
		/* Updates the page table to reflect the loading of a page into secondary memory */
		pageTableLoadProcessToSecFrame(pageFrame, (pid)+1);
		pageTable[pageFrame].vPage = vPage++;
		
		/* Loads a page of the program to the free page found in secondary memory */
		notFullyLoaded = loadProgFileToPage(progFile,ptEntry,pageFrame,&pid);
	}while(notFullyLoaded != NULL);
	
	/* When fully loaded to secondary memory, returns to command prompt */
	getCommand();
}

/****MAIN******************************************************
	MAIN FUNCTION - Entry point of the OS program.
**************************************************************/
int main(int argc, char* argv[]){
	
	/* User must provide three commandline arguments for main and secondary memory. */ 
	if(argc != 4) {
		fprintf(stderr, "Usage: %s mainMemorySize secondaryMemorySize pageSize\n", argv[0]);
		exit(1);
	}
	
	/* Variables for the commandline arguments */
	int main = atoi(argv[1]);
	int secondary = atoi(argv[2]);
	pageSize = atoi(argv[3]);
	
	/* Creation of main/secondary memory based on commandline args */
	createMainMem(main);
	createSecMem(secondary);
	
	/* Initializes the Kernel, Process Table, VMM, and PID */
	initialize();
	
	/* Starts user input (AKA: command prompt) */
	getCommand();
	
	/* Debugging Purposes ONLY *******************************
	// this is not needed, but a way to see what is in memory
	// this dumps from 0 to the size of the process
	mainMemDump(0, processSize);

	// this is not needed, but turns on verbose output to see 
	// what is going on inside cpu
	int cpuNoise = toggleCPUNoise();
	printf("cpu noise is %d\n", cpuNoise);
	*********************************************************/
	
	return 0;
}