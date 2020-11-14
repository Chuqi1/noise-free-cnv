/*
 *      PennCnvLoadSave.cc - this File is part of noise-free-cnv.
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

#include "PennCnvLoadSave.hh"

#include "CnvSequence.hh"
#include "CnvStringPool.hh"
#include "CnvEncodeDecode.hh"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

/* This file implements the load and save functionality for data sequences
   in PennCNV files. The corresponding function for loading native files can be
   found in the PennCnvLoadSave.hh file.

   In addition to the filename, the load function receives a StringPool object
   reference which it uses to minimize memory footprint. */

namespace PennCnv {

Point::Point(Cnv::StringPointer n, unsigned char c, unsigned p, float l, float b)
	:name(n),chr(c),pos(p),lrr(l),baf(b)
	{}

bool Point::operator<(const Point& l) const
{
	return chr<l.chr||(chr==l.chr&&(pos<l.pos
		||(pos==l.pos&&name<l.name)));
}

std::vector<Cnv::Sequence> load(std::string f, Cnv::StringPool& pool)
{
	std::ifstream ifs(f.c_str());

	std::string tabCode;

	std::string line;
	std::getline(ifs, line);

	size_t tab_start=0;
	while(tab_start<line.size())
	{
		size_t tab_end=line.find('\t', tab_start);
		if(tab_end>line.size()) tab_end=line.find('\r', tab_start);
		if(tab_end>line.size()) tab_end=line.size();

		std::string caption=line.substr(tab_start, tab_end-tab_start);
		if(caption=="Name") tabCode.push_back('N');
		else if(caption=="Chr") tabCode.push_back('C');
		else if(caption=="Position") tabCode.push_back('P');
		else if(caption.find(".Log R Ratio")<caption.size())
			tabCode.push_back('L');
		else if(caption.find(".B Allele Freq")<caption.size())
			tabCode.push_back('B');
		else tabCode.push_back(' ');

		if(tab_end==line.size()) tab_start=line.size();
		else tab_start=tab_end+1;
	}

	if(tabCode.find('N')<tabCode.size()
		&&tabCode.find('C')<tabCode.size()
		&&tabCode.find('P')<tabCode.size()
		&&tabCode.find('L')<tabCode.size()
		&&tabCode.find('B')<tabCode.size())
	{
		std::vector<Point> samples;

		while(std::getline(ifs, line))
		{
			size_t id_start=0, id_size=0, chr_start=0, chr_size=0,
				pos_start=0, pos_size=0, lrr_start=0, lrr_size=0,
				baf_start=0, baf_size=0;

			size_t tab_start=0;
			std::string::iterator it;
			for(it=tabCode.begin(); it!=tabCode.end(); ++it)
			{
				size_t tab_end=line.find('\t', tab_start);
				if(tab_end>line.size()) tab_end=line.find('\r', tab_start);
				if(tab_end>line.size()) tab_end=line.size();

				if(*it=='N')
					{ id_start=tab_start; id_size=tab_end-tab_start; }
				else if(*it=='C')
					{ chr_start=tab_start; chr_size=tab_end-tab_start; }
				else if(*it=='P')
					{ pos_start=tab_start; pos_size=tab_end-tab_start; }
				else if(*it=='L')
					{ lrr_start=tab_start; lrr_size=tab_end-tab_start; }
				else if(*it=='B')
					{ baf_start=tab_start; baf_size=tab_end-tab_start; }

				if(tab_end==line.size()) tab_start=line.size();
				else tab_start=tab_end+1;
			}

			while(id_size>0&&line[id_start]==' ')
				{ id_start++; id_size--; }
			while(id_size>0&&line[id_start+id_size-1]==' ')
				id_size--;

			while(chr_size>0&&line[chr_start]==' ')
				{ chr_start++; chr_size--; }
			while(chr_size>0&&line[chr_start+chr_size-1]==' ')
				chr_size--;

			while(pos_size>0&&line[pos_start]==' ')
				{ pos_start++; pos_size--; }
			while(pos_size>0&&line[pos_start+pos_size-1]==' ')
				pos_size--;

			while(lrr_size>0&&line[lrr_start]==' ')
				{ lrr_start++; lrr_size--; }
			while(lrr_size>0&&line[lrr_start+lrr_size-1]==' ')
				lrr_size--;

			while(baf_size>0&&line[baf_start]==' ')
				{ baf_start++; baf_size--; }
			while(baf_size>0&&line[baf_start+baf_size-1]==' ')
				baf_size--;

			std::string id_long;
			id_long.resize(id_size+chr_size+pos_size+2);
			for(size_t i=0; i<id_size; ++i)
				id_long[i]=line[id_start+i];
			for(size_t i=0; i<chr_size; ++i)
				id_long[id_size+1+i]=line[chr_start+i];
			for(size_t i=0; i<pos_size; ++i)
				id_long[id_size+chr_size+2+i]=line[pos_start+i];
			id_long[id_size]='/';
			id_long[id_size+chr_size+1]='/';

			samples.push_back(Point(pool(id_long),
				Cnv::decode_chr(line.begin()+chr_start,
					line.begin()+chr_start+chr_size),
				Cnv::decode_pos(line.begin()+pos_start,
					line.begin()+pos_start+pos_size),
				Cnv::decode_float_value(line.begin()+lrr_start,
					line.begin()+lrr_start+lrr_size),
				Cnv::decode_float_value(line.begin()+baf_start,
					line.begin()+baf_start+baf_size)));
		}

		std::sort(samples.begin(), samples.end());

		std::vector<Cnv::Sequence> out;
		out.resize(2);

		out[0].reserve(samples.size());
		out[1].reserve(samples.size());

		std::vector<Point>::iterator it;
		for(it=samples.begin(); it!=samples.end(); ++it)
		{
			out[0].push_back(it->name, it->lrr);
			out[1].push_back(it->name, it->baf);
		}
		return out;
	}
	else
	{
		std::vector<Cnv::Sequence> dummy;
		dummy.resize(2);
		return dummy;
	}
}

void save(const std::vector<Cnv::Sequence>& o, std::string f)
{
	std::ofstream ofs(f.c_str());
	if(o.size()>=2)
	{
		std::string name=f;
		if(name.rfind('/')<name.size())
			name=name.substr(name.rfind('/')+1, std::string::npos);
		if(name.rfind('\\')<name.size())
			name=name.substr(name.rfind('\\')+1, std::string::npos);

		ofs<<"Name\tChr\tPosition\t"
			<<name<<".Log R Ratio\t"<<name<<".B Allele Freq\r\n";

		for(Cnv::SequenceDualIterator merge(o[0], o[1]); merge; ++merge)
		{
			std::string short_id, chr, pos;
			Cnv::decompose_point_name(merge.name(), short_id, chr, pos);

			ofs<<short_id<<"\t"<<chr<<"\t"<<pos<<"\t"
				<<Cnv::encode_float_value(merge.first())<<"\t"
				<<Cnv::encode_float_value(merge.second())<<"\r\n";
		}
	}
}

}
