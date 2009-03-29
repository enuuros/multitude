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
#include <sstream>
#include <string.h>

#ifdef WIN32
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

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

    std::wstring readTextFile(const std::string & filename)
    {
        std::wostringstream oss;
        wifstream file(filename.c_str());

        if(file.is_open()) {

        wstring line;

        while(getline(file, line))        
            oss << line << wchar_t(0x200B); // W_NEWLINE
        
        file.close();
        }

        return oss.str();
    }

    bool writeTextFile(const char * filename, const char * contents)
    {
#ifdef WIN32
		int fd = _creat(filename, _S_IWRITE);
#else
      int fd = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
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

      info("baseFilename %s %d %d", filepath.c_str(), cut1, cut2);
      return (cut1 > 0) ?
	filepath.substr(cut1, cut2 - cut1) : filepath.substr(0, cut2);
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

    bool suffixMatch(const std::string & filename,
			    const std::string & suf)
    {
      string s = suffix(filename);
      return StringUtils::lowerCase(s) == StringUtils::lowerCase(suf);
    }

    string findFile(const string & filename, const string & paths)
    {
      StringList pathList;
      split(paths, ";", pathList, true);

      for(StringList::iterator it = pathList.begin();
	  it != pathList.end(); it++) {
        string fullPath = (*it) + string("/") + filename;

		debug("Radiant::findFile # Testing %s for %s", (*it).c_str(), filename.c_str());

        if(fileReadable(fullPath.c_str())) {
			debug("Radiant::findFile # FOUND %s", fullPath.c_str());
          return fullPath;
        }
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

    bool looksLikeImage(const std::string & filePath)
    {
      return suffixMatch(filePath, "png") ||
	suffixMatch(filePath, "jpg") ||
	suffixMatch(filePath, "jpeg");
    }
    
    bool looksLikeVideo(const std::string & filePath)
    {
      return suffixMatch(filePath, "avi") ||
	suffixMatch(filePath, "qt") ||
	suffixMatch(filePath, "mov");
    }

  }


}
