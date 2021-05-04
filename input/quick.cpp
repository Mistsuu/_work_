#include <iostream>
#include <string.h>
#include <time.h>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void bubblesort(int* arr, int arrsize) {
    if (arrsize == 1)
        return;

    int notsorted = 1;
    while (notsorted) {
        notsorted = 0;
        for (int i = 0; i < arrsize-1; ++i) {
            if (arr[i] > arr[i+1]) {
                swap(&arr[i], &arr[i+1]);
                notsorted = 1;
            }
        }
    }
}

int* mergesort(int* arr, int arrsize) {
    // Create merge array
    int* p = (int*)malloc(arrsize*sizeof(int));

    // Break case
    if (arrsize == 1) {
        p[0] = arr[0];
        return p;
    }

    // Create 2 parts
    int  s1 = arrsize/2;
    int  s2 = arrsize-s1;
    int* p1 = mergesort(arr,             s1);
    int* p2 = mergesort(arr + arrsize/2, s2);
    
    // Merging...
    int i=0, j=0, k=0;
    while (i < s1 && j < s2) {
        if (p1[i] < p2[j]) {
            p[k++] = p1[i++];
        } else {
            p[k++] = p2[j++];
        }
    }

    while (i < s1) 
        p[k++] = p1[i++];
    while (j < s2)
        p[k++] = p2[j++];

    return p;
}

int partitionFunc(int* arr, int left, int right) {
    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j <= right - 1; ++j) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i+1], &arr[right]);

    return i+1;
}

void quicksort(int* arr, int left, int right) {
    if (left < right) {
        int  pivot     = arr[right];
        int  partition = partitionFunc(arr, left, right);
        quicksort(arr, left, partition-1);
        quicksort(arr, partition+1, right);
    }
}

void rand(vector<int>& arr, int arrsize) {
    for (int i = 0; i < arrsize; ++i) {
        arr.push_back(rand() % 5);
    }
}

void print(int* arr, int arrsize) {
    for (int i = 0; i < arrsize; ++i) {
        cout << arr[i] << endl;
    }
}

int main() {
    srand(time(0));

    vector<int> arr; //rand(arr, SIZE);
    // fstream fs;
    // fs.open("array_in.txt", fstream::out);
    // for (int i = 0; i < SIZE; ++i) {
    //     fs << arr[i] << endl;
    // }
    // fs.close();

    fstream fs;
    fs.open("array_in.txt", fstream::in);
    string line;
    while(getline(fs, line))  {
        if (line == "")
            continue;

        stringstream ss;
        int i;
        ss << line;
        ss >> i;
        arr.push_back(i);
    }

    int* arr2 = &arr[0];
    //bubblesort(arr2, arr.size()); print(arr2, arr.size());
    int* narr = mergesort(arr2, arr.size()); print(narr, arr.size());
    //quicksort(arr2, 0, arr.size()-1); print(arr2, arr.size());
}