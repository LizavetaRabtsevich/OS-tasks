#include "thread.h"
#include <iostream>

using namespace std;

DWORD MinMaxWrapper::execute() {
    if (data->size <= 0) {
        return THREAD_ERROR_EMPTY_ARRAY;
    }
    
    ArrayProcessor::findMinMax(data->array, data->size, data->min, data->max);
    cout << "Minimal element: " << *(data->min) << endl;
    cout << "Maximal element: " << *(data->max) << endl;
    
    return THREAD_SUCCESS;
}

DWORD AverageWrapper::execute() {
    if (data->size <= 0) {
        *(data->average) = 0;
        return THREAD_ERROR_EMPTY_ARRAY;
    }
    
    double avg = ArrayProcessor::calculateAverage(data->array, data->size);
    *(data->average) = avg;
    cout << "Arithmetic mean: " << avg << endl;
    
    return THREAD_SUCCESS;
}

DWORD WINAPI min_max_thread(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    MinMaxWrapper wrapper(data);
    return wrapper.execute();
}

DWORD WINAPI average_thread(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    AverageWrapper wrapper(data);
    return wrapper.execute();
}