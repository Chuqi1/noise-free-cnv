/*
 *      GtkCnvPanel.hh - this file is part of noise-free-cnv.
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

#ifndef _GTKCNVPANEL_
#define _GTKCNVPANEL_
#include "GtkCnvButton.hh"
#include "GtkCnvOutline.hh"
#include "CnvThreadOperations.hh"

#include <gtkmm.h>

/* The Panel widget is used in the nois-free-cnv-gtk interface to display
   buttons for all the available operations that can be performed on the opened
   data sequences. The buttons are organized into three cathegories and
   sometimes depend on an aditional numeric parameter that can be entered in an
   edit field between the buttons.

   Each button directly corresponds to a function of in the CnvOperations.hh
   and the CnvThreadOperations.hh files.
   The Button Widgets are defined separately in the GtkCnvButton.hh file so that
   alternative Panels can be created that arrange the same buttons in a
   different way. */

namespace GtkCnv {

class Panel: public Gtk::Notebook
{
public:

	Panel(Outline& o);
	virtual ~Panel() {};

	class Single: public Gtk::Table
	{
	public:

		Single(Outline& o);
		Gtk::Entry entry;
		Gtk::VSeparator separator;
		ButtonSingle1 add_button, sub_button, mul_button, div_button,
			pow_button, root_button, blur_button, trunc_button, cut_button;
		ButtonSingle2 exp_button, log_button, erf_button, rank_button,
			abs_button, avg_button, sort_names_button, sort_values_button,
			stripXY_button;
	};

	class Dual: public Gtk::Table
	{
	public:

		Dual(Outline& o);
		ButtonDual add_button, sub_button, mul_button,
			div_button, sort_button;
	};

	class Multi: public Gtk::Table
	{
	public:

		Multi(Outline& o);
		Gtk::VSeparator separator;
		ButtonMulti1 add_button, arithmetic_button, mul_button,
			geometric_button, min_button, max_button, median_button,
			deviation_button;
		ButtonMulti2 align_button;

	};

	class Macro: public Gtk::Table
	{
	public:

		Macro(Outline& o);
		ButtonSingle2 button_1;
		Gtk::VSeparator separator;
		ButtonDual button_2;
	};

private:

	Single single;
	Dual dual;
	Multi multi;
	Macro macro;
};

}

#endif
