#pragma once
#include <windows.h>
#include <vector>
#include <utility>

struct ThreadData {
    std::vector<int>* arr;
    int minVal;
    int maxVal;
    double average;
};

std::pair<int, int> findMinMax(const std::vector<int>& arr);
double calcAverage(const std::vector<int>& arr);
void replaceArr(std::vector<int>& arr, int minVal, int maxVal, double average);

std::vector<int> inputArray();
void printArray(const std::vector<int>& arr);

DWORD WINAPI MinMaxThread(LPVOID lpParam);
DWORD WINAPI AverageThread(LPVOID lpParam);
