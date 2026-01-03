//#include "gtest/gtest.h"
#include <vector>
#include <iostream>
#include <thread>
#include "..\utils\lab2_oc.h"
using namespace std;
int main(int argc, char** argv) {
        std::vector<int> arr = inputArray();
        ThreadData data;
        data.arr = &arr;

        thread helper1(MinMaxThread, &data);
        thread helper2(AverageThread, &data);
    
        helper1.join();
        helper2.join();
        
        replaceArr(arr, data.minVal, data.maxVal, data.average);
    
        cout << "Array after replace min/max with average: ";
        printArray(arr);
    
        return 0;
}


//mutex
//не 



/*HANDLE hMinMax = CreateThread(NULL, 0, MinMaxThread, &data, 0, NULL);
        HANDLE hAverage = CreateThread(NULL, 0, AverageThread, &data, 0, NULL);*/


        /*WaitForSingleObject(hAverage, INFINITE);
               CloseHandle(hMinMax);
               CloseHandle(hAverage);*/
