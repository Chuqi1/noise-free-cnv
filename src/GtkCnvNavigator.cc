/*
 *      GtkCnvNavigator.cc - this file is part of noise-free-cnv.
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

#include "GtkCnvNavigator.hh"

#include "GtkCnvMonitor.hh"
#include "CnvEncodeDecode.hh"
#include <iostream>
#include <climits>

/* This widget is used to display the current position in the data sequence of
   a Monitor widget. It is used for the noise-free-cnv-gtk interface. */

namespace GtkCnv {

ButtonNavigator::ButtonNavigator(Gtk::Entry& e, Monitor& m):
	Gtk::Button("search"),entry(e),monitor(m) {}

void ButtonNavigator::on_clicked()
{
	std::string str;
	str=(std::string)entry.get_text();

	std::map<std::string,std::string> replace;
	replace["CHR"]=replace["Chr"]=replace["chr"]="";
	replace["POS"]=replace["Pos"]=replace["pos"]=":";
	replace["BEGIN"]=replace["Begin"]=replace["begin"]="0";
	replace["START"]=replace["Start"]=replace["start"]="0";
	replace["FIRST"]=replace["First"]=replace["first"]="0";
	replace["END"]=replace["End"]=replace["end"]="4294967295";
	replace["LAST"]=replace["Last"]=replace["last"]="4294967295";
	replace["to"]="-";
	replace["."]=replace[","]="";
	replace[" "]=replace["\t"]=replace["\n"]="";

	std::map<std::string,std::string>::const_iterator it;
	for(it=replace.begin(); it!=replace.end(); ++it)
	{
		size_t pos=0;
		while((pos=str.find(it->first, pos))!=std::string::npos)
		{
			str.replace(pos, it->first.length(), it->second);
			pos+=it->second.length();
		}
	}

	size_t first_colon=str.find(':');
	size_t second_colon=str.find(':', first_colon+1);
	size_t devision=str.find('-');

	unsigned char start_chr=0, end_chr=UCHAR_MAX;
	unsigned start_pos=0, end_pos=UINT_MAX;

	if(first_colon!=std::string::npos&&devision!=std::string::npos
		&&second_colon!=std::string::npos)
	{
		start_chr=Cnv::decode_chr(str.begin(), str.begin()+first_colon);
		start_pos=Cnv::decode_pos(str.begin()+first_colon+1,
			str.begin()+devision);
		end_chr=Cnv::decode_chr(str.begin()+devision+1,
			str.begin()+second_colon);
		end_pos=Cnv::decode_pos(str.begin()+second_colon+1, str.end());
	}
	else if(first_colon!=std::string::npos
		&&devision!=std::string::npos)
	{
		start_chr=Cnv::decode_chr(str.begin(), str.begin()+first_colon);
		start_pos=Cnv::decode_pos(str.begin()+first_colon+1,
			str.begin()+devision);
		end_chr=start_chr;
		end_pos=Cnv::decode_pos(str.begin()+devision+1, str.end());
	}
	else if(devision!=std::string::npos
		&&second_colon!=std::string::npos)
	{
		start_chr=Cnv::decode_chr(str.begin(), str.begin()+devision);
		start_pos=0;
		end_chr=Cnv::decode_chr(str.begin()+devision+1,
			str.begin()+second_colon);
		end_pos=Cnv::decode_pos(str.begin()+second_colon+1, str.end());
	}
	else if(devision!=std::string::npos)
	{
		start_chr=Cnv::decode_chr(str.begin(), str.begin()+devision);
		start_pos=0;
		end_chr=Cnv::decode_chr(str.begin()+devision+1, str.end());
		end_pos=UINT_MAX;
	}
	else
	{
		start_chr=Cnv::decode_chr(str.begin(), str.end());
		start_pos=0;
		end_chr=start_chr;
		end_pos=UINT_MAX;
	}

	monitor.set_boundary(start_chr, start_pos, end_chr, end_pos);
}

Navigator::Navigator(Monitor& m): button(entry,m)
{
	pack_start(entry, true, true, 0);
	pack_start(button, false, false, 0);
}

}
