/*
 *      CnvLoadSave.cc - this File is part of noise-free-cnv.
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

#include "CnvLoadSave.hh"

#include "CnvSequence.hh"
#include "CnvStringPool.hh"
#include "CnvEncodeDecode.hh"

#include <cmath>
#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <algorithm>

/* This file defines the fundamental load and save routines for data sequences.
   Both the load and the save function make use of the native file format
   exclusively. The corresponding function for loading PennCNV files are
   outsourced to the PennCnvLoadSave.hh file.

   In addition to the filename, the load function receives a StringPool object
   reference which it uses to minimize memory footprint. */

namespace Cnv {

Sequence load(std::string f, StringPool& pool)
{
	Sequence out;
	std::ifstream ifs(f.c_str());

	std::string line;
	while(std::getline(ifs, line))
	{
		size_t id_start=0, id_end=0, value_start=0, value_end=0;

		while(id_start<line.size()&&(line[id_start]==' '
			||line[id_start]=='\t')) id_start++;

		id_end=id_start;
		while(id_end<line.size()&&line[id_end]!=' '
			&&line[id_end]!='\t') id_end++;

		value_start=id_end;
		while(value_start<line.size()&&(line[value_start]==' '
			||line[value_start]=='\t')) value_start++;

		value_end=value_start;
		while(value_end<line.size()&&line[value_end]!=' '
			&&line[value_end]!='\t') value_end++;


		std::string name=line.substr(id_start, id_end-id_start);

		float value=decode_float_value(
			line.begin()+value_start, line.begin()+value_end);
		if(name.size()>0) out.push_back(pool(name), value);
	}
	return out;
}

void save(const Sequence& s, std::string f)
{
	std::ofstream ofs(f.c_str());

	for(SequenceSingleIterator iter(s); iter; ++iter)
		ofs<<(std::string)iter.name()<<"\t"<<encode_float_value(iter.value())<<"\n";
}

}
