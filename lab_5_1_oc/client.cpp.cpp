#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <conio.h>
#include <process.h> 
#include <boost/interprocess/ipc/message_queue.hpp>

namespace bip = boost::interprocess;
using namespace std;

const int MAX_MESSAGE_SIZE = 1024;
const char* SERVER_CONNECT_QUEUE = "Server_Connect_Queue";

enum CommandType {
    CMD_MODIFY = 1,
    CMD_READ = 2,
    CMD_EXIT = 3
};

struct employee {
    int num;
    char name[10];
    double hours;
};

struct ConnectionInfo {
    int clientId;
    char clientTxQueue[256];
    char clientRxQueue[256];
};

struct Request {
    CommandType cmd;
    int employeeID;
};

struct Response {
    bool status;
    employee record;
};

int main() {
    int pid = _getpid();
    string txName = "Client_Tx_" + to_string(pid);
    string rxName = "Client_Rx_" + to_string(pid);

    try {
        bip::message_queue::remove(txName.c_str());
        bip::message_queue::remove(rxName.c_str());

        bip::message_queue txQueue(bip::create_only, txName.c_str(), 10, MAX_MESSAGE_SIZE);
        bip::message_queue rxQueue(bip::create_only, rxName.c_str(), 10, sizeof(Response));

        cout << "Connecting to server...\n";

        bip::message_queue connectQueue(bip::open_only, SERVER_CONNECT_QUEUE);

        ConnectionInfo info;
        info.clientId = pid;
        strcpy(info.clientTxQueue, txName.c_str());
        strcpy(info.clientRxQueue, rxName.c_str());

        connectQueue.send(&info, sizeof(ConnectionInfo), 0);
        cout << "Connected to server.\n";

        while (true) {
            cout << "\nChoose option:\n";
            cout << "1. Modify record\n";
            cout << "2. Read record\n";
            cout << "3. Exit\n";
            cout << "> ";

            int choice;
            cin >> choice;

            if (choice == 3) {
                Request req;
                req.cmd = CMD_EXIT;
                txQueue.send(&req, sizeof(Request), 0);
                break;
            }

            if (choice != 1 && choice != 2) continue;

            Request req;
            req.cmd = (choice == 1) ? CMD_MODIFY : CMD_READ;

            cout << "Enter Employee ID: ";
            cin >> req.employeeID;

            txQueue.send(&req, sizeof(Request), 0);

            Response resp;
            bip::message_queue::size_type r_size;
            unsigned int prio;
            rxQueue.receive(&resp, sizeof(Response), r_size, prio);

            if (!resp.status) {
                cout << "Employee with ID " << req.employeeID << " not found.\n";
                continue;
            }

            cout << "ID: " << resp.record.num << "\nName: " << resp.record.name << "\nHours: " << resp.record.hours << endl;

            if (req.cmd == CMD_READ) {
                cout << "Press any key to finish reading...";
                _getch();

                char msg[] = "DONE";
                txQueue.send(msg, sizeof(msg), 0);
            }
            else {
                cout << "Enter new Name: ";
                cin >> resp.record.name;
                cout << "Enter new Hours: ";
                cin >> resp.record.hours;

                txQueue.send(&resp.record, sizeof(employee), 0);

                cout << "Updated record sent.\n";
                cout << "Press any key to finish...";
                _getch();
            }
        }

        bip::message_queue::remove(txName.c_str());
        bip::message_queue::remove(rxName.c_str());
    }
    catch (bip::interprocess_exception& ex) {
        cout << "Client Error: " << ex.what() << endl;
        _getch();
        return 1;
    }

    return 0;
}