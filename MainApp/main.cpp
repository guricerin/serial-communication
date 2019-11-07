#include <iostream>
#include <string>
#include "../SerialWrapper/serial_wrapper.h"

namespace sc = serial_communication;

void print_result(sc::CommResult result) {
    switch (result) {
    case sc::CommResult::Success:
        std::cout << "Comm Success!\n";
        break;
    case sc::CommResult::Error:
        std::cout << "Comm Error Occured!\n";
        exit(1);
        break;
    case sc::CommResult::Timeout:
        std::cout << "Comm Timeout\n";
        exit(1);
        break;
    case sc::CommResult::NotRecieved:
        std::cout << "Message has not recieved\n";
        break;
    default:
        break;
    }
}

void title() {
    std::cout << "########   ######   #######   #######   #######   ######  " << "\n";
    std::cout << "##     ## ##    ## ##     ## ##     ## ##     ## ##    ## " << "\n";
    std::cout << "##     ## ##              ##        ##        ## ##       " << "\n";
    std::cout << "########   ######   #######   #######   #######  ##       " << "\n";
    std::cout << "##   ##         ## ##               ## ##        ##       " << "\n";
    std::cout << "##    ##  ##    ## ##        ##     ## ##        ##    ## " << "\n";
    std::cout << "##     ##  ######  #########  #######  #########  ######  " << "\n";
    std::cout << "\n";
}

void prompt() {
    std::cout << "user> ";
}

int main()
{
    auto serial = sc::SerialWrapper(3);
    serial.set_timeout(10, 100);
    serial.begin();

    title();
    std::string line;
    while (true) {
        prompt();
        // 空白を含めて1行読み込む
        std::getline(std::cin, line);

        if (line == "exit") {
            std::cout << "bye\n";
            break;
        }

        const auto cstr = line.c_str();
        const auto res = serial.write_bytes(cstr, line.size());
        print_result(res);
    }
}
