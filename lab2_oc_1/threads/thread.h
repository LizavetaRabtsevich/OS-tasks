#ifndef THREAD_H
#define THREAD_H

#include <windows.h>
#include "../core/array.h"

constexpr DWORD THREAD_SUCCESS = 0;
constexpr DWORD THREAD_ERROR_EMPTY_ARRAY = 1;

class MinMaxWrapper {
private:
    ThreadData* data;
public:
    explicit MinMaxWrapper(ThreadData* data) : data(data) {}
    DWORD execute();
};

class AverageWrapper {
private:
    ThreadData* data;
public:
    explicit AverageWrapper(ThreadData* data) : data(data) {}
    DWORD execute();
};

DWORD WINAPI min_max_thread(LPVOID lpParam);
DWORD WINAPI average_thread(LPVOID lpParam);

#endif