/*
 *      GtkCnvChronoChooser.hh - this file is part of noise-free-cnv.
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

#ifndef _GTKCNVCHRONOCHOOSER_
#define _GTKCNVCHRONOCHOOSER_

#if !(defined(NFCNV_SHARE_FOLDER))
#include "config.h"
#endif

#include <gtkmm.h>

/* The ChronoChooser widget is used in the noise-free-cnv-gtk interface below
   the main menu. It defines a drop down menu which allows to choose how
   noise-free-cnv-gtk handles calls to operations that take to arguments if
   more than two data sequences are selected. */

namespace GtkCnv {

class ChronoChooser: public Gtk::ComboBox
{
public:

	ChronoChooser();
	unsigned get_chronology() const;

private:

	class ModelColumns:  public Gtk::TreeModelColumnRecord
	{
	public:

		ModelColumns() { add(m_col_pixbuf); }

		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > m_col_pixbuf;
	};

	Gtk::Image icon1, icon2, icon3, icon4, icon5, icon6;

	ModelColumns columns;
	Glib::RefPtr<Gtk::ListStore> liststore;
};

}

#endif
