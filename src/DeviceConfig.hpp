#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include <regex>

using namespace std;

class DeviceConfig 
{
public:
    DeviceConfig(string filepath) 
    {
        file_path = filepath;
    #ifdef DEBUG
    #endif
        fstream ifs(file_path);
        string line;
        while( getline(ifs, line) )
        {
            std::regex ws_re("[\\s=]+"); 
            std::vector<std::string> tokens(std::sregex_token_iterator(line.begin(), line.end(), ws_re, -1),     std::sregex_token_iterator());
        #ifdef DEBUG
            cout << tokens[0] << " = " << tokens[1] << endl;
        #endif
            if (tokens.size() > 1) 
                data[tokens[0]] = tokens[1]; 
        }
        ifs.close();
    }

    ~DeviceConfig(){}

    map<string, string>& get_data() {
        return data;
    }
    string get(string key) {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }else {
            cout << "[Config] can't found : " << key << endl;
        }
    }
    void set(string key, string value) {
        auto it = data.find(key);
        if (it != data.end()) {
            data[key] = value;
        }else {
            cout << "[Config] can't found : " << key << endl;
        }
        ofstream ofs(file_path);
        string line;
        for (auto it = data.begin(); it!=data.end(); ++it) {
            line = it->first + " = '" + it->second + "'\n";
            ofs << line;
        }
        ofs.close();
    }
    void print() {
        for (auto iter =data.begin(); iter != data.end(); ++iter) {
            cout << iter->first << " = " << iter->second << endl;
        }
    }
private:
    string file_path;
public:
    map<string, string> data;
};

