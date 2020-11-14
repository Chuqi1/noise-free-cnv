/*
 *      GtkCnvMenu.hh - this file is part of noise-free-cnv.
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

#ifndef _GTKCNVMENU_
#define _GTKCNVMENU_
#include "GtkCnvButton.hh"
#include "GtkCnvOutline.hh"
#include "CnvThreadClasses.hh"

#include <gtkmm.h>

/* The Menu widget is used in the noise-free-cnv-gtk interface as the main menu.
   It consists of buttons to load and save data sequences, to show the "about"
   dialogue and to remove data sequences from the catalogue. */

namespace GtkCnv {

class Menu: public Gtk::HBox
{
public:
	Menu(Outline& o);

private:
	void on_button_open();
	void on_button_save();
	void on_button_close();
	void on_button_info();

	Outline& outline;
	Cnv::Thread::StringPool string_pool;

	Gtk::ToolButton open_button;
	Gtk::ToolButton save_button;
	Gtk::ToolButton close_button;
	Gtk::ToolButton info_button;

	Gtk::Menu open_menu;
	Gtk::Menu save_menu;
	MenuItemOpen1 open_namesvalues_item;
	MenuItemOpen2 open_lrrbaf_item;
	MenuItemSave1 save_namesvalues_item;
	MenuItemSave2 save_lrrbaf_item;
};

}

#endif
