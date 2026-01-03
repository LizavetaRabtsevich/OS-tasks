#include "MarkerApp.h"

int size = 0;
int* arr = nullptr;

std::mutex arrMutex;
std::mutex coutMutex;

std::condition_variable startCV;
std::mutex startMutex;
bool startSignal = false;

std::vector<std::unique_ptr<std::mutex>> threadMutexes;
std::vector<std::unique_ptr<std::condition_variable>> threadCVs;

std::vector<bool> threadSignals;
std::vector<bool> threadTerminate;
std::vector<bool> threadsFinished;

std::condition_variable allSignaledCV;
std::mutex allSignaledMutex;

void marker(int threadIndex) {
    int markedCount = 0;
    std::vector<int> markedIndices;

    {
        std::unique_lock<std::mutex> lock(startMutex);
        startCV.wait(lock, [] { return startSignal; });
    }

    srand(threadIndex + 1);

    while (true) {
        int randomNumber = rand() % size;

        {
            std::lock_guard<std::mutex> lock(arrMutex);

            if (arr[randomNumber] == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                arr[randomNumber] = threadIndex + 1;
                markedIndices.push_back(randomNumber);
                markedCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }
            else {
                std::lock_guard<std::mutex> lockCout(coutMutex);
                std::cout << "Thread: " << threadIndex << "\n";
                std::cout << "Number of marked elements: " << markedCount << "\n";
                std::cout << "Element index that cannot be marked: " << randomNumber << "\n";
            }
        }

        {
            std::lock_guard<std::mutex> lock(*threadMutexes[threadIndex]);
            threadSignals[threadIndex] = true;
        }
        allSignaledCV.notify_one();

        {
            std::unique_lock<std::mutex> lock(*threadMutexes[threadIndex]);
            threadCVs[threadIndex]->wait(lock);
        }

        if (threadTerminate[threadIndex]) {
            std::lock_guard<std::mutex> lock(arrMutex);
            std::lock_guard<std::mutex> lock2(allSignaledMutex);

            for (int i : markedIndices) {
                if (i >= 0 && i < size) {
                    arr[i] = 0;
                }
            }
            threadsFinished[threadIndex] = true;
            return;
        }
        else {
            threadSignals[threadIndex] = false;
        }
    }
}

void InitArray(int arraySize) {
    size = arraySize;
    arr = new int[size]();
}

void InitThreads(int threadCount) {
    threadSignals.resize(threadCount, false);
    threadTerminate.resize(threadCount, false);
    threadsFinished.resize(threadCount, false);

    threadCVs.resize(threadCount);
    threadMutexes.resize(threadCount);
    for (int i = 0; i < threadCount; i++) {
        threadCVs[i] = std::make_unique<std::condition_variable>();
        threadMutexes[i] = std::make_unique<std::mutex>();
    }
}

void StartAllThreads() {
    {
        std::lock_guard<std::mutex> lock(startMutex);
        startSignal = true;
    }
    startCV.notify_all();
}

void StopThread(int index) {
    {
        std::lock_guard<std::mutex> lock(*threadMutexes[index]);
        threadTerminate[index] = true;
    }
    threadCVs[index]->notify_one();
}

void Cleanup() {
    if (arr) {
        delete[] arr;
        arr = nullptr;
    }
    size = 0;
    threadSignals.clear();
    threadTerminate.clear();
    threadsFinished.clear();
    threadCVs.clear();
    threadMutexes.clear();
    startSignal = false;
}
