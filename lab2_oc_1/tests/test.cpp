#include "pch.h"
#include "..\utils\lab2_oc.h"
#include <thread>

using namespace std;

TEST(FindMinMaxTest, PositiveNumbers) {
    vector<int> arr = { 3, 7, 1, 9, 5 };
    auto result = findMinMax(arr);
    EXPECT_EQ(result.first, 1);
    EXPECT_EQ(result.second, 9);
}

TEST(FindMinMaxTest, NegativeNumbers) {
    vector<int> arr = { -3, -7, -1, -9, -5 };
    auto result = findMinMax(arr);
    EXPECT_EQ(result.first, -9);
    EXPECT_EQ(result.second, -1);
}

TEST(FindMinMaxTest, SameElements) {
    vector<int> arr = { 5, 5, 5, 5 };
    auto result = findMinMax(arr);
    EXPECT_EQ(result.first, 5);
    EXPECT_EQ(result.second, 5);
}

TEST(CalcAverageTest, Integers) {
    vector<int> arr = { 1, 2, 3, 4, 5 };
    EXPECT_DOUBLE_EQ(calcAverage(arr), 3.0);
}

TEST(CalcAverageTest, NegativeNumbers) {
    vector<int> arr = { -5, -10, -15 };
    EXPECT_DOUBLE_EQ(calcAverage(arr), -10.0);
}

TEST(CalcAverageTest, MixedNumbers) {
    vector<int> arr = { -3, 3 };
    EXPECT_DOUBLE_EQ(calcAverage(arr), 0.0);
}

TEST(CalcAverageTest, OneElement) {
    vector<int> arr = { 42 };
    EXPECT_DOUBLE_EQ(calcAverage(arr), 42.0);
}

TEST(ReplaceArrTest, ReplaceMinMax) {
    vector<int> arr = { 1, 5, 10 };
    replaceArr(arr, 1, 10, 7.5);
    EXPECT_EQ(arr[0], 7);
    EXPECT_EQ(arr[1], 5);
    EXPECT_EQ(arr[2], 7);
}

TEST(ReplaceArrTest, NoReplacement) {
    vector<int> arr = { 2, 3, 4 };
    replaceArr(arr, 10, 20, 5.0);
    EXPECT_EQ(arr[0], 2);
    EXPECT_EQ(arr[1], 3);
    EXPECT_EQ(arr[2], 4);
}

TEST(ReplaceArrTest, ReplaceOnlyMin) {
    vector<int> arr = { 1, 2, 3 };
    replaceArr(arr, 1, 99, 2.5);
    EXPECT_EQ(arr[0], 2);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(ReplaceArrTest, ReplaceOnlyMax) {
    vector<int> arr = { 1, 2, 3 };
    replaceArr(arr, -99, 3, 2.5);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 2);
}

TEST(ThreadTest, MinMaxThreadWorks) {
    vector<int> arr = { 4, 7, 2, 9 };
    ThreadData data;
    data.arr = &arr;

    MinMaxThread(&data);
    EXPECT_EQ(data.minVal, 2);
    EXPECT_EQ(data.maxVal, 9);
}

TEST(ThreadTest, AverageThreadWorks) {
    vector<int> arr = { 2, 4, 6, 8 };
    ThreadData data;
    data.arr = &arr;

    AverageThread(&data);
    EXPECT_DOUBLE_EQ(data.average, 5.0);
}
