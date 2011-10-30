#ifndef __resourcedata__
#define __resourcedata__

#include <vector>
#include <map>
#include <string>

using namespace std;

/*!
 * \class ResourceData
 * \author Jeferson Rodrigues da Silva
 * \date 09/21/2008
 * \file ResourceData.h
 * \brief Data structure for storing resource data.
 */
class ResourceData {
private:
    /*!
     * \brief Converts a string to an integer number.
     * \param stringValue The string to be converted.
     * \return The integer number.
     */
    const int convertStringToInteger(const string &stringValue) const;

    /*!
     * \brief Converts a string to a real number.
     * \param stringValue The string to be converted.
     * \return The real number.
     */
    const double convertStringToReal(const string &stringValue) const;

    /*!
     * \brief Removes all leading and trailing spaces from a string.
     * \param s String whose spaces are to be removed.
     * \return The modified string.
     */
    string trimSpaces(const string &s) const;

    map<string, string> data;

public:
    /*!
     * \brief Constructor.
     */
    ResourceData();

    /*!
     * \brief Destructor.
     */
    ~ResourceData();

    /*!
     * \brief Registers a new key->value pair.
     * \param key The key.
     * \param value The associated value.
     */
    void insert(const string &key, const string &value);

    /*!
     * \brief Gets the string value associated to a specific key.
     * \param key The key.
     * \return The associated string value.
     */
    const string getStringValue(const string &key) const;

    /*!
     * \brief Gets the integer value associated to a specific key.
     * \param key The key.
     * \return The associated integer value.
     */
    const int getIntValue(const string &key) const;

    /*!
     * \brief Gets the real value associated to a specific key.
     * \param key The key.
     * \return The associated real value.
     */
    const double getRealValue(const string &key) const;

    /*!
     * \brief Gets the string list value associated to a specific key.
     * \param key The key.
     * \param separator The character to use as separator.
     * \return The associated string list value.
     */
    const vector<string> getStringListValue(const string &key,
					    const char separator) const;
    
    /*!
     * \brief Gets the integer list value associated to a specific key.
     * \param key The key.
     * \param separator The character to use as separator.
     * \return The associated integer list value.
     */
    const vector<int> getIntegerListValue(const string &key,
					  const char separator) const;

    /*!
     * \brief Gets the real list value associated to a specific key.
     * \param key The key.
     * \param separator The character to use as separator.
     * \return The associated real list value.
     */
    const vector<double> getRealListValue(const string &key, 
					  const char separator) const;
};

#endif // __resourcedata__
