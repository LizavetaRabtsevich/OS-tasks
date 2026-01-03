#ifndef ARRAY_H
#define ARRAY_H

constexpr int MIN_MAX_SLEEP_MS = 7;
constexpr int AVERAGE_SLEEP_MS = 12;

struct ThreadData {
    int* array;
    int size;
    int* min;
    int* max;
    double* average;
};

namespace ArrayProcessor {
    void findMinMax(int* arr, int size, int* minVal, int* maxVal);
    double calculateAverage(int* arr, int size);
    void replaceMinMaxWithAverage(int* arr, int size, int minVal, int maxVal, double avg);
}

#endif