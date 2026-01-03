#include <iostream>
#include <iomanip>
#include <windows.h>
#include <conio.h>

struct Employee {
    int num;
    char name[10];
    double hours;
};

int main()
{
    HANDLE hPipe = CreateFileW(L"\\\\.\\pipe\\employee_pipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Creation of the named pipe failed. The last error code:" << GetLastError() << std::endl;
        return 1;
    }

    bool exit = false;
    while (!exit) {
        int choise;
        std::cout << "Enter number of operation:\n1 - modification of file recording;\n2 - reading of the record;\n3 - exit from the cycle;" << std::endl;
        std::cin >> choise;
        int ID;

        switch (choise) {
        case 1: {
            std::cout << "Enter employee's ID to modify file." << std::endl;
            std::cin >> ID;

            DWORD bytesWritten;
            if (!WriteFile(hPipe, &choise, sizeof(choise), &bytesWritten, NULL)) {
                std::cerr << "Data writing to the named pipe failed. The last error code: " << GetLastError() << std::endl;
                break;
            }

            WriteFile(hPipe, &ID, sizeof(ID), &bytesWritten, NULL);
            Employee employee;
            DWORD bytesRead;
            if (ReadFile(hPipe, &employee, sizeof(employee), &bytesRead, NULL) && bytesRead > 0) {
                if (employee.num != 0) {
                    std::cout << "ID: " << employee.num << ", Name: " << employee.name << ", Hours: " << employee.hours << std::endl;
                }
                else {
                    std::cout << "Employee with ID " << ID << " isn't found." << std::endl;
                    std::cout << "Press any key to continue." << std::endl;
                    _getch();
                    char c = 1;
                    WriteFile(hPipe, &c, sizeof(c), &bytesWritten, NULL);
                    break;
                }
            }
            else {
                std::cerr << "Data reading to the named pipe failed. The last error code: " << GetLastError() << std::endl;
            }

            std::cout << "Enter new name (max 9 symbols): ";
            std::cin >> std::setw(10) >> employee.name;
            employee.name[9] = '\0';

            std::cout << "Enter hours: ";
            std::cin >> employee.hours;
            employee.num = ID;

            if (!WriteFile(hPipe, &employee, sizeof(employee), &bytesWritten, NULL)) {
                std::cerr << "Data writing to the named pipe failed. The last error code: " << GetLastError() << std::endl;
                break;
            }

            std::cout << "Press Enter to push information to server..." << std::endl;
            std::cin.ignore();
            std::cin.get();

            std::cout << "Data for modification are written." << std::endl;
            std::cout << "Press any key to continue." << std::endl;
            _getch();
            char c = 1;
            WriteFile(hPipe, &c, sizeof(c), &bytesWritten, NULL);
            break;
        }
        case 2: {
            std::cout << "Enter employee's ID to read of the record." << std::endl;
            std::cin >> ID;

            DWORD bytesWritten;
            if (!WriteFile(hPipe, &choise, sizeof(choise), &bytesWritten, NULL)) {
                std::cerr << "Data writing to the named pipe failed. The last error code: " << GetLastError() << std::endl;
                break;
            }

            if (!WriteFile(hPipe, &ID, sizeof(ID), &bytesWritten, NULL)) {
                std::cerr << "Failed to send ID. Error: " << GetLastError() << std::endl;
                break;
            }

            Employee employee{};
            DWORD bytesRead;
            if (ReadFile(hPipe, &employee, sizeof(employee), &bytesRead, NULL) && bytesRead > 0) {
                if (employee.num != 0) {
                    std::cout << "ID: " << employee.num << ", Name: " << employee.name << ", Hours: " << employee.hours << std::endl;
                }
                else {
                    std::cout << "Employee with ID " << ID << " isn't found." << std::endl;
                }
            }
            else {
                std::cerr << "Data reading to the named pipe failed. The last error code: " << GetLastError() << std::endl;
            }

            std::cout << "Press any key to continue." << std::endl;
            _getch();
            char c = 1;
            WriteFile(hPipe, &c, sizeof(c), &bytesWritten, NULL);
            break;
        }
        case 3: {
            exit = true;
            break;
        }
        default: {
            std::cout << "Enter correct number, please." << std::endl;
            break;
        }
        }
    }
    CloseHandle(hPipe);
    return 0;
}







//#include <windows.h>
//#include <iostream>
//#include <fstream>
//#include <string>
//
//const int MAX_MSG_LEN = 20;
//const int RECORD_SIZE = MAX_MSG_LEN + 1;
//
//int main(int argc, char* argv[]) {
//    if (argc < 2) {
//        std::cerr << "Filename not provided.\n";
//        return 1;
//    }
//
//    std::string filename = argv[1];
//
//    HANDLE emptySlots = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "emptySlots");
//    HANDLE filledSlots = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "filledSlots");
//    HANDLE fileMutex = OpenMutexA(SYNCHRONIZE, FALSE, "fileMutex");
//
//    if (!emptySlots || !filledSlots || !fileMutex) {
//        std::cerr << "Failed to open synchronization objects.\n";
//        return 1;
//    }
//
//    while (true) {
//        int choice;
//        std::cout << "\nEnter 1 to send message, 0 to exit: ";
//        std::cin >> choice;
//        std::cin.ignore();
//
//        if (choice == 0) break;
//        if (choice != 1) {
//            std::cout << "\nThere is no such choice!";
//            continue;
//        }
//
//        std::string msg;
//        std::cout << "Enter message (max 20 chars): ";
//        std::getline(std::cin, msg);
//
//        if (msg.empty()) {
//            std::cout << "Message cannot be empty.\n";
//            continue;
//        }
//
//        if (msg.length() > MAX_MSG_LEN) {
//            msg = msg.substr(0, MAX_MSG_LEN);
//            std::cout << "Message truncated to: " << msg << std::endl;
//        }
//
//        DWORD result = WaitForSingleObject(emptySlots, 0); 
//        if (result != WAIT_OBJECT_0) {
//            std::cout << "Cannot send message now. Queue is full.\n";
//            continue;
//        }
//
//        WaitForSingleObject(fileMutex, INFINITE);
//
//        std::fstream f(filename, std::ios::binary | std::ios::in | std::ios::out);
//        int writeIndex;
//        f.read(reinterpret_cast<char*>(&writeIndex), sizeof(int));
//
//        int offset = sizeof(int) * 2 + writeIndex * RECORD_SIZE;
//        f.seekp(offset);
//        char buffer[RECORD_SIZE] = { 0 };
//        strncpy_s(buffer, sizeof(buffer), msg.c_str(), MAX_MSG_LEN);
//        buffer[MAX_MSG_LEN] = '\0'; 
//        f.write(buffer, RECORD_SIZE);
//
//        f.seekg(0, std::ios::end);
//        std::streamoff fileSize = f.tellg();
//        int recordCount = static_cast<int>((fileSize - sizeof(int) * 2) / RECORD_SIZE);
//
//        writeIndex = (writeIndex + 1) % recordCount;
//        f.seekp(0);
//        f.write(reinterpret_cast<char*>(&writeIndex), sizeof(int));
//
//        f.close();
//        ReleaseSemaphore(filledSlots, 1, NULL);
//        ReleaseMutex(fileMutex);
//
//        std::cout << "Message sent: " << msg << std::endl;
//    }
//
//    CloseHandle(emptySlots);
//    CloseHandle(filledSlots);
//    CloseHandle(fileMutex);
//    return 0;
//}
