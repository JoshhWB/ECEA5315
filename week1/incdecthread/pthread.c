#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>
#include <time.h>

// Number of threads to create
#define NUM_THREADS 128

// Constants for course and assignment identification
#define COURSE 1
#define ASSIGNMENT 2

// Thread parameter structure
typedef struct
{
    int threadIdx; // Index of the thread
} threadParams_t;

// POSIX thread declarations and scheduling attributes
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

// Function to introduce a delay in milliseconds
void delay(unsigned int milisec)
{
    clock_t end = milisec + clock(); // Calculate the end time for the delay
    while (end > clock()); // Wait until the end time is reached
}

// Thread function to calculate the sum of numbers up to the thread index
void *counterThread(void *threadp)
{
   int sum = 0, i;
   threadParams_t *threadParams = (threadParams_t *)threadp; // Cast the void pointer to threadParams_t

   // Calculate the sum
   for(i = 1; i < (threadParams->threadIdx) + 1; i++)
      sum = sum + i;

   // Log the result to the system log
   syslog(LOG_DEBUG, "Thread idx=%d, sum[0...%d]=%d\n",
           threadParams->threadIdx,
           threadParams->threadIdx, sum);
}

int main (int argc, char *argv[])
{
    // Clear the system log and log system information
    system("echo > /dev/null | tee /var/log/syslog");
    system("logger [COURSE:1][ASSIGNMENT:2]: `uname -a` | tee /var/log/syslog");

    openlog("[COURSE:1][ASSIGNMENT:2]", LOG_NDELAY, LOG_DAEMON); // Open the system log

    // Create threads
    for(int i = 0; i < NUM_THREADS; i++) {
        threadParams[i].threadIdx = i; // Assign thread index

        pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );
   }

   // Wait for threads to finish
   for(int i = 0; i < NUM_THREADS; i++)
       pthread_join(threads[i], NULL);

    closelog(); // Close the system log

    delay(100000); // Introduce a delay

    // Copy the system log to a file
    system("cp /var/log/syslog assignment2.txt");

   printf("TEST COMPLETE\n"); // Print a test complete message
   return 0;
}