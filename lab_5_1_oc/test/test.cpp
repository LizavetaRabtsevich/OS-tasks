#include "pch.h"
#include "C:\Users\redmi\source\repos\3_os_final\MarkerApp.h"
#include <thread>

static TEST(MarkerTests, ArrayInitialization) {
    InitArray(10);
    for (int i = 0; i < size; i++) {
        EXPECT_EQ(arr[i], 0);
    }
    delete[] arr;
    arr = nullptr;
}

TEST(MarkerTests, SingleThreadMarksArray) {
    InitArray(50);
    InitThreads(1);
    StartAllThreads(1);

    Sleep(100);
    EnterCriticalSection(&cs);
    int marked = 0;
    for (int i = 0; i < size; ++i) if (arr[i] != 0) marked++;
    LeaveCriticalSection(&cs);
    EXPECT_GT(marked, 0);

    StopThread(0);
    Cleanup(1);
}

TEST(MarkerTests, MultipleThreadsWork) {
    InitArray(100);
    InitThreads(3);
    StartAllThreads(3);
    Sleep(200);

    EnterCriticalSection(&cs);
    int nonZero = 0;
    for (int i = 0; i < size; ++i)
        if (arr[i] != 0) nonZero++;
    LeaveCriticalSection(&cs);
    EXPECT_GT(nonZero, 0);

    for (int i = 0; i < 3; i++) StopThread(i);
    Cleanup(3);
}

TEST(MarkerTests, CleanupResetsMemory) {
    InitArray(30);
    InitThreads(1);
    StartAllThreads(1);
    Sleep(100);
    StopThread(0);

    bool cleared = true;
    for (int i = 0; i < size; ++i)
        if (arr[i] != 0) cleared = false;

    EXPECT_TRUE(cleared);

    Cleanup(1);
}
