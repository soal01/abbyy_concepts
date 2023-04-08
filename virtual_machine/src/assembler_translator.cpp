#include <iostream>
#include <fstream>
#include <string>
#include <vector>


class AssemblerTranslator {
private:
    std::ifstream& ass_programm;
    std::ofstream& bin_programm;
    std::string data;


    void write4bytes(int val) {
        char buffer[4];
        for (int i = 3; i >= 0; --i) {
            buffer[i] = val % 16;
            val /= 16;
        }
        bin_programm.write(buffer, 4);
    }

    void writeData() {
        bin_programm.write(data.c_str(), data.size());
    }

public:

    AssemblerTranslator(std::ifstream& ass_programm, std::ofstream& bin_programm):
    ass_programm(ass_programm), bin_programm(bin_programm), data("") {}

    void translate() {
        std::string command;
        
        std::vector<std::vector<int>> commandsInfo;

        while (std::getline(ass_programm, command)) {
            std::string command_name = "";
            int i = 0;
            for (; i < command.length(); ++i) {
                if (command[i] != ' ') {
                    command_name.push_back(command[i]);
                } else {
                    break;
                }
            }
            i++;
            if (command_name == "write") {
                i++;
                std::string str="";
                for (; i < command.length(); ++i) {
                    if (command[i] != '"') {
                        str += command[i];
                    }
                }
                commandsInfo.push_back({1, (int)data.size(), (int)str.size()});
                data += str;

                continue;
            }
        }
        
        int regCount = 5;
        int addrData = regCount*4;
        write4bytes(addrData + data.size());
        write4bytes(0);
        write4bytes(0);
        write4bytes(0);
        write4bytes(0);

        writeData();
        for (auto command : commandsInfo) {

            if (command[0] == 1) {
                bin_programm.put(1);
                write4bytes(addrData + command[1]);
                write4bytes(command[2]);
                continue;
            }
        }
        bin_programm.put(0);
    }

};



int main(int argc, char* argv[]) {
    char* ass_filename = argv[1];
    char* bin_filename = argv[2];

    std::ifstream ass_programm(ass_filename);
    std::ofstream bin_programm(bin_filename);

    AssemblerTranslator translator(ass_programm, bin_programm);
    translator.translate();

    return 0;
}