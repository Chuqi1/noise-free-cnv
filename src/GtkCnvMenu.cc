/*
 *      GtkCnvMenu.cc - this file is part of noise-free-cnv.
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

#include "GtkCnvMenu.hh"

#include "GtkCnvOutline.hh"
#include "CnvThreadOperations.hh"

#if !(defined(NFCNV_VERSION_MAJOR)&&defined(NFCNV_VERSION_MINOR))
#include "config.h"
#endif

#include <gtkmm.h>

/* The Menu widget is used in the noise-free-cnv-gtk interface as the main menu.
   It consists of buttons to load and save data sequences, to show the "about"
   dialogue and to remove data sequences from the catalogue. */

namespace GtkCnv {

Menu::Menu(Outline& o): outline(o),
	open_button(Gtk::Stock::OPEN), save_button(Gtk::Stock::SAVE),
	close_button(Gtk::Stock::CLOSE), info_button(Gtk::Stock::INFO),
	open_namesvalues_item("raw file format",
		outline, string_pool, Cnv::Thread::load_namesvalues),
	open_lrrbaf_item("PennCNV file format",
		outline, string_pool, Cnv::Thread::load_lrrbaf),
	save_namesvalues_item("raw file format",
		outline, Cnv::Thread::save_namesvalues),
	save_lrrbaf_item("PennCNV file format",
		outline, Cnv::Thread::save_lrrbaf)
{
	pack_start(open_button);
	pack_start(save_button);
	pack_start(close_button);
	pack_start(info_button);

	open_menu.append(open_namesvalues_item);
	open_menu.append(open_lrrbaf_item);
	save_menu.append(save_namesvalues_item);
	save_menu.append(save_lrrbaf_item);

	open_menu.show_all_children();
	save_menu.show_all_children();

	open_button.signal_clicked().connect(
		sigc::mem_fun(*this, &Menu::on_button_open));
	save_button.signal_clicked().connect(
		sigc::mem_fun(*this, &Menu::on_button_save));
	close_button.signal_clicked().connect(
		sigc::mem_fun(*this, &Menu::on_button_close));
	info_button.signal_clicked().connect(
		sigc::mem_fun(*this, &Menu::on_button_info));
}

void Menu::on_button_open() { open_menu.popup(0, 0); }
void Menu::on_button_save() { save_menu.popup(0, 0); }
void Menu::on_button_close() { outline.close(); }

void Menu::on_button_info()
{
	std::string version_string;
	std::stringstream sstream;
	sstream<<NFCNV_VERSION_MAJOR<<"."<<NFCNV_VERSION_MINOR;
	sstream>>version_string;

	Gtk::AboutDialog dialog;
	dialog.set_version(version_string);
	dialog.set_comments(
		"program for analyzing and manipulating DNA microarray data\n"
		"written in C++, using Gtkmm and FFTW3");
	dialog.set_copyright("Copyright (c) 2010-2020 Philip Ginsbach");
	dialog.set_website("https://ginsbach.github.io/noise-free-cnv");
	std::vector<Glib::ustring> authors;
	authors.push_back("Philip Ginsbach <ginsbach@github.com>");
	dialog.set_authors(authors);
	dialog.run();
}

}


