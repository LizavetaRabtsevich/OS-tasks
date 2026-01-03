#include <iostream>
#include <windows.h>
using namespace std;

struct ThreadData {
    int* array;
    int size;
    int* min;
    int* max;
    double* average;
};

DWORD WINAPI min_max(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    int* arr = data->array;
    int size = data->size;

    if (size <= 0) {
        return 0;
    }

    int min = arr[0];
    int max = arr[0];

    for (int i = 1; i < size; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
        if (arr[i] > max) {
            max = arr[i];
        }
        Sleep(7);
    }

    *(data->min) = min;
    *(data->max) = max;

    cout << "Minimal element: " << min << endl;
    cout << "Maximal element: " << max << endl;

    return 0;
}

DWORD WINAPI average(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    int* arr = data->array;
    int size = data->size;

    if (size <= 0) {
        *(data->average) = 0;
        return 0;
    }

    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
        Sleep(12);
    }

    double avg = sum / size;
    *(data->average) = avg;

    cout << "Arithmetic mean: " << avg << endl;

    return 0;
}

int main() {

    int size;

    cout << "Enter arrays' size: ";
    cin >> size;

    if (size <= 0) {
        cout << "Arrays' size must be a positiv number" << endl;
        return 1;
    }

    int* array = new int[size];

    cout << "Enter " << size << " arrays' elements:\n" << endl;
    for (int i = 0; i < size; i++) {
        cout << "Element " << i + 1 << ": ";
        cin >> array[i];
    }

    cout << "\nOriginal array: ";
    for (int i = 0; i < size; i++) {
        cout << array[i] << " ";
    }
    cout << endl;

    int min, max;
    double avg;

    ThreadData threadData;
    threadData.array = array;
    threadData.size = size;
    threadData.min = &min;
    threadData.max = &max;
    threadData.average = &avg;

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, &threadData, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, &threadData, 0, NULL);

    if (hMinMax == NULL || hAverage == NULL) {
        cout << "Error of threads creaton" << endl;
        delete[] array;
        return 1;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);


    for (int i = 0; i < size; i++) {
        if (array[i] == min || array[i] == max) {
            array[i] = (int)avg;
        }
    }

    cout << "Array after replacement min and max with arithmetic mean: ";
    for (int i = 0; i < size; i++) {
        cout << array[i] << " ";
    }
    cout << endl;

    delete[] array;

    return 0;
}