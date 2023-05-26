// =============================================================================
//
// File Name: cruzandoUnPuente.cpp
// Author: Yuna Chung
// Date: 2023.04.27
// Description: Programa que simula el cruce de un puente por varios vehiculos
// To compile: g++ cruzandounpuente.cpp
// =============================================================================

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

#define NUMTHREADS 6 // Number of threads
#define MAXVEHICLES 3 // Maximum number of vehicles on the bridge

// Global variables
sem_t PosAvailable; // Spots available on the bridge
bool direction = true; // true = North, false = South

pthread_mutex_t mutex; // Mutex
pthread_cond_t ChangeDirection; // Condition variable


// Vehicle structure to contain vehicle information
struct Vehicle
{
    int id;
    bool direction;
};

// Function when vehicle arrives at the bridge
void arrive2Bridge(Vehicle vehicle)
{
    cout << "Vehicle " << vehicle.id << " arrives at the bridge through " << vehicle.direction << endl;
}

// Function when vehicle is crossing the bridge
void crossBridge(Vehicle vehicle)
{
    pthread_mutex_lock(&mutex); // Lock mutex

    sem_wait(&PosAvailable); // Wait for a spot to be available on the bridge

    int VehiclesOnBridge = 0;
    sem_getvalue(&PosAvailable, &VehiclesOnBridge); // Get the number of vehicles on the bridge

    cout << "Vehicle " << vehicle.id << " is crossing the bridge through " << vehicle.direction << endl;

    usleep(1000000); // Sleep for 1 second

    if (VehiclesOnBridge >= MAXVEHICLES) { // If the bridge is full
        direction = !direction; // Switch direction
        pthread_cond_broadcast(&ChangeDirection); // Signal all threads to wake up
    }

    sem_post(&PosAvailable); // Signal that a spot is available on the bridge

    pthread_mutex_unlock(&mutex); // Unlock mutex
}


// Function when vehicle leaves the bridge
void exitBridge(Vehicle vehicle)
{
    pthread_mutex_lock(&mutex); // Lock mutex

    sem_post(&PosAvailable); // Signal that a spot is available on the bridge

    int VehicleOnBridge = 0;

    sem_getvalue(&PosAvailable, &VehicleOnBridge); // Get the number of vehicles on the bridge

    cout << "Vehicle " << vehicle.id << " has crossed the bridge through " << vehicle.direction << endl;

    pthread_mutex_unlock(&mutex); // Unlock mutex
}

// Function for the vehicle thread
void* vehicle(void* arg)
{
    Vehicle vehicle = *(Vehicle*)arg; // Get vehicle information

    arrive2Bridge(vehicle); // Vehicle arrives at the bridge
    pthread_mutex_lock(&mutex); /// Lock mutex

    if(direction != vehicle.direction) // If the vehicle is going the opposite direction of the bridge
    {
        direction = vehicle.direction; // Switch direction
        pthread_cond_wait(&ChangeDirection, &mutex); // Wait for the signal to change direction
        cout << "Vehicle changes direction!" << endl;
    }

    pthread_mutex_unlock(&mutex); // Unlock mutex

    crossBridge(vehicle); // Vehicle crosses the bridge

    exitBridge(vehicle); // Vehicle exits the bridge

    pthread_exit(NULL); // Exit thread
    return NULL;
}

// Main function
int main()
{
    pthread_t threads[NUMTHREADS]; // Array of threads
    Vehicle vehicles[NUMTHREADS]; // Array of vehicles

    sem_init(&PosAvailable, 0, MAXVEHICLES); // Initialize semaphore

    pthread_mutex_init(&mutex, NULL); // Initialize mutex
    pthread_cond_init(&ChangeDirection, NULL); // Initialize condition variable

    for (int i = 0; i < NUMTHREADS; i++) // Create threads
    {
        vehicles[i].id = i; // Set vehicle id
        vehicles[i].direction = direction; // Set vehicle direction
        pthread_create(&threads[i], NULL, vehicle, (void*)&vehicles[i]); // Create thread
        usleep(1000000); // Sleep for 1 second
    }

    for (int i = 0; i < NUMTHREADS; i++)
    {
        pthread_join(threads[i], NULL); // Join thread
    }

    pthread_mutex_destroy(&mutex); // Destroy mutex
    pthread_cond_destroy(&ChangeDirection); // Destroy condition variable

    return 0;
}