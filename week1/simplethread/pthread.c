#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>

#include <syslog.h>
#include <time.h>
#include <string.h>

#define NUM_THREADS 12 // Number of threads to create

// Constants for course and assignment identification
#define COURSE 1
#define ASSIGNMENT 1


typedef struct
{
    int threadIdx;
} threadParams_t;

// POSIX thread declarations and scheduling attributes
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

// Function to log a message to the system log
void sysHello(const char *msg) {
    openlog("pthread", LOG_PID, LOG_USER); // Open the system log
    syslog(LOG_INFO, "[COURSE:%d][ASSIGNMENT:%d]%s", COURSE, ASSIGNMENT, msg); // Log the message
    closelog(); // Close the system log
}

// Thread function to calculate the sum of numbers up to the thread index
void *counterThread(void *threadp)
{
    int sum = 0, i;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    // Log a hello message from the first thread
    if (threadParams->threadIdx == 0) {
        sysHello("Hello World from Thread!");
    }

    // Calculate the sum
    for(i = 1; i < (threadParams->threadIdx) + 1; i++)
        sum = sum + i;

    // Print the result
    printf("Thread idx=%d, sum[0...%d]=%d\n",
           threadParams->threadIdx,
           threadParams->threadIdx, sum);
}

// Function to log system information (uname -a)
void sysUname() {
    FILE *unameCommand = popen("uname -a", "r"); // Open a pipe to the uname command
    if (unameCommand == NULL) {
        perror("popen"); // Handle error if pipe open fails
        return;
    }

    char unameOutput[256]; // Buffer to store the output of uname
    size_t len = 0;

    // Read the output of uname line by line
    while (fgets(unameOutput, sizeof(unameOutput), unameCommand) != NULL) {
        len = strlen(unameOutput);
        if (len > 0 && unameOutput[len - 1] == '\n') {
            unameOutput[len - 1] = '\0'; // Remove the newline character
        }
        // Log the output of uname
        sysHello(unameOutput);
    }

    pclose(unameCommand); // Close the pipe
}

int main (int argc, char *argv[])
{
   int rc;
   int i;

   sysUname(); // Log system information
   sysHello("Hello World from Main!"); // Log a hello message from the main thread

   // Create threads
   for(i = 0; i < NUM_THREADS; i++)
   {
       threadParams[i].threadIdx=i;

       pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );
   }

   // Wait for threads to finish
   for(i = 0; i < NUM_THREADS; i++)
       pthread_join(threads[i], NULL);

   printf("TEST COMPLETE\n"); // Print a test complete message
}