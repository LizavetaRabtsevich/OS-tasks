#include <iostream>
#include <iomanip>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>
#include <map>
struct Employee
{
    int num;
    char name[10];
    double hours;
};
const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\employee_pipe";
int employeesCount;
int clientsCount;
std::string fileName;
std::map<int, HANDLE> semaphores;
std::vector<Employee> employees;

HANDLE mutex;

void createFile() {
    std::ofstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "The file could not be opened." << std::endl;
        return;
    }

    employeesCount = 0;
    std::cout << "Enter count of emploees: ";
    std::cin >> employeesCount;
    while (employeesCount <= 0) {
        std::cout << "Enter a number greater than 0.\n";
        std::cout << "Enter count of emploees: ";
        std::cin.clear();
        std::cin.ignore(INT_MAX, '\n');
        std::cin >> employeesCount;
    }

    for (int i = 0; i < employeesCount; ++i) {
        Employee employee;
        std::cout << "Enter information about employee number " << (i + 1) << ":\n";
        std::cout << "Enter identification number: ";
        std::cin >> employee.num;
        while (employee.num <= 0) {
            std::cout << "Enter a number greater than 0.\n";
            std::cout << "Enter identification number: ";
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            std::cin >> employee.num;
        }
        std::cout << "Enter name: ";
        std::cin >> std::setw(10) >> employee.name;
        employee.name[9] = '\0';
        std::cout << "Enter hours: ";
        std::cin >> employee.hours;
        employees.push_back(employee);
        file.write(reinterpret_cast<char*>(&employee), sizeof(employee));
    }
    file.close();
    std::cout << "Information about employees is written in binary file." << std::endl;
}
void displayEmployees() {
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "The file could not be opened." << std::endl;
        return;
    }
    std::cout << "File contents" << std::endl;
    Employee emp{};
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        std::cout << "ID: " << emp.num << ", Name: " << emp.name << ", Hours: " << emp.hours << std::endl;
    }
    std::cout << std::endl;
}

void modifyEmployee(const Employee& emp) {
    std::fstream outputFile(fileName, std::ios::binary | std::ios::in | std::ios::out);
    if (!outputFile) {
        std::cerr << "Cannot open file for writing.\n";
        return;
    }

    Employee temp;
    while (outputFile.read(reinterpret_cast<char*>(&temp), sizeof(temp))) {
        if (temp.num == emp.num) {
            outputFile.seekp(-static_cast<int>(sizeof(temp)), std::ios::cur);
            outputFile.write(reinterpret_cast<const char*>(&emp), sizeof(emp));
            break;
        }
    }
}
Employee readEmployeeByID(int employeeID) {
    std::ifstream inputFile(fileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Cannot open file for reading." << std::endl;
        return { 0, "", 0.0 };
    }
    Employee emp{};
    while (inputFile.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        if (emp.num == employeeID) {
            return emp;
        }
    }

    return { 0, "", 0.0 };
}
DWORD WINAPI ProcessClient(LPVOID param) {
    HANDLE hPipe = (HANDLE)param;
    DWORD bytesRead, bytesWrite;
    while (true) {
        int choise;
        if (!ReadFile(hPipe, &choise, sizeof(choise), &bytesRead, NULL) || bytesRead == 0) break;
        int ID;
        if (!ReadFile(hPipe, &ID, sizeof(ID), &bytesRead, NULL)) break;
        if (semaphores.find(ID) == semaphores.end()) {
            std::cerr << "Invalid ID: " << ID << " (no such employee)" << std::endl;

            Employee emptyEmp = { 0, "", 0.0 };
            DWORD bytesWrite;
            WriteFile(hPipe, &emptyEmp, sizeof(emptyEmp), &bytesWrite, NULL);
            char ack;
            DWORD bytesRead;
            ReadFile(hPipe, &ack, sizeof(ack), &bytesRead, NULL);
            continue;
        }
        HANDLE semaphore = semaphores[ID];
        std::cout << "Request: choise=" << choise << ", ID=" << ID << std::endl;
        if (choise == 1) {
            char m;
            WaitForSingleObject(mutex, INFINITE);
            for (int i = 0; i < clientsCount; i++) {
                WaitForSingleObject(semaphore, INFINITE);
            }
            ReleaseMutex(mutex);

            Employee employee = readEmployeeByID(ID);
            WriteFile(hPipe, &employee, sizeof(employee), &bytesWrite, NULL);
            if (ReadFile(hPipe, &employee, sizeof(employee), &bytesRead, NULL)) {
                modifyEmployee(employee);
            }
            ReadFile(hPipe, &m, sizeof(m), &bytesRead, NULL);
            ReleaseSemaphore(semaphore, clientsCount, NULL);
        }
        else if (choise == 2) {
            char m;
            WaitForSingleObject(semaphore, INFINITE);

            Employee emp = readEmployeeByID(ID);
            WriteFile(hPipe, &emp, sizeof(emp), &bytesWrite, NULL);

            ReadFile(hPipe, &m, sizeof(m), &bytesRead, NULL);
            ReleaseSemaphore(semaphore, 1, NULL);
        }

    }
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    return 0;
}
int main()
{
    std::cout << "Enter binary file name : ";
    std::getline(std::cin, fileName);

    createFile();
    displayEmployees();

    std::cout << "Enter count of Clients : ";
    std::cin >> clientsCount;
    while (clientsCount <= 0) {
        std::cout << "Enter a number greater than 0.\n";
        std::cout << "Enter count of Clients : ";
        std::cin.clear();
        std::cin.ignore(INT_MAX, '\n');
        std::cin >> clientsCount;
    }

    mutex = CreateMutex(NULL, FALSE, NULL);
    if (!mutex) {
        std::cerr << "Failed to create global mutex. Error: " << GetLastError() << std::endl;
        return 1;
    }

    for (const auto& emp : employees) {
        semaphores[emp.num] = CreateSemaphore(NULL, clientsCount, clientsCount, NULL);
        if (!semaphores[emp.num]) {
            std::cerr << "Failed to create semaphore for employee ID " << emp.num << ". Error: " << GetLastError() << std::endl;
            return 1;
        }
    }

    std::vector<HANDLE> clientThreads;
    for (int i = 0; i < clientsCount; ++i) {
        HANDLE hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 1024, 1024, 0, NULL);
        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Error creating name pipe." << std::endl;
            continue;
        }
        std::cout << "Waiting for the client to connections " << (i + 1) << "..." << std::endl;
        STARTUPINFOA startInfo = {};
        PROCESS_INFORMATION procInfo = {};
        startInfo.cb = sizeof(startInfo);
        std::string command = "Client.exe";
        LPSTR cmdLine = const_cast<LPSTR>(command.c_str());
        if (!CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startInfo, &procInfo)) {
            std::cerr << "Failed to create client process. The last error code: " << GetLastError() << std::endl;
            CloseHandle(hPipe);
            continue;
        }

        if (ConnectNamedPipe(hPipe, NULL)) {
            HANDLE hThread = CreateThread(NULL, 0, ProcessClient, hPipe, 0, NULL);
            clientThreads.push_back(hThread);
        }
        else {
            std::cerr << "Client " << (i + 1) << " isn't connected. The last error code: " << GetLastError() << std::endl;
            CloseHandle(hPipe);
        }
        CloseHandle(procInfo.hProcess);
        CloseHandle(procInfo.hThread);
    }
    WaitForMultipleObjects(clientThreads.size(), clientThreads.data(), TRUE, INFINITE);
    for (HANDLE h : clientThreads) {
        CloseHandle(h);
    }
    for (const auto& pair : semaphores) {
        if (pair.second) {
            CloseHandle(pair.second);
        }
    }
    CloseHandle(mutex);

    displayEmployees();
    std::cout << "Press Enter to exit server..." << std::endl;
    std::cin.ignore();
    std::cin.get();

    return 0;
}







//#include <windows.h>
//#include <iostream>
//#include <fstream>
//#include <vector>
//
//const int MAX_MSG_LEN = 20;
//const int RECORD_SIZE = MAX_MSG_LEN + 1;
//
//int main() {
//    std::string filename;
//    int recordCount, senderCount;
//
//    std::cout << "Enter binary file name: ";
//    while (!(std::cin >> filename) || filename.empty()) {
//        std::cin.clear();
//        std::cin.ignore(10000, '\n');
//        std::cout << "Invalid input. Enter binary file name: ";
//    }
//
//    std::cout << "Enter number of records: ";
//    while (!(std::cin >> recordCount) || recordCount <= 0) {
//        std::cin.clear();
//        std::cin.ignore(10000, '\n');
//        std::cout << "Invalid input. Enter positive number: ";
//    }
//
//    std::cout << "Enter number of Sender processes: ";
//    while (!(std::cin >> senderCount) || senderCount <= 0) {
//        std::cin.clear();
//        std::cin.ignore(10000, '\n');
//        std::cout << "Invalid input. Enter positive number: ";
//    }
//
//    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
//    int writeIndex = 0, readIndex = 0;
//    file.write(reinterpret_cast<char*>(&writeIndex), sizeof(int));
//    file.write(reinterpret_cast<char*>(&readIndex), sizeof(int));
//    std::vector<char> empty(RECORD_SIZE * recordCount, 0);
//    file.write(empty.data(), empty.size());
//    file.close();
//
//    HANDLE emptySlots = CreateSemaphoreA(NULL, recordCount, recordCount, "emptySlots");
//    HANDLE filledSlots = CreateSemaphoreA(NULL, 0, recordCount, "filledSlots");
//    HANDLE fileMutex = CreateMutexA(NULL, FALSE, "fileMutex");
//
//    for (int i = 0; i < senderCount; ++i) {
//        std::string cmd = "Sender.exe " + filename;
//        STARTUPINFOA si = { sizeof(si) };
//        PROCESS_INFORMATION pi;
//        if (!CreateProcessA(NULL, const_cast<char*>(cmd.c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
//            std::cerr << "Failed to start Sender " << i << std::endl;
//        }
//        else {
//            CloseHandle(pi.hProcess);
//            CloseHandle(pi.hThread);
//        }
//    }
//
//    while (true) {
//        int choice;
//        std::cout << "\nEnter 1 to read message, 0 to exit: ";
//        std::cin >> choice;
//
//        if (choice == 0) break;
//        if (choice != 1) {
//            std::cout << "\nThere is no such choice!";
//            continue;
//        }
//         
//        DWORD result = WaitForSingleObject(filledSlots, 0); 
//        if (result != WAIT_OBJECT_0) { 
//            std::cout << "No messages available. Queue is empty.\n"; 
//            continue;
//        }
//
//        WaitForSingleObject(fileMutex, INFINITE);
//
//        std::fstream f(filename, std::ios::binary | std::ios::in | std::ios::out);
//        f.seekg(sizeof(int)); 
//        f.read(reinterpret_cast<char*>(&readIndex), sizeof(int));
//
//        int offset = sizeof(int) * 2 + readIndex * RECORD_SIZE;
//        f.seekg(offset);
//        char buffer[RECORD_SIZE] = { 0 };
//        f.read(buffer, RECORD_SIZE);
//
//        std::cout << "Received: " << buffer;
//
//        f.seekp(offset);
//        std::vector<char> zero(RECORD_SIZE, 0);
//        f.write(zero.data(), RECORD_SIZE);
//
//        readIndex = (readIndex + 1) % recordCount;
//        f.seekp(sizeof(int));
//        f.write(reinterpret_cast<char*>(&readIndex), sizeof(int));
//
//        f.close();
//        ReleaseMutex(fileMutex);
//        ReleaseSemaphore(emptySlots, 1, NULL);
//    }
//
//    CloseHandle(emptySlots);
//    CloseHandle(filledSlots);
//    CloseHandle(fileMutex);
//    return 0;
//}
