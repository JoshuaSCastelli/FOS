/*
 * vmm.h
 * virtual memory management for fos os
 * Joshua Castelli/Nathan Helmig
 * version 0.91
 */

#ifndef VMM_H
#define VMM_H

#include "computer2.h"
#include "fos-kernel2.h"

/*
 * PageTableRec - page table record
 *    each PageTableRec represents a page frame in secondary memory
 *    a secondary page frame begins free (available for a proces to load into)
 *    after loading, the page frame may/not be loaded to main memory for use
 *    when in main mem, the page may be accessed
 *                    , the page may be evicted
 *
 * each PageTableRec has these fields
 *    free          int(bool) - is sec page frame unallocated
 *    pid           int       - process id occupying frame
 *    vPage         int       - the virutal page number of the process
 *    mainPageFrame int       - the main mem page frame that has copy, if any
 *    dirty         int(bool) - has the main mem page frame been written to
 *    lastRef       int       - system clock time of last main page access
 */

typedef struct {
   int free;
   int pid;
   int vPage;
   int mainPageFrame;
   int dirty;
   int lastRef;
} PageTableRec;

PageTableRec *pageTable;

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


/*
 * initVMM
 * make the necessary data structures for virtual memory
 *
 * return
 *    0 if success
 *    non-zero value for failure
 */
int initVMM();

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
int pageTableGetFreeSecPage();

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
int pageTableLoadProcessToSecFrame(int sPageFrame, int pid);

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
int pageTableAccessPageFrame(int mPageFrame, int write);

/*
 * pageTableProcessTerm
 *    update the pageTable to reflect that a process has terminated
 *
 *    return
 *       void
 */

void pageTableProcessTerm(int pid);

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
int pageTableCopyToPageFrame(int sPageFrame, int mPageFrame);

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
void pageTablePageEvicted(int pid, int mPageFrame);

/*
 * pageTableFindFreeMainPageFrame
 *    find a free page frame in main memory
 *
 *    return
 *       the main page number of a currently free frame
 *       -1 on failure (no secondary page available)
 */
int pageTableFindFreeMainPageFrame();

/*
 * pageTableFindLRUFrame
 *    find the main memory page frame used least recently
 *
 *    return
 *       a secondary page frame number that corresponds to LRU main page frame
 */
int pageTableFindLRUFrame();

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
WORD readWordFromMainMem(WORD vAddr);

/*
 * writeWordToMainMem(WORD addr, WORD value)
 *
 */
int writeWordToMainMem(WORD vAddr, WORD value);
int pageReplacement();
#endif
