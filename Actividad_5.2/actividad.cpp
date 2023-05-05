// =============================================================================
//
// File Name: actividad.cpp
// Author: Yuna Chung
// Author: Olimpia Garcia
// Date: 2023.05.05
// Description: Programa que calcula la suma de los numeros primos de 0 a 5,000,000
// To compile: g++ actividad.cpp
// =============================================================================

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "utils.h"

using namespace std;

const int size = 5000000;
const int threads = 6;

typedef struct {
  int start, end;
} Block;

bool isPrimo(int x){
    if(x < 2){
        return false;
    }

    for(int i = 2; i <= sqrt(x); i++){
        if(x % i == 0){
            return false;
        }
    }

    return true;
}

void* primo_multihilo(void* param) {
  double *acum;
  Block *block;
  int i;

  block = (Block *) param;
  acum = new double;
  (*acum) = 0;

  for(i = block->start; i < block->end; i++){
    if(isPrimo(i) == true){
      (*acum) += i;
    }
  }

  return ((void**) acum);
}

int main(int argc, char* argv[]) {
  double correct = 838596693108;
  double ms, ms2;

  /*----------------------------------Secuencial--------------------------------------*/
  double count= 0;

  ms2 = 0;


  for(int i = 0; i<=size; i++){
    start_timer();
    if(isPrimo(i) == true){
        count += i;
    }
    ms2 += stop_timer();
  }

  if(count == correct){
    cout << "Resultado secuencial= " << endl;
  }else{
    cout << "Falla secuencial :(   " << count << "   !=   " << correct << endl;
  }

  cout << "Suma final: " << count << endl;

  cout << "tiempo promedio secuencial =  " << (ms2 / 1000) << "s" << endl << endl;

  /*----------------------------------Multi-hilo--------------------------------------*/
  int blocksize, i, j;
  double result, *acum;
  Block blocks[threads];
  pthread_t tids[threads];

  blocksize = size / threads;

  for(i = 0; i < threads; i++){
    blocks[i].start = i * blocksize;

    if(i != (threads - 1)){
      blocks[i].end = (i + 1) * blocksize;
    } else{
      blocks[i].end = size;
    }
  }

  ms = 0;

  for(j = 0; j < N; j++){
    start_timer();

    result = 0;

    for(i = 0; i < threads; i++){
      pthread_create(&tids[i], NULL, primo_multihilo, (void*) &blocks[i]);
    }

    for(i = 0; i < threads; i++){
      pthread_join(tids[i], (void**) &acum);
      result += (*acum);
      delete acum;
    }

    ms += stop_timer();
  }

  if(result == correct){
    cout << "Resultado multithread=    " << result << endl;
  }else{
    cout << "multithread falla :(   "<< result << "   !=   " << correct << endl;
  }

  cout << "tiempo promedio multithread =  " << (ms / 1000) << "s" << endl << endl;

  return 0;
}
