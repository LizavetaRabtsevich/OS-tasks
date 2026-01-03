#include "lab2_oc.h"
#include <iostream>
#include <thread>

using namespace std;

pair<int, int> findMinMax(const vector<int>& arr) {
    int minVal = arr[0];
    int maxVal = arr[0];
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < minVal) minVal = arr[i];
        if (arr[i] > maxVal) maxVal = arr[i];
    }
    return { minVal, maxVal };
}

double calcAverage(const vector<int>& arr) {
    long long sum = 0;
    for (int x : arr) sum += x;
    return static_cast<double>(sum) / arr.size();
}

void replaceArr(vector<int>& arr, int minVal, int maxVal, double average) {
    for (int& x : arr) {
        if (x == minVal || x == maxVal) {
            x = static_cast<int>(average);
        }
    }
}

vector<int> inputArray() {
    int n;
    cout << "Enter size of array: ";
    cin >> n;

    vector<int> arr(n);
    cout << "Enter elements of the array: ";
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }
    return arr;
}

void printArray(const vector<int>& arr) {
    for (int x : arr) cout << x << " ";
    cout << endl;
}

DWORD WINAPI MinMaxThread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    vector<int>& arr = *(data->arr);

    int minVal = arr[0];
    int maxVal = arr[0];

    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < minVal) minVal = arr[i];
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
        if (arr[i] > maxVal) maxVal = arr[i];
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
    }

    data->minVal = minVal;
    data->maxVal = maxVal;

    cout << "Min element = " << minVal << endl;
    cout << "Max element = " << maxVal << endl;

    return 0;
}

DWORD WINAPI AverageThread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    vector<int>& arr = *(data->arr);

    long long sum = 0;
    for (size_t i = 0; i < arr.size(); i++) {
        sum += arr[i];
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }

    data->average = static_cast<double>(sum) / arr.size();

    cout << "Average = " << data->average << endl;

    return 0;
}