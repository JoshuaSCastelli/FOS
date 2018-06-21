/*
 * vmm.c 
 * virtual memory management for fos os
 * Joshua Castelli/Nathan Helmig
 * version 0.91
 */

#include "vmm.h"
#include "computer2.h"
#include "fos-kernel2.h"
#include <stdlib.h>
// #include <stdio.h>

/*
 * the functions in this file fall under two categories:
 *   1. functions that access / update the pageTable
 *   2. functions that use the pageTable to do address translation
 *
 * functions in the first category all all named pageTable*
 * (that is, the names of the functions all begin with "pageTable")
 * there is nothing 'magic' about this naming, except that it should
 * could help the programmer keep straight the intention of the functions
 *
 */

/*================================================================================*/
/*
 * initVMM
 * make the necessary data structures for virtual memory
 *
 * return
 *    0 if success
 *    non-zero value for failure
 */
int initVMM(){
	if(VMEM_NOISE) printf("VMEM: init\n");
	// create memory for and initialize page table(s)
	pageTable = calloc(getNumSecPages(), sizeof(PageTableRec));
	if(pageTable == 0){
	  fprintf(stderr, "failed to create pageTable data structure\n");
	  return 2;
	}
	for(int page = 0; page < getNumSecPages(); page++){
	  pageTable[page].free = TRUE;
	  pageTable[page].vPage = -1;
	  pageTable[page].mainPageFrame = -1;
	}

	return 0;
}
/*================================================================================*/
/*
 * pageTableGetFreeSecPage
 *    search the pageTable for a free secondary page
 *    find the first entry where free == TRUE
 *    the index of the entry is also the secondary page number
 *
 *    return
 *       the secondary page number of a currently free frame
 *       -1 on failure (no secondary page available)
 */
int pageTableGetFreeSecPage(Process pTableEntry){
	if(VMEM_NOISE) printf("VMEM: Searching for secondary page...\n");
	int success = -1;
	int found = -1;
	for(int i = 0; i < getNumSecPages(); i++){
		if(pageTable[i].free == TRUE && found == -1){
			pageTable[i].free=FALSE;
			pageTable[i].vPage = i;
			pageTable[i].pid = pTableEntry.pid;
			success = i;
			found = 0;
		}
	}
	return success;
}
/*================================================================================*/

/*================================================================================*/
/*
 * pageTableLoadProcessToSecFrame
 *    this does not do the loading (done in kernel)
 *    this records in the page frame that a page of a process
 *    has been loaded in secondary memory
 *
 * return
 *    0 success
 *    -1 failure (secondary page is already occupied)
 */
int pageTableLoadProcessToSecFrame(int sPageFrame, int pid){
    if(VMEM_NOISE) printf("VMEM: Loading sPageFrame %d\n",sPageFrame);
	int success = -1;
 	for(int i = 0; i < getNumSecPages(); i++){
		if(pageTable[i].vPage == sPageFrame){
			pageTable[i].pid = pid;
			success = 0;
		}	
	} 
    return success;
}
/*================================================================================*/


/*================================================================================*/
/*
 * pageTableAccessPageFrame
 *    update the pageTable with last access time
 *    used for (some) page replacement algorithms
 *
 *    parameters
 *       mPageFrame - the main memory page frame accessed
 *       write (boolean) - 0 if reading, non-zero for write
 *
 *    return (int)
 *       0 success - page table updated appropriately
 *       -1 failure - page referenced is unoccupied
 */
int pageTableAccessPageFrame(int mPageFrame, int write){
	if(VMEM_NOISE) printf("VMEM: \n");
	int success = -1;
	if(write == 0){
		for(int i = 0; i < getNumSecPages(); i++){
			if(pageTable[i].mainPageFrame == mPageFrame){
				pageTable[i].lastRef = clock;
				success = 0;
			}
		}
	}else{
		for(int i = 0; i < getNumSecPages(); i++){
			if(pageTable[i].mainPageFrame == mPageFrame){
				pageTable[i].dirty = TRUE;
				pageTable[i].lastRef = clock;
				success = 0;
			}
		}
	}
	return success;
}
/*================================================================================*/

/*================================================================================*/
/*
 * pageTableProcessTerm
 *    update the pageTable to reflect that a process has terminated
 *
 *    return
 *       void
 */
void pageTableProcessTerm(int pid){
	if(VMEM_NOISE) printf("VMEM: \n");
	for(int i = 0; i < getNumSecPages(); i++){
		if(pageTable[i].pid == pid){
			pageTable[i].pid = 0;
			pageTable[i].free = TRUE;
			pageTable[i].vPage = -1;
			pageTable[i].mainPageFrame = -1;
		}
	}
}
/*================================================================================*/

/*================================================================================*/
/*
 * pageTableCopyToPageFrame
 *    update the pageTable to reflect that a page has been (or will be)
 *    copied to main memory
 *
 *    parameters
 *       sPageFrame - secondary memory page frame copied
 *       mPageFrame - main memory page frame where copy was placed
 *
 *    return
 *       0 success
 *       -1 failure 
 */
int pageTableCopyToPageFrame(int sPageFrame, int mPageFrame){
	if(VMEM_NOISE) printf("VMEM: Copying sPage to mPage\n");
	
	int success = -1;
	
	pageTable[sPageFrame].mainPageFrame = mPageFrame;
	
	if(pageTable[sPageFrame].mainPageFrame == mPageFrame){
		success == 0;
	}
	
	
	/*
	for(int i = 0; i < getNumSecPages(); i++){
		if(cpu.pid == pageTable[i].pid){
			if(pageTable[i].vPage == sPageFrame){
				pageTable[i].mainPageFrame = mPageFrame;
				success = 0;
			}
		}
	}
	*/
	
	return success;
}
/*================================================================================*/

/*================================================================================*/
/*
 * pageTablePageEvicted
 *    update pageTable to reflect that a page was (or will be) evicted
 *
 *    parameters
 *       pid - the pid of the process
 *       mPageFrame - the main memory page frame evicted
 *
 *    return
 *       void 
 */
void pageTablePageEvicted(int pid, int mPageFrame){
	if(VMEM_NOISE) printf("VMEM: Evicting mPage %d\n",mPageFrame);
	for(int i = 0; i < getNumSecPages(); i++){
		if(pageTable[i].pid == pid){
			if(pageTable[i].mainPageFrame == mPageFrame){
				pageTable[i].mainPageFrame = -1;
			}
		}
	}
}
/*================================================================================*/

/*================================================================================*/
/*
 * pageTableFindFreeMainPageFrame
 *    find a free page frame in main memory
 *
 *    return
 *       the main page number of a currently free frame
 *       -1 on failure (no secondary page available)
 */
int pageTableFindFreeMainPageFrame(){
	if(VMEM_NOISE) printf("VMEM: Searching for free main page frame\n");
	
	int pageFrame[getNumMainPages()];

	for(int page=0; page < getNumMainPages(); page++){
	  pageFrame[page] = -1;
	}

	for(int page=0; page < getNumSecPages(); page++){
	  if(pageTable[page].free == FALSE && pageTable[page].mainPageFrame != -1){
		 pageFrame[pageTable[page].mainPageFrame] = pageTable[page].pid;
	  }
	}

	for(int page=0; page < getNumMainPages(); page++){
	  if (pageFrame[page] == -1){
		 return page;
	  }
	}

	if(VMEM_NOISE) printf("VMEM: No free main page frame\n");
	return -1;
}
/*================================================================================*/

/*================================================================================*/
/*
 * pageTableFindLRUFrame
 *    find the main memory page frame used least recently
 *
 *    return
 *       a secondary page frame number that corresponds to LRU main page frame
 */
int pageTableFindLRUFrame(){
	if(VMEM_NOISE) printf("VMEM: \n");
	int last = -1;
	int index = -1;
	
	for(int i = 0; i < getNumSecPages(); i++){
		if(pageTable[i].mainPageFrame > -1){
			if(last == -1){
				last = pageTable[i].lastRef;
				index = i;
			}
			else if(pageTable[i].lastRef < last){
				last = pageTable[i].lastRef;
				index = i;
			}
		}
	}
	
	//index == -1 if no page in main mem.
	return index;
}
/*================================================================================*/

/*================================================================================*/
/*
 * readWordFromMainMem
 *    vAddr - a virtual memory address
 *    return - value read from memory
 * 
 * the CPU will call this to read a value from a virtual address
 * this function converts the virtual address to a physical address
 * and reads the value from main memory at the virutal address
 * 
 * this is the only function that reads directly from mainMem[]
 */
WORD readWordFromMainMem(WORD vAddr){
	if(VMEM_NOISE) printf("READ\n");
    if(VMEM_NOISE) printf("vmemnoise: reading vAddr: %ld\n", vAddr);
    WORD pAddr;
	int sPageOrigin,vpage,offset,freeMainPage;
	
	int found = -1;
	for(int i = 0; i < getNumSecPages(); i++){
		if(cpu.pid == pageTable[i].pid && found == -1){
			//vpage = pageTable[i].vPage;
			sPageOrigin=i;//new
			found = 0;
		}
	}
	
	vpage = vAddr/getPageSize();
	offset = vAddr%getPageSize();
	
	if(pageTable[sPageOrigin + vpage].mainPageFrame == -1){
		//page fault
		if(VMEM_NOISE) printf("PAGE FAULT\n");
		freeMainPage = pageTableFindFreeMainPageFrame();
		if(freeMainPage == -1){
			//no free main page found, page replacement needed
			if(VMEM_NOISE) printf("PAGE REPLACEMENT\n");
			freeMainPage = pageReplacement();
		}
		//once a page is found, copy secondary page to main and calculate pAddr.
		copySecToMain((sPageOrigin+vpage)*getPageSize(),freeMainPage*getPageSize(), getPageSize());
		pageTableCopyToPageFrame((sPageOrigin+vpage),freeMainPage);
		pAddr = (freeMainPage * getPageSize()) + offset;
		
	}else{
		pAddr = (pageTable[sPageOrigin+vpage].mainPageFrame * getPageSize()) + offset;
	}
	if(VMEM_NOISE) printf("pAddr: %d\tvpage: %d\tfreemainpage: %d\tsPage: %d\n",pAddr,vpage,pageTable[sPageOrigin+vpage].mainPageFrame,(sPageOrigin+vpage));	
	
	return mainMem[pAddr];
}
/*================================================================================*/

/*================================================================================*/
/*
 * writeWordToMainMem(WORD addr, WORD value)
 *	return:
 * 		0 on success
 *		-1 on failure
 */

int writeWordToMainMem(WORD vAddr, WORD value){
	if(VMEM_NOISE) printf("WRITE\n");
	if(VMEM_NOISE) printf("vmemnoise: reading vAddr: %ld\n", vAddr);
    
	WORD pAddr;
	int sPageOrigin,vpage,offset,freeMainPage;
	int success = -1;
	
	int found = -1;
	for(int i = 0; i < getNumSecPages(); i++){
		if(cpu.pid == pageTable[i].pid && found == -1){
			sPageOrigin=i;
			found = 0;
		}
	}
	
	vpage = vAddr/getPageSize();
	offset = vAddr%getPageSize();
	
	if(pageTable[sPageOrigin + vpage].mainPageFrame == -1){
		//page fault
		if(VMEM_NOISE) printf("PAGE FAULT\n");
		freeMainPage = pageTableFindFreeMainPageFrame();
		if(freeMainPage == -1){
			//no free main page found, page replacement needed
			if(VMEM_NOISE) printf("PAGE REPLACEMENT\n");
			freeMainPage = pageReplacement();
		}
		//once a page is found, copy secondary page to main and calculate pAddr.
		copySecToMain((sPageOrigin+vpage)*getPageSize(),freeMainPage*getPageSize(), getPageSize());
		pageTableCopyToPageFrame((sPageOrigin+vpage),freeMainPage);
		pAddr = (freeMainPage * getPageSize()) + offset;
		
	}else{
		pAddr = (pageTable[sPageOrigin+vpage].mainPageFrame * getPageSize()) + offset;
	}
	if(VMEM_NOISE) printf("pAddr: %d\tvpage: %d\tfreemainpage: %d\tsPage: %d\n",pAddr,vpage,freeMainPage,(sPageOrigin+vpage));	
	mainMem[pAddr] = value;

	if(pAddr < 0 || pAddr >= getMainMemSize()){
		fprintf(stderr, "seg fault in writeWordToMainMem\n");
		exit(1);
	}
	
	if(pAddr >= 0){
		success =0;
	}
	
	return success;
}
/*================================================================================*/


/****Page Replacement Algo*************************************
	pageReplacement is called when a page is not in main 
	memory(AKA page fault) and finds a page using the Least 
	Recently User(LRU) algorithm.
**************************************************************/
int pageReplacement(){
	int returnPage;
	int pageFound = -1;
	//int last = -1;
	
	/* Find LRU Page */
	for(int i = 0; i < getNumSecPages(); i++){
		pageFound = pageTableFindLRUFrame();
	}
	
	/*If the page found is dirty, write it back to secondary memory */
	if(pageTable[pageFound].dirty == TRUE){
		copyMainToSec(pageTable[pageFound].mainPageFrame*getPageSize(), pageTable[pageFound].vPage*getPageSize(), getPageSize());
	}
	
	printf("page replacement found page %d in main\n",returnPage); 
	return returnPage;
}