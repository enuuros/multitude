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
#include "Directory.hpp"
#include "StringUtils.hpp"
#include "FileUtils.hpp"

#ifdef WIN32
#	include <windows.h>
#endif

#include <boost/filesystem.hpp>
#include <iostream>

#define DOT		std::string(".")
#define DOTDOT	std::string("..")

namespace fs = boost::filesystem;

namespace Radiant
{

	static bool applyFilters(const fs::directory_iterator & it, int filterFlags, const std::vector<std::string> & suffixes) 
	{
#ifdef WIN32
		const std::string name = it->filename();
#else
		const std::string name = it->leaf();
#endif

		if(fs::is_directory(it->status()) && !(filterFlags & Directory::Dirs)) return false;
		if(fs::is_regular(it->status()) && !(filterFlags & Directory::Files)) return false;
		if((name == DOT || name == DOTDOT) && (filterFlags & Directory::NoDotAndDotDot)) return false;


		if(!suffixes.empty()) {
			bool ok = false;

			std::string suffix = StringUtils::lowerCase(FileUtils::suffix(name));			

			for(unsigned i = 0; i < suffixes.size(); i++) {
				if(suffix == suffixes[i]) {
					ok = true;
					break;
				}
			}
			if(!ok) return false;
		}

		return true;		
	}

	bool Directory::mkdir(const char * dirname)
	{
		return fs::create_directory(dirname);	
	}

	bool Directory::mkdir(const std::string & dirname)
	{
		return fs::create_directory(dirname);	
	}

	void Directory::populate()
	{
		fs::path path(m_path, fs::native);

		fs::directory_iterator end;
		for(fs::directory_iterator it(path); it != end; it++) {

			if(applyFilters(it, m_filterFlags, m_suffixes))
#ifdef WIN32
				m_entries.push_back(it->filename());
#else
				m_entries.push_back(it->leaf());
#endif
		}

		if(m_sortFlags == Name)
			std::sort(m_entries.begin(), m_entries.end());
	}

  bool Directory::exists(const std::string & filename)
  {
    return fs::exists(filename);
  }

}

