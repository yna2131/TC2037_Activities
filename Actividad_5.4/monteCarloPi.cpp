// =============================================================================
//
// File: monteCarloPi.cpp
// Author: Yuna Chung
// Author: Olimpia Garcia
// Date: 2023.04.24
// Description: Calculate pi using Monte Carlo method
// To compile: g++ -std=c++2a monteCarloPi.cpp -o app
// Copyright (c) 2023
//
// =============================================================================


// No pthread cause it is not working on both computers :(

#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <atomic>
#include <cmath>

#define NUMSAMPLES 1000000
#define NUMTHREADS 6

std::atomic <int> totalNum(0); // 점들을 세는 변수

void monteCarloThread(int threadId){
    std::random_device rd; // 랜덤으로 점을 찍기 위해 사용
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<double> dis(-1.0, 1.0); // -1.0 ~ 1.0 사이의 실수를 랜덤으로 생성

    int count = 0;
    
    // 위에서 생성된 점들을 원의 내부에 있는지 판단
    for(int i = 0; i < NUMSAMPLES / NUMTHREADS; i++){
        double x = dis(gen);
        double y = dis(gen);

        if(sqrt(pow(x, 2) + pow(y, 2)) <= 1){
            count++;
        }
    }

    // 원의 내부에 있는 점들의 개수를 atomic 변수에 저장
    totalNum += count;
}

int main(int argc, char *argv[])
{
    // 시간 측정: 시작
    auto startTime = std::chrono::high_resolution_clock::now();

    // 쓰레드 생성
    std::thread arrThread[NUMTHREADS];

    for(int i = 0; i < NUMTHREADS; i++){
        arrThread[i] = std::thread(monteCarloThread, i);
    }

    for(int i = 0; i < NUMTHREADS; i++){
        arrThread[i].join();
    }

    // 원의 내부에 있는 점들의 개수를 이용하여 pi를 계산
    double pi = 4.0 * totalNum / NUMSAMPLES;

    // 시간 측정: 끝
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> calcTime = endTime - startTime;

    std::cout<< "Time = " << calcTime.count() << " seconds" << std::endl;

    std::cout<< "pi = " << pi << std::endl;

    return 0;
}