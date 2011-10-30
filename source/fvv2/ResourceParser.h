#ifndef __resourceparser__
#define __resourceparser__

#include <fstream>
#include <string>
#include <vector>

class ResourceData;

using namespace std;

/*!
 * \class ResourceParser
 * \author Jeferson Rodrigues da Silva
 * \date 09/21/2008
 * \file ResourceParser.h
 * \brief Parser for resource files.
 */
class ResourceParser {
private:
    /*!
     * \brief Removes all leading and trailing spaces from a string.
     * \param s String whose spaces are to be removed.
     * \return The modified string.
     */
    string trimSpaces(const string &s) const;

    /*!
     * \brief Reads all lines from a resource file removing all comment and
     *        blank lines.
     * \param file The resource file stream.
     * \return A list containing all remaining lines.
     */
    vector<string> getDataLines(ifstream &file) const;

    /*!
     * \brief Reads the all key->value pairs from the resource file.
     * \param file The resource file stream.
     * \return The resource data or a null pointer if no valid data
     *         was found.
     */
    ResourceData *readResourceData(ifstream &file) const;

public:
    /*!
     * \brief Constructor.
     */
    ResourceParser();

    /*!
     * \brief Destructor.
     */
    ~ResourceParser();

    /*!
     * \brief Parses resource data from a resource file.
     * \param filename The filename of the resource file.
     * \return Pointer to the parsed resource data or a null pointer if the
     *         file couldn't be opened.
     */
    ResourceData *parseDataFromFile(const string filename) const;
};

#endif // __resourceparser__
