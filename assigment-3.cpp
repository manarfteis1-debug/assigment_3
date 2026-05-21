#include <iostream>
#include <fstream> 
#include <ctime>
using namespace std;

// التوابت 
const int N = 8; // تحدد عدد الملكات وحجم المصفوفة 
const int POP_SIZE = 56; // عدد الحالات في كل جيل  
const int MAX_GENERATIONS = 1000; //حددت اقصى حد للاجيال 
const int MUTATION_RATE = 20; // نسبة حدود الطفرة 

int randomSeed=7; //قيمة اولية للبذرة العشوائية 

// دالة تغير في قيمة random في كل مره 
int simpleRandom() {
    randomSeed = (randomSeed * 37 + 11) % 1000;
    return randomSeed;
}

// دالة ترجع رقم عشوائي يتراوح بين 0 و الرقم المدخل للدالة  
int randomNumber(int maxValue) {
    return simpleRandom() % maxValue;
}

// قيمة مطلقة 
int myAbs(int x) {
    if (x < 0) return -x;
    return x;
}

// دالة تحسب تعارضات (h)
int calculateConflicts(int state[]) {
    int conflicts = 0;

    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {

            if (state[i] == state[j])
                conflicts++;

            if (myAbs(state[i] - state[j]) == myAbs(i - j))
                conflicts++;
        }
    }

    return conflicts;
}

// نسخ المصفوفة الاصليه في مصفوفة جديدة للحفاظ على قيمتها 
void copyState(int source[], int destination[]) {
    for (int i = 0; i < N; i++) {
        destination[i] = source[i];
    }
}