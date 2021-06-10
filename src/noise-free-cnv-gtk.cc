/*
 *      noise-free-cnv-gtk.cc - this file is part of noise-free-cnv.
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

#include "GtkCnvInterface.hh"
#include "CnvOperations.hh"
#include "CnvLoadSave.hh"
#include "PennCnvLoadSave.hh"
#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include "CnvCalling.hh"

#if !(defined(NFCNV_VERSION_MAJOR)&&defined(NFCNV_VERSION_MINOR))
#include "config.h"
#endif

/* The noise-free-cnv-gtk program is the core of nosie-free-cnv and proviedes an
   interactive editor for DNA microarray data sequences in the PennCNV file
   format. All the actual logic of noise-free-cnv-gtk is outsourced to the
   Interface widget. */

int main(int Args, char** Arg)
{
	for(int i=1; i<Args; i++)
	{
		if(!strcmp(Arg[i], "-h")||!strcmp(Arg[i], "--help"))
		{
			std::cout<<"Usage: noise-free-cnv-gtk [OPTION]...\n"
				"Interactively manipulate DNA-microarray data\n"
				"\n"
				"  -h, --help      display this help and exit\n"
				"      --version   output version information and exit\n"
				"\n"
				"Report noise-free-cnv bugs to philip.development@googlemail.com\n"
				"noise-free-cnv home page: <http://noise-free-cnv.sourceforge.net>"<<std::endl;
			return 0;

		}
		else if(!strcmp(Arg[i], "--version"))
		{
			std::cout<<"noise-free-cnv-gtk "<<NFCNV_VERSION_MAJOR<<"."<<NFCNV_VERSION_MINOR<<"\n"
				"Copyright (c) 2010, 2011, 2012 Philip Ginsbach\n"
				"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
				"This is free software: you are free to change and redistribute it.\n"
				"There is NO WARRANTY, to the extent permitted by law.\n"
				"\n"
				"Written by Philip Ginsbach."<<std::endl;
			return 0;
		}
		else if(Arg[i][0]=='-')
		{
			std::cout<<"noise-free-cnv-gtk: unrecognized option \'"<<Arg[i]<<"\'\n"
				"Try \'noise-free-cnv-gtk --help\' for more information."<<std::endl;
			return 0;
		}
		else
		{
			std::cout<<"noise-free-cnv-gtk: unrecognized option \'"<<Arg[i]<<"\'\n"
				"Try \'noise-free-cnv-gtk --help\' for more information."<<std::endl;
			return 0;
		}
	}

	Gtk::Main main_obj(Args, Arg);

	Gtk::Window window_obj;

	GtkCnv::Interface interface;

	window_obj.add(interface);
	window_obj.show_all_children();

	main_obj.run(window_obj);

	return 0;
}
