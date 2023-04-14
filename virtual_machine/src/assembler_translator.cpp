#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>


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

    int getRegAddr(const std::string& reg) {
        int addr;
        if (reg == "ax") {
            addr = 4;
        }
        if (reg == "bx") {
            addr = 8;
        }
        if (reg == "cx") {
            addr = 12;
        }
        if (reg == "dx") {
            addr = 16;
        }
        return addr;
    }


public:

    AssemblerTranslator(std::ifstream& ass_programm, std::ofstream& bin_programm):
    ass_programm(ass_programm), bin_programm(bin_programm), data("") {}

    void translate() {
        std::string command;
        
        std::vector<std::vector<int>> commandsInfo;
        std::map<int, std::string> lostLabels;
        std::map<std::string, int> labels;

        while (std::getline(ass_programm, command)) {
            std::string command_name = "";
            if (command[0] == ':') {
                std::string label = "";
                for (int i = 1; i < command.length(); ++i) {
                    if (command[i] != ' ') {
                        label.push_back(command[i]);
                    } else {
                        break;
                    }
                }
                labels[label] = commandsInfo.size();
                continue;
            }


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
                if (command[i] == '$') {
                    i++;
                    std::string reg = "";
                    for (; i < command.length(); ++i) {
                        if (command[i] != ' ') {
                            reg += command[i];
                        }
                    }
                    int addr = 0;
                    if (reg == "ax") {
                        addr = 4;
                    }
                    if (reg == "bx") {
                        addr = 8;
                    }
                    if (reg == "cx") {
                        addr = 12;
                    }
                    if (reg == "dx") {
                        addr = 16;
                    }
                    commandsInfo.push_back({11, addr, 0});
                } else {
                    i++;
                    std::string str="";
                    for (; i < command.length(); ++i) {
                        if (command[i] != '"') {
                            str += command[i];
                        }
                    }

                    commandsInfo.push_back({1, (int)data.size(), (int)str.size()});
                    data += str;
                }
                continue;
            }

            if (command_name == "read") {
                i++;
                    std::string reg = "";
                    for (; i < command.length(); ++i) {
                        if (command[i] != ' ') {
                            reg += command[i];
                        }
                    }
                    int addr = 0;
                    if (reg == "ax") {
                        addr = 4;
                    }
                    if (reg == "bx") {
                        addr = 8;
                    }
                    if (reg == "cx") {
                        addr = 12;
                    }
                    if (reg == "dx") {
                        addr = 16;
                    }
                    commandsInfo.push_back({12, addr, 0});
            }

            if (command_name == "mov") {
                i++;
                std::string srcReg="";
                for (; i < command.length(); ++i) {
                    if (command[i] !=  '$') {
                        if (command[i] == ' ') {
                            break;
                        }
                        srcReg += command[i];
                    }
                }
                i++;
                
                std::string val = "";
                for (; i < command.length(); ++i) {
                    val += command[i];
                }
                commandsInfo.push_back({3, getRegAddr(srcReg), (int)strtoll(val.c_str(), NULL, 10)});
                continue;
            }

            if (command_name == "sub") {
                i++;
                std::string srcReg="";
                for (; i < command.length(); ++i) {
                    if (command[i] !=  '$') {
                        if (command[i] == ' ') {
                            break;
                        }
                        srcReg += command[i];
                    }
                }
                i++;
                
                std::string val = "";
                for (; i < command.length(); ++i) {
                    val += command[i];
                }
                commandsInfo.push_back({6, getRegAddr(srcReg), (int)strtoll(val.c_str(), NULL, 10)});
                continue;
            }

            if (command_name == "je") {
                std::string label="";
                for (; i < command.length(); ++i) {
                    
                    if (command[i] == ' ') {
                        break;
                    }
                    label += command[i];
                    
                }
                if (labels.find(label) == labels.end()) {
                    lostLabels[commandsInfo.size()] = label;
                }
                commandsInfo.push_back({10, labels[label]*9, 0});
                continue;
            }

            if (command_name == "jump") {
                std::string label="";
                for (; i < command.length(); ++i) {
                    
                    if (command[i] == ' ') {
                        break;
                    }
                    label += command[i];
                    
                }
                if (labels.find(label) == labels.end()) {
                    lostLabels[commandsInfo.size()] = label;
                }
                commandsInfo.push_back({9, labels[label]*9, 0});
                continue;
            }

            if (command_name == "add") {
                i++;
                std::string dstReg="";
                for (; i < command.length(); ++i) {
                    if (command[i] !=  '$') {
                        if (command[i] == ' ') {
                            break;
                        }
                        dstReg += command[i];
                    }
                }
                i++;
                
                std::string srcReg="";
                for (; i < command.length(); ++i) {
                    if (command[i] !=  '$') {
                        if (command[i] == ' ') {
                            break;
                        }
                        srcReg += command[i];
                    }
                }
                
                commandsInfo.push_back({5, getRegAddr(dstReg), getRegAddr(srcReg)});
                continue;
            }

            if (command_name == "swap") {
                i++;
                std::string dstReg="";
                for (; i < command.length(); ++i) {
                    if (command[i] !=  '$') {
                        if (command[i] == ' ') {
                            break;
                        }
                        dstReg += command[i];
                    }
                }
                i++;
                
                std::string srcReg="";
                for (; i < command.length(); ++i) {
                    if (command[i] !=  '$') {
                        if (command[i] == ' ') {
                            break;
                        }
                        srcReg += command[i];
                    }
                }
                
                commandsInfo.push_back({8, getRegAddr(dstReg), getRegAddr(srcReg)});
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
        for (int i = 0; i < commandsInfo.size(); ++i) {
            auto command = commandsInfo[i];
            if (command[0] == 1) {
                bin_programm.put(1);
                write4bytes(addrData + command[1]);
                write4bytes(command[2]);
                continue;
            }
            if (command[0] == 9 || command[0] == 10) {
                bin_programm.put(command[0]);
                if (lostLabels.find(i) != lostLabels.end()) {
                    command[1] = labels[lostLabels[i]]*9;
                }
                write4bytes(addrData + data.size() + command[1]);
                write4bytes(command[2]);
                continue;
            } else {
                bin_programm.put(command[0]);
                write4bytes(command[1]);
                write4bytes(command[2]);
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