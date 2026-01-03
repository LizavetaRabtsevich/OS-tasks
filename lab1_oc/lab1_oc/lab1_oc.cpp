#include <windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>

struct employee {
	int num;
	char name[10];
	double hours;
};

int main() {
	setlocale(LC_ALL, "rus");
	setlocale(LC_NUMERIC, "c");
	wchar_t commandLine[50];
	std::wstring binaryFileName;
	int numRecord;
	std::cout << "Введите имя бинарного файла и число записей: ";
	std::wcin >> binaryFileName >> numRecord;
	if (numRecord < 1) {
		std::cout << "Должна быть хотя бы одна запись!";
		return 0;
	}
	std::wstring s = L"Creator.exe " + binaryFileName + L" " + std::to_wstring(numRecord);
	lstrcpyW(commandLine, s.data());

	STARTUPINFO si;  
	PROCESS_INFORMATION piCreator;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	CreateProcess(NULL, commandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piCreator);

	WaitForSingleObject(piCreator.hProcess, INFINITE);
	CloseHandle(piCreator.hThread);
	CloseHandle(piCreator.hProcess);

	employee worker;
	std::ifstream binaryFile(binaryFileName, std::ios::binary);
	std::cout << "Creator создает бинарный файл с такой информацией:" << std::endl;
	for (int i = 0; i < numRecord; i++) {
		binaryFile.read((char*)&worker, sizeof(employee));
		std::cout << worker.num << "\t" << worker.name << "\t" << worker.hours << std::endl;
	}

	double payPerHour;
	std::wstring reportFileName;
	std::cout << "Введите имя report файла и оплату за час: ";
	std::wcin >> reportFileName >> payPerHour;
	if (payPerHour <= 0) {
		std::cout << "Оплата за час должна быть больше 0!";
		return 0;
	}
	reportFileName += L".txt";
	s = L"Reporter.exe " + binaryFileName + L" " + reportFileName + L" " + std::to_wstring(payPerHour);
	lstrcpyW(commandLine, s.data());

	PROCESS_INFORMATION piReporter;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	CreateProcess(NULL, commandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piReporter);
	WaitForSingleObject(piReporter.hProcess, INFINITE);
	CloseHandle(piReporter.hProcess);
	CloseHandle(piReporter.hThread);

	std::ifstream reportFile(reportFileName);
	std::cout << "Reporter моздает report файл с данной информацией:" << std::endl;
	std::string info;
	while (getline(reportFile, info)) {
		std::cout << info << std::endl;
	}
	reportFile.close();

	_cputs("\nПрограмма завершена.\nНажмите чтобы завершить...");
	_getch();

	return 0;
}