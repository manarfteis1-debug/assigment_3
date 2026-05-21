#include <iostream>
#include <fstream> 
#include <ctime>
using namespace std;

// التوابت 
const int N = 8; // تحدد عدد الملكات وحجم المصفوفة 
const int POP_SIZE = 56; // عدد الحالات في كل جيل  
const int MAX_GENERATIONS = 1000; //حددت اقصى حد للاجيال 
const int MUTATION_RATE = 20; // نسبة حدود الطفرة 
