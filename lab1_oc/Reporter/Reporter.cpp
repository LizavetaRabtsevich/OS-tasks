#include <iostream> 
#include <fstream>
#include <conio.h>
#include <vector>
#include <algorithm>

struct employee {
	int num;
	char name[10];
	double hours;
};

bool comparator(const employee e1, const employee e2) {
	return e1.num < e2.num;
}

int main(int args, char* argv[]) {
	setlocale(LC_ALL, "rus");
	setlocale(LC_NUMERIC, "c");
	std::ifstream  binaryFile(argv[1], std::ios::binary);
	std::ofstream reportFile(argv[2]);
	double payPerHour = atof(argv[3]);
	std::vector<employee> workerList;
	reportFile << "Report on " << argv[1] << std::endl;
	reportFile << "Номер \t Имя \t Часы \t Зарплата" << std::endl;
	employee worker;
	while (binaryFile.read((char*)&worker, sizeof(employee))) {
		workerList.push_back(worker);
	}

	sort(workerList.begin(), workerList.end(), comparator);

	for (int i = 0; i < workerList.size(); i++) {
		reportFile << workerList[i].num << "\t" << workerList[i].name << "\t" << workerList[i].hours << "\t" << workerList[i].hours * payPerHour << std::endl;
	}

	_cputs("\nReporter завершил свою работу.\nНажмите чтобы завершить...\n");
	_getch();

	reportFile.close();
}