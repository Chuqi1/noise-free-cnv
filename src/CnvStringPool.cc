/*
 *      CnvStringPool.cc - this file is part of noise-free-cnv.
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

#include "CnvStringPool.hh"

#include <string>

/* The StringPool class is used to reduce the memory footprint of
   noise-free-cnv. It ensures that several data sequences with the same data
   point identifiers ( but different data point values ) do not save the
   indentifiers seperately but only store pointers to the actual strings.
   There pointers are represented by the StringPointer class.

   StringPool can be thought of as an optimized std::set<std::string>. */

namespace Cnv {

StringPointer::StringPointer(): ptr(&null_ptr) {};

StringPointer::StringPointer(const std::string* s): ptr(s) {};

StringPointer::operator const std::string&() const { return *ptr; }

StringPointer::operator bool() const { return ptr!=&null_ptr; }

bool StringPointer::operator<(const StringPointer& s) const
{
	return s.ptr!=&null_ptr&&(ptr==&null_ptr||*ptr<*s.ptr);
};

bool StringPointer::operator>(const StringPointer& s) const
	{ return s<*this; };

bool StringPointer::operator<=(const StringPointer& s) const
	{ return !(*this>s); }

bool StringPointer::operator>=(const StringPointer& s) const
	{ return !(*this<s); }

bool StringPointer::operator==(const StringPointer& s) const
{
	return ptr==s.ptr||(ptr!=&null_ptr&&s.ptr!=&null_ptr&&*ptr==*s.ptr);

}
bool StringPointer::operator!=(const StringPointer& s) const
	{ return !(*this==s); }

std::string StringPointer::null_ptr="";


StringPointer StringPool::operator()(const std::string& s)
{
	float hash=0.0;
	if(s.size()>=sizeof(float))
		hash=*(float*)(s.c_str()+(s.size()-sizeof(float))/2);
	StringPointer out(&*(data[hash].insert(s).first));
	return out;
}

}
