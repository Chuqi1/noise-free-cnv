/*
 *      GtkCnvInterface.hh - this file is part of noise-free-cnv.
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

#ifndef _GTKCNVINTERFACE_
#define _GTKCNVINTERFACE_
#include "GtkCnvChronoChooser.hh"
#include "GtkCnvMenu.hh"
#include "GtkCnvMonitor.hh"
#include "GtkCnvOutline.hh"
#include "GtkCnvPanel.hh"
#include "GtkCnvNavigator.hh"

#include <gtkmm.h>

/* This Widget combines the Menu, ChronoChooser, Monitor, Outline, Panel and
   Navigator widget into the Interface widget that makes up the complete
   noise-free-cnv-gtk interface. */

namespace GtkCnv {

class Interface: public Gtk::HPaned
{
public:
	Interface();
private:
	ChronoChooser chrono_chooser;
	Monitor monitor;
	Outline outline;
	Menu menu;
	Panel panel;
	Navigator navi;
	Gtk::VBox box_left;
	Gtk::VBox box_right;
};

}


#endif
