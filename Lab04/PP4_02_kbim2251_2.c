#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MIN_DOC 1
#define MAX_DOC 10
#define MAX_PRINTINGS 15
#define MAX_CONCURRENT_PRINTS 3
#define NUM_WORKERS 5

int totalPrintedDocs = 0;
int inkLevel = MAX_PRINTINGS;
pthread_mutex_t printerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t inkChangeCond = PTHREAD_COND_INITIALIZER;

void* workerFunction(void* workerID) {
    int id = *((int*)workerID);

    while (totalPrintedDocs < MAX_PRINTINGS) {
        int numDocsToPrint = rand() % MAX_CONCURRENT_PRINTS + 1;

        pthread_mutex_lock(&printerMutex);

        if (inkLevel <= 0) {
            printf("Worker %d notices that the printer is out of ink.\n", id);
            pthread_cond_wait(&inkChangeCond, &printerMutex);
            printf("Worker %d is informed that the ink has been changed. Resuming printing.\n", id);
        }

        printf("Worker %d is printing %d documents.\n", id, numDocsToPrint);

        // Simulate printing
        sleep(1);

        totalPrintedDocs += numDocsToPrint;
        inkLevel -= numDocsToPrint;

        printf("Worker %d has finished printing. Total printed documents: %d. Ink level: %d\n", id, totalPrintedDocs, inkLevel);

        pthread_mutex_unlock(&printerMutex);

        // Simulate a pause before printing again
        sleep(rand() % 3 + 1);
    }

    // Signal that this worker has finished printing
    pthread_cond_signal(&inkChangeCond);

    pthread_exit(NULL);
}

int main() {
    pthread_t workers[NUM_WORKERS];
    int workerIDs[NUM_WORKERS];

    // Create worker threads
    for (int i = 0; i < NUM_WORKERS; i++) {
        workerIDs[i] = i + 1;
        pthread_create(&workers[i], NULL, workerFunction, &workerIDs[i]);
    }

    // Supervisor enters the office
    printf("Supervisor enters the office.\n");

    // Wait for all workers to finish printing
    pthread_mutex_lock(&printerMutex);
    while (totalPrintedDocs < MAX_PRINTINGS) {
        pthread_cond_wait(&inkChangeCond, &printerMutex);
    }
    pthread_mutex_unlock(&printerMutex);

    // Ink change process
    printf("Supervisor notices that the printer is out of ink. Changing the ink...\n");
    sleep(2);
    inkLevel = MAX_PRINTINGS;
    printf("Supervisor has changed the ink. Resuming printing.\n");

    // Inform workers that ink has been changed
    pthread_mutex_lock(&printerMutex);
    pthread_cond_broadcast(&inkChangeCond);
    pthread_mutex_unlock(&printerMutex);

    // Join worker threads
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(workers[i], NULL);
    }

    // Supervisor leaves the office
    printf("Supervisor leaves the office. All workers have finished printing.\n");

    return 0;
}
