#include "MarkerApp.h"
#include <iostream>

int main() {
    int arraySize;
    std::cout << "Enter array size: ";
    std::cin >> arraySize;

    InitArray(arraySize);

    int threadCount;
    std::cout << "Enter thread count: ";
    std::cin >> threadCount;

    InitThreads(threadCount);
    StartAllThreads(threadCount);

    bool* finished = new bool[threadCount]();
    int completed = 0;

    while (completed < threadCount) {
        WaitForMultipleObjects(threadCount, stopEvents, TRUE, INFINITE);

        EnterCriticalSection(&cs);
        std::cout << "Array: ";
        for (int i = 0; i < size; i++) std::cout << arr[i] << " ";
        std::cout << "\n";
        LeaveCriticalSection(&cs);

        int stopIndex;
        std::cout << "Enter thread number to stop: ";
        std::cin >> stopIndex;
        stopIndex--;

        if (stopIndex < 0 || stopIndex >= threadCount || finished[stopIndex]) {
            std::cout << "Invalid thread number!\n";
            continue;
        }

        finished[stopIndex] = true;
        completed++;

        StopThread(stopIndex);

        EnterCriticalSection(&cs);
        std::cout << "Final array: ";
        for (int i = 0; i < size; i++) std::cout << arr[i] << " ";
        std::cout << "\n";
        LeaveCriticalSection(&cs);

        for (int i = 0; i < threadCount; i++) {
            if (!finished[i]) {
                ResetEvent(stopEvents[i]);
                SetEvent(startEvents[i]);
            }
        }
    }

    Cleanup(threadCount);
    delete[] finished;

    std::cout << "All threads finished. Program completed.\n";
    return 0;
}