#include "ResourceParser.h"
#include "ResourceData.h"

#include <iostream>
using namespace std;

ResourceParser::ResourceParser() {

}

ResourceParser::~ResourceParser() {

}

string ResourceParser::trimSpaces(const string &s) const {
    string newString = s;

    size_t firstNonSpacePos = newString.find_first_not_of(" \t");
    size_t lastNonSpacePos = newString.find_last_not_of(" \t");

    if (firstNonSpacePos == string::npos || lastNonSpacePos == string::npos) {
        newString = "";
    } else {
        newString = newString.substr(firstNonSpacePos,
                                     lastNonSpacePos - firstNonSpacePos + 1);
    }

    return newString;
}

vector<string> ResourceParser::getDataLines(ifstream &file) const {
    vector<string> usefulLines;

    while (!file.eof()) {
        char buffer[256];

        file.getline(buffer, 256);
        string line = string(buffer);
        line = trimSpaces(line);

        if (line.length() > 0) {
            if (line[0] != '[' && line[0] != '%') {
                usefulLines.push_back(line);
            }
        }
    }

    return usefulLines;
}

ResourceData *ResourceParser::readResourceData(ifstream &file) const {
    ResourceData *resourceData = 0;
    
    vector<string> usefulLines = getDataLines(file);

    string key = "";
    string value = "";

    for (unsigned int i = 0; i < usefulLines.size(); i++) {
        string line = usefulLines[i];

        if (line[0] != '$') {
            if (!resourceData) {
                resourceData = new ResourceData();
            }

            if (key.length() > 0) {
                key = trimSpaces(key);
                value = trimSpaces(value);

                resourceData->insert(key, value);
                key = "";
                value = "";
            }

            size_t keyStart = line.find_first_not_of("$");
            size_t keyEnd = line.find_first_of("=");
            if (keyEnd == string::npos) {
                key = line.substr(keyStart, line.length() - keyStart);
            } else {
                key = line.substr(keyStart, keyEnd - keyStart);
                value = line.substr(keyEnd + 1, line.length() - keyEnd + 1);
            }
        } else {
            if (value.length() > 0) {
                value += " ";
            }
            value += line;
        }
    }

    if (key.length() > 0) {
        key = trimSpaces(key);
        value = trimSpaces(value);

        resourceData->insert(key, value);
        key = "";
        value = "";
    }

    return resourceData;
}

ResourceData* ResourceParser::parseDataFromFile(const string filename) const {
    ResourceData *resourceData = 0;
    ifstream file;

    file.open(filename.c_str());
    if (file.is_open()) {
        while (!file.eof()) {
            resourceData = readResourceData(file);
        }

        file.close();
    }

    return resourceData;
}
