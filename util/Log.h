#ifndef LOG_H
#define LOG_H

#include <string>
#include <iostream>

using namespace std;

class Log {
public:
    static void success(const string& message) {
        cout << "[SUCCESS]" << " " << message << endl;
    }

    static void info(const string& message) {
        cout << "[INFO]" << " " << message << endl;

    }

    static void error(const string& message) {
        cerr << "[ERROR]" << " " << message << endl;
    }
};

#endif //LOG_H