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

// تقوم بطباعة ارقام صفوف الملكات في سطر واحد 
void printStateLine(int state[], ostream &out) {
    out << "State: ";
    for (int i = 0; i < N; i++) {
        out << state[i] + 1 << " ";
    }
    out << endl;
}

// دالة لطباعة شكل المصفوفة كرسمة في ملف واماكن الملكات
void printBoard(int state[], ostream &out) {
    out << "\nChess Board:\n";

    for (int row = 0; row < N; row++) {
        out << "+---+---+---+---+---+---+---+---+\n";

        for (int col = 0; col < N; col++) {
            if (state[col] == row)
                out << "| Q ";
            else
                out << "|   ";
        }

        out << "|\n";
    }

    out << "+---+---+---+---+---+---+---+---+\n";
}

// دالة تقوم بتوليد حالات جديدة  56 حالة جديدة من خلال تحريك ملكه في كل مره 
void generateSuccessors(int initialState[], int population[POP_SIZE][N]) {
    int index = 0;

    for (int col = 0; col < N; col++) {
        for (int row = 0; row < N; row++) {

            if (row != initialState[col]) {
                for (int i = 0; i < N; i++) {
                    population[index][i] = initialState[i];
                }

                population[index][col] = row;
                index++;
            }
        }
    }
}

// دالة لاختيار الآباء تعتمد على إعطاء فرصة أكبر للحلول التي تحتوي على تعارض أقل
void rouletteWheelSelection(int population[POP_SIZE][N], int selectedParent[]) {
    int fitness[POP_SIZE];
    int totalFitness = 0;

    for (int i = 0; i < POP_SIZE; i++) {
        int h = calculateConflicts(population[i]);

        fitness[i] = 100 / (1 + h);
        totalFitness += fitness[i];
    }

    int randomValue = randomNumber(totalFitness);
    int sum = 0;

    for (int i = 0; i < POP_SIZE; i++) {
        sum += fitness[i];

        if (sum >= randomValue) {
            copyState(population[i], selectedParent);
            return;
        }
    }

    copyState(population[0], selectedParent);
}

// دالة تختار عدد من الافراد عشوائيا وتاخد الأقل تعارض بينهم ليكون هو الاب
void tournamentSelection(int population[POP_SIZE][N], int selectedParent[]) {
    int tournamentSize = 5;

    int bestIndex = randomNumber(POP_SIZE);
    int bestH = calculateConflicts(population[bestIndex]);

    for (int i = 1; i < tournamentSize; i++) {
        int randomIndex = randomNumber(POP_SIZE);
        int currentH = calculateConflicts(population[randomIndex]);

        if (currentH < bestH) {
            bestH = currentH;
            bestIndex = randomIndex;
        }
    }

    copyState(population[bestIndex], selectedParent);
}