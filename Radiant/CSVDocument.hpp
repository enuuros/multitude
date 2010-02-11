/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in
 * file "LGPL.txt" that is distributed with this source package or obtained
 * from the GNU organization (www.gnu.org).
 *
 */

#ifndef RADIANT_CSVDOCUMENT_HPP
#define RADIANT_CSVDOCUMENT_HPP

#include <Radiant/Export.hpp>

#include <string>
#include <list>
#include <vector>

namespace Radiant {

  /** A simple class for loading CSV documents.

      CSV documents are basically spreadsheets, with information
      placed on rows, with an agreed separator.

  */

    /// @todo make delimeter configurable, row access, document

  class RADIANT_API CSVDocument
  {
  public:

    class RADIANT_API Row : public std::vector<std::wstring>
    {
    public:
      Row() {}
    };


    CSVDocument();
    ~CSVDocument();

    /** Load a file, and return the number of lines read. */
    int load(const char * filename);

    Row * findRow(const std::wstring & key, unsigned col);

  private:

    typedef std::list<Row> Rows;
    Rows m_rows;
  };


}

#endif // CSVDOCUMENT_HPP
