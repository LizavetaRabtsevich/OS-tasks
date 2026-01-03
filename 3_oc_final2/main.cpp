#include <iostream>
#include <mutex>
#include <vector>
#include "MarkerApp.h"

//int size = 0;
//int* arr;
//
//std::mutex arrMutex;
//std::mutex coutMutex;
//
//std::condition_variable startCV;
//std::mutex startMutex;
//bool startSignal = false;
//
//std::vector<std::unique_ptr<std::mutex>> threadMutexes;
//std::vector<std::unique_ptr<std::condition_variable>> threadCVs;
//
//std::vector<bool> threadSignals;
//std::vector<bool> threadTerminate;
//std::vector<bool> threadsFinished;
//
//std::condition_variable allSignaledCV;
//std::mutex allSignaledMutex;
//
//
//void marker(int threadIndex) {
//    int markedCount = 0;
//    std::vector<int> markedIndices;
//
//    {
//        std::unique_lock<std::mutex> lock(startMutex);
//        startCV.wait(lock, [] { return startSignal; });
//    }
//
//    srand(threadIndex + 1);
//
//    while (true) {
//        int randomNumber = rand() % size;
//
//        {
//            std::lock_guard<std::mutex> lock(arrMutex);
//
//            if (arr[randomNumber] == 0) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(5));
//                arr[randomNumber] = threadIndex + 1;
//                markedIndices.push_back(randomNumber);
//                markedCount++;
//                std::this_thread::sleep_for(std::chrono::milliseconds(5));
//                continue;
//            }
//            else {
//                std::cout << "Thread: " << threadIndex << "\n";
//                std::cout << "Number of marked elements: " << markedCount << "\n";
//                std::cout << "Element index that cannot be marked: " << randomNumber << "\n";
//            }
//        }
//
//        {
//            std::lock_guard<std::mutex> lock(*threadMutexes[threadIndex]);
//            threadSignals[threadIndex] = true;
//        }
//        allSignaledCV.notify_one();
//
//        {
//            std::unique_lock<std::mutex> lock(*threadMutexes[threadIndex]);
//            threadCVs[threadIndex]->wait(lock);
//        }
//
//        if (threadTerminate[threadIndex]) {
//            std::lock_guard<std::mutex> lock(arrMutex);
//            std::lock_guard<std::mutex> lock2(allSignaledMutex);
//
//            for (int i : markedIndices) {
//                if (i >= 0 && i < size) {
//                    arr[i] = 0;
//                }
//            }
//            threadsFinished[threadIndex] = true;
//            return;
//        }
//        else {
//            threadSignals[threadIndex] = false;
//        }
//    }
//}

int main() {
    std::cout << "Enter size of array: ";
    std::cin >> size;
    arr = new int[size]();

    int countOfThreads = 0;
    std::cout << "Enter count of marker call: ";
    std::cin >> countOfThreads;

    threadSignals.resize(countOfThreads, false);
    threadTerminate.resize(countOfThreads, false);
    threadsFinished.resize(countOfThreads, false);

    threadCVs.resize(countOfThreads);
    threadMutexes.resize(countOfThreads);
    for (int i = 0; i < countOfThreads; i++) {
        threadCVs[i] = std::make_unique<std::condition_variable>();
        threadMutexes[i] = std::make_unique<std::mutex>();
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < countOfThreads; i++) {
        threads.emplace_back(marker, i);
    }

    {
        std::lock_guard<std::mutex> lock(startMutex);
        startSignal = true;
    }
    startCV.notify_all();

    while (true) {

        bool allSignaled = false;
        while (!allSignaled) {
            allSignaled = true;
            for (int i = 0; i < countOfThreads; i++) {
                if (!threadsFinished[i] && !threadSignals[i]) {
                    allSignaled = false;
                    break;
                }
            }
            if (!allSignaled) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }

        std::cout << "Array content: ";
        for (int i = 0; i < size; i++) {
            std::cout << arr[i] << " ";
        }
        std::cout << "\n";

        int stopIndex;
        std::cout << "Enter the serial number of the thread to stop its work: ";
        std::cin >> stopIndex;

        if (stopIndex < 1 || stopIndex > countOfThreads || threadsFinished[stopIndex - 1]) {
            std::cout << "Invalid thread number!\n";
            continue;
        }

        int stopActualIndex = stopIndex - 1;

        {
            std::lock_guard<std::mutex> lock(*threadMutexes[stopActualIndex]);
            threadTerminate[stopActualIndex] = true;
        }
        threadCVs[stopActualIndex]->notify_one();

        threads[stopActualIndex].join();
        threadsFinished[stopActualIndex] = true;

        std::cout << "Final array content: ";
        for (int i = 0; i < size; i++) {
            std::cout << arr[i] << " ";
        }
        std::cout << "\n";

        bool allDone = true;
        for (int i = 0; i < countOfThreads; i++) {
            if (!threadsFinished[i]) {
                allDone = false;
                break;
            }
        }

        if (allDone) {
            break;
        }

        for (int i = 0; i < countOfThreads; i++) {
            if (!threadsFinished[i]) {
                std::lock_guard<std::mutex> lock(*threadMutexes[i]);
                threadSignals[i] = false;
                threadCVs[i]->notify_one();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    delete[] arr;
    std::cout << "All threads finished. Program completed." << std::endl;
    return 0;
}