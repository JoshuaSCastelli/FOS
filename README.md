# FOS

# Description
FOS is a fake operating system created to learn how an actual operating system works at the fundemental level. It encompases elements of loading and executing programs/processes. It also manages pages and page frames using a virtual memory management system. 

# How to run FOS
# *Note* FOS was built on a UNIX system and may have issues with other operating systems. For optimal functionality, run on a UNIX system.

# Step 1:
Copy all files from the repository into a directory. Open a command prompt and navigate to the directory where the files are stored.

# Step 2:
Compile the code using the following command(without the quotes):
"gcc -o FOS loadAndRun.c vmm.c computer2.o fos-kernel2.o"
This will generate a file called FOS.

# Step 3:
Run the program by typing "./FOS sMemSize mMemSize pSize" into the command prompt, replacing sMemSize with a secondary memory size of       your choosing. The same goes for mMemSize(main memory size) and pSize(page size - number of words/page). These are just integer values. I   recommend something like "./FOS 20 20 1" to see many pages(since page size = 1, individual words take up a whole page) get put into         memory or "./FOS 50 50 8" for a less populated memory(8 words per page, therefore less pages used). Mess around with it and try different   things to get an understanding of how words relate to pages and the space they take up in memory.
  
# Operating FOS
There are two included files in the repository with the file extension ".fex2". These are the programs you'll use to load into memory.
You should be greeted with a "Enter Command: " command prompt. You have the following commands at your disposal:

load: 		  loads a program into memory(the ".fex2" files)
run:		    runs a designated process to termination
ps:			    displays the process table  (shows all processes in memory)
dpt:		    displays the page table     (shows all pages in memory)
osnoise:	  toggles the OS debugging output
cpunoise:	  toggles the CPU debugging output
memnoise:	  toggles the main memory debugging output
vmemnoise:	toggles the secondary memory debugging output
noise:		  toggles all debugging outputs
exit: 		  terminates the OS program

# Enjoy
Feel free to have a look at the code and get a bit of an understanding how a real operating system loads, runs, and manages pages in memory.
