/*
 *      PennCnvLoadSave.hh - this File is part of noise-free-cnv.
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

#ifndef _PENNCNVLOADSAVE_
#define _PENNCNVLOADSAVE_
#include "CnvSequence.hh"
#include "CnvStringPool.hh"
#include <vector>

/* This file implements the load and save functionality for data sequences
   in PennCNV files. The corresponding function for loading native files can be
   found in the PennCnvLoadSave.hh file.

   In addition to the filename, the load function receives a StringPool object
   reference which it uses to minimize memory footprint. */

namespace PennCnv {

class Point
{
public:

	Point(Cnv::StringPointer n, unsigned char c, unsigned p, float l, float b);

	bool operator<(const Point& l) const;

	Cnv::StringPointer name;
	unsigned chr;
	unsigned pos;
	float lrr, baf;
};

std::vector<Cnv::Sequence> load(std::string f, Cnv::StringPool& pool);

void save(const std::vector<Cnv::Sequence>& o, std::string f);

}
#endif
