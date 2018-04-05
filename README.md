# Sieve-of-Eratosthenes
Sieve of Eratosthenes implementation using pthreads in C

## Objectives:
This goal of this assignment is to practice pthread programming for the master/slave parallel computing paradigm.  


## Details:
You're supposed to use pthreads and you'll need to extend the Sieve of Eratosthenes method to find prime numbers. 

In the program, you need to create one master thread and p number of slave threads. p is a command-line argument. The master thread is supposed to dictate work for the slaves. Each slave waits for a piece of work, finishes the work, and then waits for more work. It continues until there is no more work.

More specifically, the master manages an array of all numbers from 2 to n. It loops from k=2 to sqrt(n). n is given as a command line argument. For each k, the master divides the entire range (actually from k^2 to n) into small chunks of size c, which is also a command-line argument. Each chunk is treated as a piece of work, which will be given to a slave.

A slave receives the work, which is a range (with min and max indices to the shared array) and a number k, it then filters out the multiples of k within the given range (by marking them off). After the slave is done it goes back and asks for more work from master. This process continues until no more work left.

The program starts with three parameters: n (the max number we want to find out about prime numbers), p (the number of slave threads), and c (chunk size). The program should print out whether n is a prime number (although the algorithm actually finds all prime numbers from 2 to n).

The master and slave threads should communicate via conditional variables. The program should keep a ready queue, which is a list of thread ids (ranging from 0 to p-1) that are ready for more work. Note that the ready queue needs to be protected from simultaneous access from multiple threads.   Initially the ids of all slaves should be in it. You should use a mutex and a conditional variable (the master is a consumer and all slaves are producers). The master takes one thread from the ready queue at a time, and send to it the next chunk to be worked on. To do it, the master thread communicates with the specific thread using another data structure. The program should also keep a list of data structures, one for each slave thread. The data structure is used for the master to communicate with each slave thread, it should contain min and max indices, and k. It also needs to have a conditional variable, so that the slave thread can use to wait for master's command. (In this case, the master is a producer and the slave is a consumer).

You don't need to care too much about the terminating condition for the slaves. The master prints the result, exits the process by calling exit(0), in which case all slave threads would be terminated.

Submission Instructions:
Please submit your work through moodle as a gzipped tar file. Follow the instructions below carefully (to avoid unnecessary loss of grades):

You should rename the directory where the program and the Makefile are located. Assuming your name is John Smith, you shall name your directory john-smith-hw3 (lower cases). Of course, you need to replace that with your true name. 
Your submission should be a directory that contains only the source code (may it be as many source code files as needed), and the Makefile. Once done, you need to archive and compress the directory by running the following command *in the parent directory* (after you make sure you removed all unnecessary files): 'tar czvf firstname-lastname-hw3.tar.gz firstname-lastname-hw3'. This will create a file named firstname-lastname-hw3.tar.gz, which you should upload to moodle.
Please make sure you submit homework before the given deadline. Late submission will incur penalties as specified in the syllabus. You must submit the homework through moodle. Email submission is ignored.
