/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef RADIANT_FILEUTILS_HPP
#define RADIANT_FILEUTILS_HPP

#include <fstream>

#include <Radiant/Export.hpp>

namespace Radiant
{

  /// FileUtils contains functions for platform independent file-handing
  namespace FileUtils
  {

    /// Get the size of a file
    MTEXPORT unsigned long getFileLen(std::ifstream& file);
    /** Load a text file. The contens of the file are returned as a
     * zero-terminated string. 
     */ 
    MTEXPORT char* loadTextFile(const char* filename);
    /// Check if a given file is readable
    MTEXPORT bool fileReadable(const char* filename);
    /// Check if the user can append to a given file
    /** This function is useful if you want to overwrite a file, and
	want to check beforehand, that it is possible. 
	
	@return Returns true if the file exists and can be written
	to. Otherwise returns false.
    */
    bool fileAppendable(const char* filename);

    /// Rename a file
    MTEXPORT bool renameFile(const char * from, const char * to);

    /// Extract path
    MTEXPORT std::string path(const std::string & filepath);
    /// Extract filename
    MTEXPORT std::string filename(const std::string & filepath);
    /// Extract filename without suffix
    MTEXPORT std::string baseFilename(const std::string & filepath);
    /// Extract suffix
    MTEXPORT std::string suffix(const std::string & filepath);

    /** Find a file given a list of paths to search. The directory names are
     * separated by colon or semicolon in typical Windows or UNIX fashion
     * (/usr/foo:/home/user/foo etc.).
     */
    MTEXPORT std::string findFile(const std::string & filename, const std::string & paths);
    // const std::string & paths);
    /** Tries to find a file that could be over-written. If such
	cannot be found, then returns filename. */
    std::string findOverWritable(const std::string & filename,
				 const std::string & paths);

  }
}

#endif
