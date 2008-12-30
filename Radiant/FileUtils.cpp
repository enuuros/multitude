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

#include "FileUtils.hpp"
#include "PlatformUtils.hpp"
#include "StringUtils.hpp"
#include "Directory.hpp"

#include <fcntl.h>
#include <iostream>

using namespace std;

namespace Radiant
{

  namespace FileUtils
  {

    using namespace StringUtils;

    unsigned long getFileLen(ifstream& file)
    {
      if(!file.good()) return 0;

      unsigned long pos = file.tellg();
      file.seekg(0, ios::end);
      unsigned long len = file.tellg();   
      file.seekg(pos, ios::beg);

      return len;
    }

    bool fileReadable(const char* filename)
    {
      return PlatformUtils::fileReadable(filename);
    }

    bool fileAppendable(const char* filename)
    {
      if(!fileReadable(filename))
	return false;

      FILE * f = fopen(filename, "r+");
      if(!f)
        return false;
      fclose(f);
      return true;
    }

    bool renameFile(const char * from, const char * to)
    {
      int ok = rename(from, to);
      return (ok == 0);
    }

    char* loadTextFile(const char* filename)
    {
      ifstream file;

      file.open(filename, ios::in | ios::binary);
      if(!file.good()) {
        cerr << "loadTextFile # could not open '" << filename <<
          "' for reading" << endl;
        return 0;
      }

      unsigned long len = getFileLen(file);

      if(len == 0) {
        cerr << "loadTextFile # file '" << filename << "' is empty" << endl;
        return 0;
      }

      char* contents = new char [len + 1];
      file.read(contents, len);
      contents[len] = 0;

      file.close();

      return contents;
    }

    bool writeTextFile(const char * filename, const char * contents)
    {
      int fd = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

      if(fd <= 0)
	return false;

      int len = strlen(contents);
      
      bool ok = write(fd, contents, len) == len;

      close(fd);

      return ok;
    }

    string path(const string & filepath)
    {
      int cut = filepath.rfind("/") + 1;
      return filepath.substr(0, cut);
    }

    string filename(const string & filepath)
    {
      int cut = filepath.rfind("/") + 1;
      return filepath.substr(cut);
    }

    string baseFilename(const string & filepath)
    {
      int cut1 = filepath.rfind("/") + 1;
      int cut2 = filepath.rfind(".");
      return filepath.substr(cut1, cut2);
    }

    std::string withoutSuffix(const std::string & filepath)
    {
      int cut = filepath.rfind(".");
      if(cut > 0)
	return filepath.substr(0, cut);

      return filepath;
    }


    string suffix(const string & filepath)
    {
      int cut = filepath.rfind(".") + 1;
      return filepath.substr(cut);
    }

    string findFile(const string & filename, const string & paths)
    {
      StringList pathList;
      split(paths, ";", pathList, true);

      for(StringList::iterator it = pathList.begin();
	  it != pathList.end(); it++) {
        string fullPath = (*it) + string("/") + filename;

        //trace("Testing %s for %s", (*it).c_str(), filename.c_str());

        if(fileReadable(fullPath.c_str()))
          return fullPath;
      }

      return std::string();
    }

    string findOverWritable(const string & filename, const string & paths)
    {
      StringList pathList;
      split(paths, ";", pathList, true);

      for(StringList::iterator it = pathList.begin();
	  it != pathList.end(); it++) {
        string fullPath = (*it) + string("/") + filename;

        if(fileAppendable(fullPath.c_str()))
          return fullPath;
      }

      return filename;
    }

    FILE * createFilePath(const std::string & filePath)
    {
      if(filePath.empty()) return 0;

      StringList pieces;
      split(filePath, "/", pieces, true);

      const string file(pieces.back());
      pieces.pop_back();

      string soFar("");

      for(StringList::iterator it = pieces.begin(); it != pieces.end(); it++) {
        soFar += string("/") + *it;

        if(!Directory::exists(soFar)) {
          Directory::mkdir(soFar);
        }
      }

      soFar += string("/") + file;

      return fopen(soFar.c_str(), "w");
    }

  }
}
