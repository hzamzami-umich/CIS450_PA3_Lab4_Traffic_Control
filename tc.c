#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// Constant number of cars
#define NUM_CARS 8

// Struct for Car
typedef struct {
    int cid;
    double arrival_time;
    char dir_original, dir_target;
} Car;

// Struct for Event
typedef struct {
    double time;
    int cid;
    char dir_original, dir_target;
    char type;
} Event;

// Array of cars
Car cars[NUM_CARS] = {
    {1, 1.1, '^', '^'},
    {2, 2.2, '^', '^'},
    {3, 3.3, '^', '<'},
    {4, 4.4, 'v', 'v'},
    {5, 5.5, 'v', '>'},
    {6, 6.6, '^', '^'},
    {7, 7.7, '>', '^'},
    {8, 8.8, '<', '^'}
};

// Array of events
Event events[100];
int count = 0;

// Mutex for thread
pthread_mutex_t lock;

// Hard-coded arrival times
double arrival_times[NUM_CARS] = {
    3.1,
    4.2,
    5.3,
    10.3,
    19.3,
    10.3,
    14.3,
    19.3
};

// Hard-coded exit times
double exit_times[NUM_CARS] = {
    7.1,
    8.2,
    10.3,
    14.3,
    24.3,
    14.3,
    19.3,
    22.3
};

// Thread for the car
void* car_thread(void* arg) {
    Car* c = (Car*)arg;
    
    // Lock thread
    pthread_mutex_lock(&lock);
    
    // Assign int i as 1 less than the current cid
    int i = c->cid - 1;

    // ArriveIntersection event
    events[count++] = (Event){c->arrival_time, c->cid, c->dir_original, c->dir_target, 'A'};

    // CrossIntersection event
    events[count++] = (Event){arrival_times[i], c->cid, c->dir_original, c->dir_target, 'C'};

    // ExitIntersection event
    events[count++] = (Event){exit_times[i], c->cid, c->dir_original, c->dir_target, 'E'};
    
    // Unlock thread
    pthread_mutex_unlock(&lock);

    return NULL;
}

// For qsort to sort each event
int cmp(const void* a, const void* b) {
    Event* e1 = (Event*)a;
    Event* e2 = (Event*)b;
    
    if (e1->time != e2->time)
        return (e1->time > e2->time) - (e1->time < e2->time);

    return e1->type - e2->type; // Arriving < Crossing < Exiting
}

int main() {
    
    // Create an array of threads
    pthread_t threads[NUM_CARS];
    
    // Initialize mutex
    pthread_mutex_init(&lock, NULL);
    
    // Create the threads
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_create(&threads[i], NULL, car_thread, &cars[i]);
    }
    
    // Join the threads
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Sort the events
    qsort(events, count, sizeof(Event), cmp);

    for (int i = 0; i < count; i++) {
        Event e = events[i];
        
        // Print message for arriving car
        if (e.type == 'A') {
            printf("Time %.1f: Car %d (%c %c) arriving\n",
                   e.time, e.cid, e.dir_original, e.dir_target);
        }
        
        // Print message for crossing car
        else if (e.type == 'C') {
            printf("Time %.1f: Car %d (%c %c)\t\tcrossing\n",
                   e.time, e.cid, e.dir_original, e.dir_target);
        }
        
        // Print message for exiting car
        else if (e.type == 'E') {
            printf("Time %.1f: Car %d (%c %c)\t\t\t  exiting\n",
                   e.time, e.cid, e.dir_original, e.dir_target);
        }
    }

    return 0;
}