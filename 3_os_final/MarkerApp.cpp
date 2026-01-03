#include "MarkerApp.h"
#include <iostream>

CRITICAL_SECTION cs;
int* arr = nullptr;
int size = 0;

HANDLE* threadHandles = nullptr;
HANDLE* startEvents = nullptr;
HANDLE* stopEvents = nullptr;
HANDLE* exitEvents = nullptr;

DWORD WINAPI marker(LPVOID param) {
    int threadIndex = reinterpret_cast<std::intptr_t>(param);
    int markedCount = 0;
    std::vector<int> markedIndices;

    WaitForSingleObject(startEvents[threadIndex], INFINITE);
    srand(threadIndex + 1);

    while (true) {
        EnterCriticalSection(&cs);

        int randomNumber = rand() % size;

        if (arr[randomNumber] == 0) {
            Sleep(5);
            arr[randomNumber] = threadIndex + 1;
            markedIndices.push_back(randomNumber);
            markedCount++;
            Sleep(5);
            LeaveCriticalSection(&cs);
        }
        else {
            std::cout << "Thread: " << threadIndex << "\n";
            std::cout << "Number of marked elements: " << markedCount << "\n";
            std::cout << "Element index that cannot be marked: " << randomNumber << "\n";

            LeaveCriticalSection(&cs);

            SetEvent(stopEvents[threadIndex]);
            ResetEvent(startEvents[threadIndex]);

            HANDLE events[2] = { startEvents[threadIndex], exitEvents[threadIndex] };
            DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);

            if (result == WAIT_OBJECT_0 + 1) {
                EnterCriticalSection(&cs);
                for (int i : markedIndices) {
                    arr[i] = 0;
                }
                LeaveCriticalSection(&cs);
                return 0;
            }
            else {
                ResetEvent(stopEvents[threadIndex]);
                continue;
            }
        }
    }
}

void InitArray(int arraySize) {
    size = arraySize;
    arr = new int[size]();
}

void InitThreads(int threadCount) {
    InitializeCriticalSection(&cs);

    threadHandles = new HANDLE[threadCount];
    startEvents = new HANDLE[threadCount];
    stopEvents = new HANDLE[threadCount];
    exitEvents = new HANDLE[threadCount];

    for (int i = 0; i < threadCount; i++) {
        startEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        stopEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        exitEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        threadHandles[i] = CreateThread(NULL, 0, marker, reinterpret_cast<LPVOID>(static_cast<INT_PTR>(i)), 0, NULL);
    }
}

void StartAllThreads(int threadCount) {
    for (int i = 0; i < threadCount; i++) {
        SetEvent(startEvents[i]);
    }
}

void StopThread(int index) {
    SetEvent(exitEvents[index]);
    WaitForSingleObject(threadHandles[index], INFINITE);
}

void Cleanup(int threadCount) {
    for (int i = 0; i < threadCount; i++) {
        CloseHandle(threadHandles[i]);
        CloseHandle(startEvents[i]);
        CloseHandle(stopEvents[i]);
        CloseHandle(exitEvents[i]);
    }
    delete[] threadHandles;
    delete[] startEvents;
    delete[] stopEvents;
    delete[] exitEvents;

    if (arr) {
        delete[] arr;
        arr = nullptr;
    }

    DeleteCriticalSection(&cs);
}
