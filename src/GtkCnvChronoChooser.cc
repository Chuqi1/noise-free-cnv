/*
 *      GtkCnvChronoChooser.cc - this file is part of noise-free-cnv.
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

#include "GtkCnvChronoChooser.hh"

#include <gtkmm.h>

/* The ChronoChooser widget is used in the noise-free-cnv-gtk interface below
   the main menu. It defines a drop down menu which allows to choose how
   noise-free-cnv-gtk handles calls to operations that take to arguments if
   more than two data sequences are selected. */

namespace GtkCnv {

ChronoChooser::ChronoChooser():
	icon1(NFCNV_SHARE_FOLDER "icon1.png"),
	icon2(NFCNV_SHARE_FOLDER "icon2.png"),
	icon3(NFCNV_SHARE_FOLDER "icon3.png"),
	icon4(NFCNV_SHARE_FOLDER "icon4.png"),
	icon5(NFCNV_SHARE_FOLDER "icon5.png"),
	icon6(NFCNV_SHARE_FOLDER "icon6.png")
{
	Glib::RefPtr<Gtk::IconTheme> theme=Gtk::IconTheme::get_default();

	liststore=Gtk::ListStore::create(columns);
	set_model(liststore);
	Gtk::TreeModel::Row row=*(liststore->append());
	row[columns.m_col_pixbuf]=icon1.get_pixbuf();
	row=*(liststore->append());
	row[columns.m_col_pixbuf]=icon2.get_pixbuf();
	row=*(liststore->append());
	row[columns.m_col_pixbuf]=icon3.get_pixbuf();
	row=*(liststore->append());
	row[columns.m_col_pixbuf]=icon4.get_pixbuf();
	row=*(liststore->append());
	row[columns.m_col_pixbuf]=icon5.get_pixbuf();
	row=*(liststore->append());
	row[columns.m_col_pixbuf]=icon6.get_pixbuf();
	pack_start(columns.m_col_pixbuf);

	set_active(0);
}

unsigned ChronoChooser::get_chronology() const
	{ return get_active_row_number(); }

}
