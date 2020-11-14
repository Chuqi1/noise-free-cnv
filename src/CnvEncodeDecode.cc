/*
 *      CnvEncodeDecode.cc - this File is part of noise-free-cnv.
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

#include "CnvEncodeDecode.hh"
#include <string>
#include <cmath>
#include <climits>

/* This file is used for encoding data into character strings and to decode
   character strings of that kind. */

namespace Cnv {

unsigned char decode_chr(std::string::const_iterator i,
	std::string::const_iterator end)
{
	if(end-i==1&&i[0]>='0'&&i[0]<='9')
		return (unsigned char)(i[0]-'0');
	else if(end-i==2&&i[0]>='1'&&i[0]<='9'&&i[1]>='0'&&i[1]<='9')
		return (unsigned char)(i[1]-'0')+10*(unsigned char)(i[0]-'0');
	else if(end-i==3&&i[0]>='1'&&i[0]<='2'&&((i[1]=='5'&&i[2]=='0')
		||(i[1]>='0'&&i[1]<='4'&&i[2]>='0'&&i[2]<='9')))
	{
		return (unsigned char)(i[2]-'0')+10*(unsigned char)(i[1]-'0')
			+100*(unsigned char)(i[0]-'0');
	}
	else if(end-i==1&&(i[0]=='x'||i[0]=='X')) return UCHAR_MAX-4;
	else if(end-i==1&&(i[0]=='y'||i[0]=='Y')) return UCHAR_MAX-3;
	else if(end-i==2&&(i[0]=='x'||i[0]=='X')
		&&(i[1]=='y'||i[1]=='Y')) return UCHAR_MAX-2;
	else if(end-i==2&&(i[0]=='m'||i[0]=='M')
		&&(i[1]=='t'||i[1]=='T')) return UCHAR_MAX-1;
	else return UCHAR_MAX;
}

unsigned char decode_chr(const std::string& s)
{
	return decode_chr(s.begin(), s.end());
}

std::string encode_chr(unsigned char c)
{
	if(c<251)
	{
		std::string result;
		if(c>=100) result.push_back('0'+c/100);
		if(c>=10) result.push_back('0'+(c%100)/10);
		result.push_back('0'+c%10);
		return result;
	}
	if(c==UCHAR_MAX-4) return "X";
	else if(c==UCHAR_MAX-3) return "Y";
	else if(c==UCHAR_MAX-2) return "XY";
	else if(c==UCHAR_MAX-1) return "Mt";
	else return "?";
}

unsigned decode_pos(std::string::const_iterator i,
	std::string::const_iterator end)
{
	unsigned result=0;
	while(i!=end&&*i>='0'&&*i<='9')
		result=result*10+(unsigned)(*(i++)-'0');

	if(i==end) return result;
	else return 0;
}

unsigned decode_pos(const std::string& s)
{
	return decode_pos(s.begin(), s.end());
}

std::string encode_pos(unsigned p)
{
	std::string out;
	unsigned factor=1;
	while(p/factor>=10) factor*=10;
	for(; factor>0; factor/=10)
	{
		out.push_back('0'+p/factor);
		p=p%factor;
	}
	return out;
}

float decode_float_value(std::string::const_iterator i,
	std::string::const_iterator end)
{
	float sign=1.0;
	if(i!=end&&*i=='+') ++i;
	else if(i!=end&&*i=='-') { sign=-1.0; ++i; }

	float first_number=0.0;
	while(i!=end&&*i>='0'&&*i<='9')
		first_number=first_number*10.0+(float)(*(i++)-'0');

	if(i==end) return sign*first_number;
	else if(*i=='.'||*i==',')
	{
		--end;
		float last_number=0.0;
		std::string::const_reverse_iterator jt;
		while(*end>='0'&&*end<='9')
			last_number=(last_number+(float)(*(end--)-'0'))/10.0;

		if(i==end)
			return sign*(first_number+last_number);
		else
		{
			float exp_base=10.0;
			if(*end=='+') --end;
			else if(*end=='-') { sign=0.1; --end; }

			if(*end=='e'||*end=='E')
			{
				--end;
				float middle_number=0.0;
				std::string::const_reverse_iterator jt;
				while(*end>='0'&&*end<='9')
					middle_number=(middle_number+(float)(*(end--)-'0'))/10.0;

				if(i==end)
					return sign*(first_number+middle_number)*pow(exp_base, last_number);
				else return 0.0/0.0;
			}
			else return 0.0/0.0;
		}
	}
	else return 0.0/0.0;
}

float decode_float_value(const std::string s)
{
	return decode_float_value(s.begin(), s.end());
}

std::string encode_float_value(float v)
{
	if(std::isnan(v)||std::isinf(v)) return "NaN";
	else
	{
		std::string out;
		if(v<0.0) out.push_back('-');
		v=fabs(v)+0.00005;

		float factor=1.0;
		while(v/factor>=10.0) factor*=10.0;
		for(; factor>0.5; factor/=10.0)
		{
			out.push_back('0'+(char)(v/factor));
			v-=(float)(unsigned)(v/factor)*factor;
		}
		if(v>0.0001) out.push_back('.');

		for(; v>0.0001; factor/=10.0)
		{
			out.push_back('0'+(char)(v/factor));
			v-=(float)(unsigned)(v/factor)*factor;
		}
		return out;
	}
}

std::string compose_point_name(const std::string& id,
	const std::string& c, const std::string& p)
{
	return id+"/"+c+"/"+p;
}

std::string compose_point_name(const std::string& n,
	unsigned char chr, unsigned pos)
{
	return compose_point_name(n,encode_chr(chr), encode_pos(pos));
}

void decompose_point_name(const std::string& n, std::string& id,
	std::string& chr, std::string& pos)
{
	size_t found_start=n.find("/");
	if(found_start<n.size())
	{
		id=n.substr(0, found_start-0);
		found_start+=1;
	}
	else { found_start=n.size(); }

	size_t found_end=n.find("/", found_start);
	if(found_end<n.size())
	{
		chr=n.substr(found_start, found_end-found_start);
		found_start=found_end+1;
	}
	else { found_start=n.size(); }

	if(found_start<n.size())
		pos=n.substr(found_start, n.size()-found_start);
}

void decompose_point_name(const std::string& n, std::string& id,
	unsigned char& chr, unsigned& pos)
{
	size_t found_start=n.find("/");
	if(found_start<n.size())
	{
		id=n.substr(0, found_start-0);
		found_start+=1;
	}
	else { found_start=n.size(); }

	size_t found_end=n.find("/", found_start);
	if(found_end<n.size())
	{
		chr=decode_chr(n.begin()+found_start, n.begin()+found_end);
		found_start=found_end+1;
	}
	else { found_start=n.size(); }

	if(found_start<n.size())
		pos=decode_pos(n.begin()+found_start, n.end());
}

}
