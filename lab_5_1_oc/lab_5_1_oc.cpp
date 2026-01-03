#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <map>
#include <fstream> 
#include <string>
#include <thread>
#include <mutex>

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>


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
namespace bip = boost::interprocess;
using namespace std;

string filename;
int clientCount;

bip::interprocess_mutex stateMutex;
bip::interprocess_condition stateCond;

map<int, int> activeReaders;
map<int, bool> activeWriter;

void PrintFile() {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) return;

    employee emp;
    cout << "\n--- File Content ---\n";
    while (fin.read((char*)&emp, sizeof(employee))) {
        cout << "ID: " << emp.num << ", Name: " << emp.name << ", Hours: " << emp.hours << endl;
    }
    cout << "--------------------\n";
    fin.close();
}

int FindRecord(int id, employee& outEmp) {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) return -1;

    employee emp;
    int pos = 0;
    while (fin.read((char*)&emp, sizeof(employee))) {
        if (emp.num == id) {
            outEmp = emp;
            return pos;
        }
        pos++;
    }
    return -1;
}

void WriteRecord(int pos, employee emp) {
    fstream f(filename, ios::binary | ios::in | ios::out);
    if (f.is_open()) {
        f.seekp(pos * sizeof(employee), ios::beg);
        f.write((char*)&emp, sizeof(employee));
        f.close();
    }
}

void ClientHandler(ConnectionInfo info) {
    try {
        bip::message_queue mqIn(bip::open_only, info.clientTxQueue);
        bip::message_queue mqOut(bip::open_only, info.clientRxQueue);

        cout << "Client " << info.clientId << " connected.\n";

        bool connected = true;

        char rawBuffer[MAX_MESSAGE_SIZE];
        bip::message_queue::size_type recvd_size;
        unsigned int priority;

        while (connected) {
            mqIn.receive(rawBuffer, MAX_MESSAGE_SIZE, recvd_size, priority);

            Request* reqPtr = reinterpret_cast<Request*>(rawBuffer);
            Request req = *reqPtr;

            if (req.cmd == CMD_EXIT) {
                break;
            }

            int id = req.employeeID;
            employee empData;
            int pos = FindRecord(id, empData);

            Response resp;
            resp.status = (pos != -1);
            resp.record = empData;

            if (!resp.status) {
                mqOut.send(&resp, sizeof(Response), 0);
                continue;
            }

            {
                bip::scoped_lock<bip::interprocess_mutex> lock(stateMutex);

                if (req.cmd == CMD_READ) {
                    stateCond.wait(lock, [&] { return !activeWriter[id]; });
                    activeReaders[id]++;
                }
                else if (req.cmd == CMD_MODIFY) {
                    stateCond.wait(lock, [&] { return !activeWriter[id] && activeReaders[id] == 0; });
                    activeWriter[id] = true;
                }
            }

            mqOut.send(&resp, sizeof(Response), 0);

            if (req.cmd == CMD_READ) {
                mqIn.receive(rawBuffer, MAX_MESSAGE_SIZE, recvd_size, priority);

                {
                    bip::scoped_lock<bip::interprocess_mutex> lock(stateMutex);
                    activeReaders[id]--;
                    stateCond.notify_all();
                }
            }
            else if (req.cmd == CMD_MODIFY) {
                mqIn.receive(rawBuffer, MAX_MESSAGE_SIZE, recvd_size, priority);
                employee* newEmpPtr = reinterpret_cast<employee*>(rawBuffer);

                WriteRecord(pos, *newEmpPtr);

                {
                    bip::scoped_lock<bip::interprocess_mutex> lock(stateMutex);
                    activeWriter[id] = false;
                    stateCond.notify_all();
                }
            }
        }

        cout << "Client " << info.clientId << " disconnected.\n";
    }
    catch (bip::interprocess_exception& ex) {
        cout << "Client Handler Error: " << ex.what() << endl;
    }
}

int main() {
    bip::message_queue::remove(SERVER_CONNECT_QUEUE);

    cout << "Enter filename: ";
    cin >> filename;

    cout << "Enter number of employees: ";
    int empCount;
    cin >> empCount;

    ofstream fout(filename, ios::binary);
    for (int i = 0; i < empCount; ++i) {
        employee emp;
        emp.num = i + 1;
        sprintf(emp.name, "Emp%d", i + 1);
        emp.hours = 0.0;

        cout << "Enter ID, Name, Hours for employee " << i + 1 << ":\n";
        cin >> emp.num >> emp.name >> emp.hours;

        fout.write((char*)&emp, sizeof(employee));
    }
    fout.close();
    PrintFile();

    cout << "Enter number of clients: ";
    cin >> clientCount;

    try {
        bip::message_queue connectQueue(bip::create_only, SERVER_CONNECT_QUEUE, 20, sizeof(ConnectionInfo));

        for (int i = 0; i < clientCount; ++i) {
            string cmd = "start Client.exe";
            system(cmd.c_str());
        }

        cout << "Server started. Waiting for connections...\n";

        while (true) {
            ConnectionInfo info;
            bip::message_queue::size_type recvd_size;
            unsigned int priority;

            connectQueue.receive(&info, sizeof(ConnectionInfo), recvd_size, priority);
            std::thread(ClientHandler, info).detach();
        }
    }
    catch (bip::interprocess_exception& ex) {
        cout << "Server Main Error: " << ex.what() << endl;
        bip::message_queue::remove(SERVER_CONNECT_QUEUE);
        return 1;
    }

    bip::message_queue::remove(SERVER_CONNECT_QUEUE);
    return 0;
}