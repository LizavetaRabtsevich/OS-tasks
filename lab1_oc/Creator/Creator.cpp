#include <iostream>
#include <fstream>
#include <conio.h>

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    setlocale(LC_NUMERIC, "c");
    if (argc != 3) {
        std::cout << "Ошибка!";
        return 0;
    }
    int numRecord = atoi(argv[2]);
    if (numRecord < 1) {
        std::cout << "Должна быть хотя бы одна запись!";
        return 0;
    }
    employee* workerList = new employee[numRecord];

    std::fstream binaryFile(argv[1], std::ios::binary | std::ios::out);
    std::cout << "Введите список работников\n";
    std::cout << "#  Номер  Имя  Часы\n";
    for (int i = 0; i < numRecord; i++) {
        std::cout << i + 1 << ". ";
        std::cin >> workerList[i].num >> workerList[i].name >> workerList[i].hours;
    }

    for (int i = 0; i < numRecord; i++) {
        binaryFile.write((char*)&workerList[i], sizeof(employee));
    }
    _cputs("\nCreator завершил свою работу.\nНажмите чтобы завершить...");
    _getch(); 

    binaryFile.close();

    return 0;
}