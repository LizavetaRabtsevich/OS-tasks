#pragma once
#include <windows.h>
#include <vector>

extern CRITICAL_SECTION cs;
extern int* arr;
extern int size;

extern HANDLE* threadHandles;
extern HANDLE* startEvents;
extern HANDLE* stopEvents;
extern HANDLE* exitEvents;

DWORD WINAPI marker(LPVOID param);

void InitArray(int arraySize);
void InitThreads(int threadCount);
void StartAllThreads(int threadCount);
void StopThread(int index);
void Cleanup(int threadCount);
