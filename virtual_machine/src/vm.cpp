#include <iostream>
#include <fstream>




class VirtualMachine {
private:
    std::fstream& programm;
    int ip, ax, bx, cx, dx;
    int initialIp;
    int lastCommand;
    bool isEnd;



    void readRegisters() {
        ip = read4Bytes(0);
        ax = read4Bytes(4);
        bx = read4Bytes(8);
        cx = read4Bytes(12);
        dx = read4Bytes(16);
        initialIp = ip;
    }

    int read4Bytes(int addr) {
        programm.seekg(addr, std::ios::beg);

        int ans = 0;
        char buffer[4];
        programm.read(buffer, 4);
        for (int i = 0; i < 4; ++i) {
            ans = ans*16 + buffer[i];
        }
        
        return ans;
    }

    void write4Bytes(int addr, int val) {
        programm.seekg(addr, std::ios::beg);

        char buffer[4];
        for (int i = 3; i >= 0; --i) {
            buffer[i] = val % 16;
            val /= 16;
        }
        programm.write(buffer, 4);
    }

    void executeCurCommand() {
        executeCommand(ip);
        ip = read4Bytes(0);
        if (lastCommand != 9 && lastCommand != 0) {
            ip += 9;
        }
        write4Bytes(0, ip);
    }

    void executeCommand(int addr) {
        programm.seekg(addr, std::ios::beg);
        char commandType = programm.get();

        lastCommand = commandType;
        if (commandType == 0) {
            isEnd = true;
            write4Bytes(0, initialIp);
            return;
        }
        if (commandType == 1) {
            int dataAddr = read4Bytes(addr + 1);
            int dataLen = read4Bytes(addr + 5);
            writeCommand(dataAddr, dataLen);
            return;
        }
        if (commandType == 2) {
            int destAddr = read4Bytes(addr + 1);
            int srcAddr = read4Bytes(addr + 5);
            int val = read4Bytes(srcAddr);
            write4Bytes(destAddr, val);
            return;
        }
        if (commandType == 3) {
            int destAddr = read4Bytes(addr + 1);
            int val = read4Bytes(addr + 5);
            write4Bytes(destAddr, val);
            return;
        }
        if (commandType == 4) {
            int destAddr = read4Bytes(addr + 1);
            int val = read4Bytes(addr + 5);
            int destVal = read4Bytes(destAddr);
            destVal += val;
            write4Bytes(destAddr, destVal);
            return;
        }
        if (commandType == 5) {
            int destAddr = read4Bytes(addr + 1);
            int srcAddr = read4Bytes(addr + 5);
            int destVal = read4Bytes(destAddr);
            int srcVal = read4Bytes(srcAddr);
            destVal += srcVal;
            write4Bytes(destAddr, destVal);
            return;
        }
        if (commandType == 6) {
            int destAddr = read4Bytes(addr + 1);
            int val = read4Bytes(addr + 5);
            int destVal = read4Bytes(destAddr);
            destVal -= val;
            write4Bytes(destAddr, destVal);
            return;
        }
        if (commandType == 7) {
            int destAddr = read4Bytes(addr + 1);
            int srcAddr = read4Bytes(addr + 5);
            int destVal = read4Bytes(destAddr);
            int srcVal = read4Bytes(srcAddr);
            destVal -= srcVal;
            write4Bytes(destAddr, destVal);
            return;
        }
        if (commandType == 8) {
            int destAddr = read4Bytes(addr + 1);
            int srcAddr = read4Bytes(addr + 5);
            int destVal = read4Bytes(destAddr);
            int srcVal = read4Bytes(srcAddr);
            std::swap(destVal, srcVal);
            write4Bytes(destAddr, destVal);
            write4Bytes(srcAddr, srcVal);
            return;
        }
        if (commandType == 9) {
            int destAddr = read4Bytes(addr + 1);
            write4Bytes(0, destAddr);
            return;
        }
        if (commandType == 10) {
            int cx = read4Bytes(12);
            if (cx == 0) {
                int destAddr = read4Bytes(addr + 1);
                write4Bytes(0, destAddr);
                lastCommand = 9;
            }
            return;
        }
        if (commandType == 11) {
            int destAddr = read4Bytes(addr + 1);
            int val = read4Bytes(destAddr);
            std::cout << val << std::endl;
            //writeRegisters();
            return;
        }
        if (commandType == 12) {
            int destAddr = read4Bytes(addr + 1);
            int val;
            std::cin >> val;
            write4Bytes(destAddr, val);
            return;
        }
    }

    void writeCommand(int addr, int len) {
        programm.seekg(addr, std::ios::beg);
        for (int i = 0; i < len; ++i) {
            char c = programm.get();
            std::cout << c;
        }
    }

    void writeRegisters() {
        int val = read4Bytes(0);
        std::cout << "ip: " << val << std::endl;
        val = read4Bytes(4);
        std::cout << "ax: " << val << std::endl;
        val = read4Bytes(8);
        std::cout << "bx: " << val << std::endl;
        val = read4Bytes(12);
        std::cout << "cx: " << val << std::endl;
        val = read4Bytes(16);
        std::cout << "dx: " << val << std::endl;
    }

public:
    VirtualMachine(std::fstream& programm) : programm(programm), isEnd(false) {
        readRegisters();
        //writeRegisters();
    }



    void execute() {
        while (!isEnd) {
            executeCurCommand();
        }
        //writeRegisters();
    }
};



int main(int argc, char* argv[]) {

    char* filename = argv[1];
    std::fstream programm = std::fstream(filename, std::ios::in | std::ios::out);
    VirtualMachine vm(programm);

    vm.execute();

    programm.close();
}