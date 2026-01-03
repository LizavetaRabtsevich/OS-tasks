#pragma once
#include <iostream>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <memory>
#include <thread>

extern int size;
extern int* arr;

extern std::mutex arrMutex;
extern std::mutex coutMutex;

extern std::condition_variable startCV;
extern std::mutex startMutex;
extern bool startSignal;

extern std::vector<std::unique_ptr<std::mutex>> threadMutexes;
extern std::vector<std::unique_ptr<std::condition_variable>> threadCVs;

extern std::vector<bool> threadSignals;
extern std::vector<bool> threadTerminate;
extern std::vector<bool> threadsFinished;

extern std::condition_variable allSignaledCV;
extern std::mutex allSignaledMutex;

void marker(int threadIndex);

void InitArray(int arraySize);
void InitThreads(int threadCount);
void StartAllThreads();
void StopThread(int index);
void Cleanup();




//#pragma once
//#include <windows.h>
//#include <vector>
//
//extern CRITICAL_SECTION cs;
//extern int* arr;
//extern int size;
//
//extern HANDLE* threadHandles;
//extern HANDLE* startEvents;
//extern HANDLE* stopEvents;
//extern HANDLE* exitEvents;
//
//DWORD WINAPI marker(LPVOID param);
//
//void InitArray(int arraySize);
//void InitThreads(int threadCount);
//void StartAllThreads(int threadCount);
//void StopThread(int index);
//void Cleanup(int threadCount);
////void RunMarkerApp();