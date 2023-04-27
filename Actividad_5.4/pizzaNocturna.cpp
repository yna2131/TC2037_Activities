// =====================================================================================
// 
// File Name: pizzaNocturna.cpp
// Author: Yuna Chung
// Author: Olimpia Garcia
// Date: 2023.04.26
// Description: Programa que simula un pedido a una pizzeria con varios estudiantes 
// que estudian y comen pizza
// To compile: g++ pizzaNocturna.cpp
// Copyright (c) 2023
// 
// =====================================================================================

// Finally working pthreads on my computer :D

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <chrono>

#define THREADS 8
#define STUDENTS 7
#define PIZZA_SLICES 8

// Global variables
pthread_mutex_t pizza_mutex = PTHREAD_MUTEX_INITIALIZER;  // mutex for pizza slices
pthread_mutex_t order_mutex = PTHREAD_MUTEX_INITIALIZER;  // mutex for pizza order
pthread_cond_t pizza_order = PTHREAD_COND_INITIALIZER;    // condition variable for pizza order
pthread_cond_t pizza_delivery = PTHREAD_COND_INITIALIZER; // condition variable for pizza delivery
int pizza_slices = PIZZA_SLICES;
bool pizza_ordered = false;


// Function for students

void *studentAction(void *param)
{
    int id = *((int *)param);

    while (true)
    {
        pthread_mutex_lock(&pizza_mutex);

        while (pizza_slices == 0 && !pizza_ordered)
        {
            pthread_mutex_lock(&order_mutex);

            pizza_ordered = true;

            sleep(1);
            std::cout << "Student " << id << " is calling the Pizza Store" << std::endl;
            sleep(1);

            pthread_cond_signal(&pizza_order);
            pthread_cond_wait(&pizza_delivery, &pizza_mutex);

            std::cout << "Student " << id << " receives the pizza" << std::endl;
            sleep(1);

            pthread_mutex_unlock(&order_mutex);
        }

        if (!pizza_ordered)
        {
            std::cout << "Student " << id << " grabbed a pizza slice. Slices left: " << pizza_slices << std::endl;
            pizza_slices--;
            sleep(1);
        }

        pthread_mutex_unlock(&pizza_mutex);

        sleep(2);
    }

    pthread_exit(NULL);
}

// Function for pizza store

void *pizzaCall(void *param)
{

    while (true)
    {
        pthread_mutex_lock(&pizza_mutex);

        while (pizza_slices > 0)
        {
            pthread_cond_wait(&pizza_order, &pizza_mutex);
        }

        while (pizza_ordered)
        {
            pizza_slices = PIZZA_SLICES; 
            std::cout << "Pizza!" << std::endl;
            sleep(1);

            pthread_cond_broadcast(&pizza_delivery); 
            pizza_ordered = false;                   
            pthread_mutex_unlock(&pizza_mutex);      
        }

        sleep(2);
    }

    pthread_exit(NULL);
}

// main function

int main(int argc, char *argv[])
{

    pthread_t pizzeria_thread;            
    pthread_t students_threads[STUDENTS]; 
    int id_students[STUDENTS];

    for (int i = 0; i < STUDENTS; i++)
    {
        id_students[i] = i + 1;                                                             
        pthread_create(&students_threads[i], NULL, studentAction, (void *)&id_students[i]); 
    }
    pthread_create(&pizzeria_thread, NULL, pizzaCall, NULL); 

    for (int i = 0; i < STUDENTS; i++) 
    {
        pthread_join(students_threads[i], NULL);
    }

    pthread_join(pizzeria_thread, NULL); 

    return 0;
}