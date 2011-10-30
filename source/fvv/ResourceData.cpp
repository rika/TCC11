#include <cmath>

#include "ResourceData.h"

using namespace std;

ResourceData::ResourceData() {

}

ResourceData::~ResourceData() {

}

const int ResourceData::convertStringToInteger(const string &stringValue) const {
    string strValue = trimSpaces(stringValue);

    int value = 0;
    int sign = 1;

    unsigned int i = 0;
    if (strValue[i] == '-') {
        sign = -1;
        i++;
    }

    while (i < strValue.size()) {
        value = value * 10 + (strValue[i] - '0');
        i++;
    }
    value = value * sign;

    return value;
}

const double ResourceData::convertStringToReal(const string &stringValue) const {
    string strValue = trimSpaces(stringValue);

    double value = 0.0;
    double sign = 1.0;

    unsigned int i = 0;
    if (strValue[i] == '-') {
        sign = -1.0;
        i++;
    }

    while (i < strValue.size() && strValue[i] != '.') {
        value = value * 10.0 + (strValue[i] - '0');
        i++;
    }

    if (strValue[i] == '.') {
        i++;
    }

    int powFactor = 1;
    double frac = 0.0;
    while (i < strValue.size()) {
        frac += (strValue[i] - '0') * pow(0.1, powFactor);

        powFactor++;
        i++;
    }
    value = (value + frac) * sign;

    return value;
}

string ResourceData::trimSpaces(const string &s) const {
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

void ResourceData::insert(const string &key, const string &value) {
    map<string, string>::const_iterator it = data.find(key);
    string newValue = value;

    if (it != data.end()) {
        pair<string, string> p = (*it);
        newValue = p.second + ", " + value;
    }

    data[key] = newValue;
}

const string ResourceData::getStringValue(const string &key) const {
    string value = "";

    map<string, string>::const_iterator it = data.find(key);

    if (it != data.end()) {
        pair<string, string> p = (*it);
        value = p.second;
    }

    return value;
}

const int ResourceData::getIntValue(const string &key) const {
    int value = 0;

    map<string, string>::const_iterator it = data.find(key);

    if (it != data.end()) {
        pair<string, string> p = (*it);
        string strValue = p.second;

        value = convertStringToInteger(strValue);
    }

    return value;
}

const double ResourceData::getRealValue(const string &key) const {
    double value = 0.0;

    map<string, string>::const_iterator it = data.find(key);

    if (it != data.end()) {
        pair<string, string> p = (*it);
        string strValue = p.second;

        value = convertStringToReal(strValue);
    }

    return value;
}

const vector<string> ResourceData::getStringListValue(const string &key,
						      const char separator) const {
    vector<string> stringList;
    map<string, string>::const_iterator it = data.find(key);

    if (it != data.end()) {
        pair<string, string> p = (*it);
        string value = p.second;

        size_t sepPosition = value.find_first_of(separator);
        while (sepPosition != string::npos) {
            string element = value.substr(0, sepPosition);
            element = trimSpaces(element);
            stringList.push_back(element);
            value = value.substr(sepPosition + 1,
                                 value.length() - sepPosition + 2);

            sepPosition = value.find_first_of(separator);
        }

        if (value.length() > 0) {
            string element = trimSpaces(value);
            stringList.push_back(element);
        }
    }

    return stringList;
}

const vector<int> ResourceData::getIntegerListValue(const string &key,
						    const char separator) const {
    vector<int> integerList;
    map<string, string>::const_iterator it = data.find(key);

    if (it != data.end()) {
        pair<string, string> p = (*it);
        string value = p.second;

        size_t sepPosition = value.find_first_of(separator);
        while (sepPosition != string::npos) {
            int element = convertStringToInteger(value.substr(0, sepPosition));
            integerList.push_back(element);
            value = value.substr(sepPosition + 1,
                                 value.length() - sepPosition + 2);

            sepPosition = value.find_first_of(separator);
        }

        if (value.length() > 0) {
            int element = convertStringToInteger(value);
            integerList.push_back(element);
        }
    }

    return integerList;
}

const vector<double> ResourceData::getRealListValue(const string &key,
						    const char separator) const {
    vector<double> realList;
    map<string, string>::const_iterator it = data.find(key);

    if (it != data.end()) {
        pair<string, string> p = (*it);
        string value = p.second;

        size_t sepPosition = value.find_first_of(separator);
        while (sepPosition != string::npos) {
            double element = convertStringToReal(value.substr(0, sepPosition));
            realList.push_back(element);
            value = value.substr(sepPosition + 1,
                                 value.length() - sepPosition + 2);

            sepPosition = value.find_first_of(separator);
        }

        if (value.length() > 0) {
            double element = convertStringToReal(value);
            realList.push_back(element);
        }
    }

    return realList;
}
