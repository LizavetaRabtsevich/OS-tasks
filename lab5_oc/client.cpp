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

