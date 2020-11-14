/*
 *      GtkCnvButton.cc - this file is part of noise-free-cnv.
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

#include "GtkCnvButton.hh"

#include "GtkCnvOutline.hh"
#include "CnvThreadClasses.hh"

#include <gtkmm.h>
#include <string>
#include <sstream>

/* These Widgets are used as Buttons in the GtkCnvPanel.hh file. Further
   explanation can be found there. */

namespace GtkCnv {

void MenuItemOpen1::on_activate()
{
	Gtk::FileChooserDialog openDialog("Open File",
		Gtk::FILE_CHOOSER_ACTION_OPEN);

	openDialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	openDialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	openDialog.set_select_multiple(true);

	int Response=openDialog.run();
	if(Response==Gtk::RESPONSE_OK)
	{
		std::vector<std::string> filenames=openDialog.get_filenames();
		std::vector<std::string>::iterator it;
		for(it=filenames.begin(); it!=filenames.end(); ++it)
			outline.add_object(function(*it, string_pool));
	}
}

void MenuItemOpen2::on_activate()
{
	Gtk::FileChooserDialog openDialog("Open File",
		Gtk::FILE_CHOOSER_ACTION_OPEN);

	openDialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	openDialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	openDialog.set_select_multiple(true);

	int Response=openDialog.run();
	if(Response==Gtk::RESPONSE_OK)
	{
		std::vector<std::string> filenames=openDialog.get_filenames();
		std::vector<std::string>::iterator it;
		for(it=filenames.begin(); it!=filenames.end(); ++it)
			outline.add_objects(function(*it, string_pool));
	}
}

void MenuItemSave1::on_activate()
{
	std::vector<Cnv::Thread::Sequence> select=outline.get_selection();

	Gtk::FileChooserDialog saveDialog("Save File",
		Gtk::FILE_CHOOSER_ACTION_SAVE);

	saveDialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	saveDialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	if(select.size()>0)
	{
		int Response=saveDialog.run();
		if(Response==Gtk::RESPONSE_OK)
		{
			std::string filename=saveDialog.get_filename();
			function(select[0], filename);
		}
	}
}

void MenuItemSave2::on_activate()
{
	std::vector<Cnv::Thread::Sequence> select=outline.get_selection();

	Gtk::FileChooserDialog saveDialog("Save File",
		Gtk::FILE_CHOOSER_ACTION_SAVE);

	saveDialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	saveDialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	if(select.size()>0)
	{
		int Response=saveDialog.run();
		if(Response==Gtk::RESPONSE_OK)
		{
			std::string filename=saveDialog.get_filename();
			function(select, filename);
		}
	}
}

void ButtonSingle1::on_clicked()
{
	std::vector<Cnv::Thread::Sequence> select=outline.get_selection();

	float argument=0.0;
	std::stringstream sstream;
	sstream<<(std::string)entry.get_text();
	sstream>>argument;

	std::vector<Cnv::Thread::Sequence>::iterator it;
	for(it=select.begin(); it!=select.end(); ++it)
		outline.add_object(function(*it, argument));
}

void ButtonSingle2::on_clicked()
{
	std::vector<Cnv::Thread::Sequence> select=outline.get_selection();

	std::vector<Cnv::Thread::Sequence>::iterator it;
	for(it=select.begin(); it!=select.end(); ++it)
		outline.add_object(function(*it));
}

void ButtonDual::on_clicked()
{
	std::vector<Cnv::Thread::Sequence> select=outline.get_selection();

	if(select.size()>=2)
	{
		if(outline.get_chronology()==0)
		{
			for(unsigned i=0; i<select.size()-1; ++i)
				outline.add_object(function(select[i], select.back()));
		}
		else if(select.size()%2==0&&outline.get_chronology()==1)
		{
			for(unsigned i=0; i+1<select.size(); i+=2)
				outline.add_object(function(select[i], select[i+1]));
		}
		else if(select.size()%2==0&&outline.get_chronology()==2)
		{
			for(unsigned i=0; i<select.size()/2; ++i)
				outline.add_object(function(select[i],
					select[i+select.size()/2]));
		}
		else if(outline.get_chronology()==3)
		{
			for(unsigned i=1; i<select.size(); ++i)
				outline.add_object(function(select[i], select.front()));
		}
		else if(outline.get_chronology()==4)
		{
			for(unsigned i=0; i+1<select.size(); i+=2)
				outline.add_object(function(select[i+1], select[i]));
		}
		else if(outline.get_chronology()==5)
		{
			for(unsigned i=1; i<select.size(); ++i)
				outline.add_object(function(select[i+select.size()/2],
					select[i]));
		}
	}
}

void ButtonMulti1::on_clicked()
{
	std::vector<Cnv::Thread::Sequence> select=outline.get_selection();
	if(select.size()>=2)
		outline.add_object(function(select));
}

void ButtonMulti2::on_clicked()
{
	std::vector<Cnv::Thread::Sequence> select=outline.get_selection();
	if(select.size()>=2)
		outline.add_objects(function(select));
}

}

