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

// تقوم الدالة بدمج صفات الاب الاول مع التاني لانتاج ابن جديد  
void crossover(int parent1[], int parent2[], int child[]) {
    for (int i = 0; i < 4; i++) {
        child[i] = parent1[i];
    }

    for (int i = 4; i < N; i++) {
        child[i] = parent2[i];
    }
}

 // داله تاخد الابن الجديد وتقوم بعمل طفره بناء على نسبة احتمال
void mutation(int state[]) {
    int randomValue = randomNumber(100);

    if (randomValue < MUTATION_RATE) {
        int randomColumn = randomNumber(N);
        int randomRow = randomNumber(N);

        state[randomColumn] = randomRow;
    }
}

// تقوم الدالة بطباعة بيانات الجيل 
void printGeneration(
    int generation,
    int population[POP_SIZE][N],
    ostream &out
) {
    out << "\n====================================\n";
    out << "Generation Number: " << generation << endl;
    out << "====================================\n";

    for (int i = 0; i < POP_SIZE; i++) {
        out << "\nIndividual Number: " << i + 1 << endl;
        out << "h = " << calculateConflicts(population[i]) << endl;
        printStateLine(population[i], out);
        printBoard(population[i], out);
    }
}

// تقوم الدالة بتنفيد الخوارزمية الجينية بالكامل لايجاد الحل وتحفظ النتائج في ملف
void runGeneticAlgorithm(
    int firstPopulation[POP_SIZE][N],
    int selectionType,
    const char fileName[]
) {
    ofstream file(fileName);

    int population[POP_SIZE][N];
    int newPopulation[POP_SIZE][N];

    for (int i = 0; i < POP_SIZE; i++) {
        for (int j = 0; j < N; j++) {
            population[i][j] = firstPopulation[i][j];
        }
    }

    clock_t startTime = clock();

    if (selectionType == 1) {
        cout << "\nRunning Roulette Wheel Selection...\n";
        file << "Genetic Algorithm Using Roulette Wheel Selection\n";
    } else {
        cout << "\nRunning Tournament Selection...\n";
        file << "Genetic Algorithm Using Tournament Selection\n";
    }

    for (int generation = 1; generation <= MAX_GENERATIONS; generation++) {

        printGeneration(generation, population, file);

        int bestIndex = 0;
        int bestH = calculateConflicts(population[0]);

        for (int i = 1; i < POP_SIZE; i++) {
            int h = calculateConflicts(population[i]);

            if (h < bestH) {
                bestH = h;
                bestIndex = i;
            }
        }

        cout << "Generation " << generation
             << " | Best h = " << bestH << endl;

        file << "\nBest State In This Generation:\n";
        file << "Best h = " << bestH << endl;
        printStateLine(population[bestIndex], file);
        printBoard(population[bestIndex], file);

        if (bestH == 0) {
            clock_t endTime = clock();
            double totalTime =
                double(endTime - startTime) / CLOCKS_PER_SEC;

            cout << "\nGoal Found!\n";
            cout << "Time = " << totalTime << " seconds\n";
            cout << "Results saved in: " << fileName << endl;

            file << "\n====================================\n";
            file << "GOAL FOUND\n";
            file << "Generation = " << generation << endl;
            file << "Time = " << totalTime << " seconds\n";
            file << "Final Solution:\n";
            printStateLine(population[bestIndex], file);
            printBoard(population[bestIndex], file);

            file.close();
            return;
        }

        for (int i = 0; i < POP_SIZE; i++) {
            int parent1[N];
            int parent2[N];
            int child[N];

            if (selectionType == 1) {
                rouletteWheelSelection(population, parent1);
                rouletteWheelSelection(population, parent2);
            } else {
                tournamentSelection(population, parent1);
                tournamentSelection(population, parent2);
            }

            crossover(parent1, parent2, child);
            mutation(child);

            copyState(child, newPopulation[i]);
        }

        for (int i = 0; i < POP_SIZE; i++) {
            for (int j = 0; j < N; j++) {
                population[i][j] = newPopulation[i][j];
            }
        }
    }

    clock_t endTime = clock();
    double totalTime = double(endTime - startTime) / CLOCKS_PER_SEC;

    cout << "\nGoal Not Found.\n";
    cout << "Time = " << totalTime << " seconds\n";
    cout << "Results saved in: " << fileName << endl;

    file << "\nGOAL NOT FOUND\n";
    file << "Time = " << totalTime << " seconds\n";

    file.close();
}

int main() {
    int initialState[N];
    int population[POP_SIZE][N];

    cout << "8-Queens Problem Using Genetic Algorithm\n";
    cout << "Enter queen positions from LEFT to RIGHT.\n";
    cout << "Each number is the row of the queen in that column.\n";
    cout << "Rows must be from 1 to 8.\n\n";

    cout << "Enter 8 numbers: ";

    for (int i = 0; i < N; i++) {
        cin >> initialState[i];

        initialState[i]--;

        if (initialState[i] < 0 || initialState[i] >= N) {
            cout << "Invalid input. Rows must be from 1 to 8.\n";
            return 0;
        }
    }

    cout << "\nInitial State h = "
         << calculateConflicts(initialState) << endl;

    generateSuccessors(initialState, population);

    cout << "Number of generated successor states = 56\n";

    runGeneticAlgorithm(
        population,
        1,
        "roulette_results.txt"
    );

    runGeneticAlgorithm(
        population,
        2,
        "tournament_results.txt"
    );

    cout << "\nFinished.\n";
    cout << "Check these files:\n";
    cout << "roulette_results.txt\n";
    cout << "tournament_results.txt\n";

    return 0;
}