/*
 *      GtkCnvNavigator.hh - this file is part of noise-free-cnv.
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

#ifndef _GTKCNVNAVIGATOR_
#define _GTKCNVNAVIGATOR_
#include "GtkCnvMonitor.hh"

#include <gtkmm.h>

/* This widget is used to display the current position in the data sequence of
   a Monitor widget. It is used for the noise-free-cnv-gtk interface. */

namespace GtkCnv {

class ButtonNavigator: public Gtk::Button
{
public:
	ButtonNavigator(Gtk::Entry& e, Monitor& m);

private:
	Gtk::Entry& entry;
	Monitor& monitor;

protected:
	void on_clicked();
};

class Navigator: public Gtk::HBox
{
public:
	Navigator(Monitor& m);
private:
	Gtk::Entry entry;
	ButtonNavigator button;
};

}


#endif
