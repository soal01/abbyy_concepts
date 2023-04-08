#include <iostream>
#include <fstream>




class VirtualMachine {
private:
    std::ifstream& programm;
    int ip, ax, bx, cx, dx;
    bool isEnd;



    void readRegisters() {
        ip = read4Bytes(0);
        ax = read4Bytes(4);
        bx = read4Bytes(8);
        cx = read4Bytes(12);
        dx = read4Bytes(16);
    }

    int read4Bytes(int addr) {
        programm.seekg(addr, std::ios::beg);

        int ans = 0;
        char buffer[4];
        programm.read(buffer, 4);
        for (int i = 0; i < 4; ++i) {
            ans = ans*16 + buffer[i];
        }
        //std::cout << ans << std::endl;
        
        return ans;
    }

    void executeCurCommand() {
        executeCommand(ip);
        ip += 9;
    }

    void executeCommand(int addr) {
        programm.seekg(addr, std::ios::beg);
        char commandType = programm.get();

        if (commandType == 0) {
            isEnd = true;
            return;
        }
        if (commandType == 1) {
            int dataAddr = read4Bytes(addr + 1);
            int dataLen = read4Bytes(addr + 5);
            writeCommand(dataAddr, dataLen);
        }
    }

    void writeCommand(int addr, int len) {
        programm.seekg(addr, std::ios::beg);
        for (int i = 0; i < len; ++i) {
            char c = programm.get();
            std::cout << c;
        }
    }


public:
    VirtualMachine(std::ifstream& programm) : programm(programm), isEnd(false) {
        readRegisters();
    }



    void execute() {
        while (!isEnd) {
            executeCurCommand();
        }
    }
};



int main(int argc, char* argv[]) {

    char* filename = argv[1];
    std::ifstream programm = std::ifstream(filename, std::ios::in);
    VirtualMachine vm(programm);

    vm.execute();

    programm.close();
}