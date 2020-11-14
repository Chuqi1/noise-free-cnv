/*
 *      CnvStringPool.hh - this file is part of noise-free-cnv.
 *
 *      Copyright 2010-2013 Philip Ginsbach <philip.develop@gmail.com>
 *
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CNVSTRINGPOOL_
#define _CNVSTRINGPOOL_
#include <string>
#include <set>
#include <map>
#include <glibmm.h>

/* The StringPool class is used to reduce the memory footprint of
   noise-free-cnv. It ensures that several data sequences with the same data
   point identifiers ( but different data point values ) do not save the
   indentifiers seperately but only store pointers to the actual strings.
   There pointers are represented by the StringPointer class.

   StringPool can be thought of as an optimized std::set<std::string>. */

namespace Cnv {

//	This class encapsules a pointer to a std::string.
class StringPointer
{
public:
	StringPointer();
	StringPointer(const std::string* s);

	operator const std::string&() const;

	operator bool() const;

	bool operator<(const StringPointer& s) const;
	bool operator>(const StringPointer& s) const;
	bool operator<=(const StringPointer& s) const;
	bool operator>=(const StringPointer& s) const;
	bool operator==(const StringPointer& s) const;
	bool operator!=(const StringPointer& s) const;

private:
	static std::string null_ptr;
	const std::string* ptr;
};

//	This class stores std::strings. It can be used to reduce memory
//	usage to avoid storing the same string multiple times.
class StringPool
{
public:
	StringPointer operator()(const std::string& s);

private:
	std::map<float,std::set<std::string> > data;
};

}

#endif

