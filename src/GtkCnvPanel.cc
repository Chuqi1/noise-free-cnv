/*
 *      GtkCnvPanel.cc - this file is part of noise-free-cnv.
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

#include "GtkCnvPanel.hh"

#include "GtkCnvButton.hh"
#include "GtkCnvOutline.hh"
#include "CnvThreadOperations.hh"

#include <gtkmm.h>
#include <string>
#include <sstream>

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

Panel::Panel(Outline& o): single(o),dual(o),multi(o),macro(o)
{
	append_page(single, "single");
	append_page(dual, "dual");
	append_page(multi, "multi");
	append_page(macro, "macros");
}

Panel::Single::Single(Outline& o):
	Gtk::Table(2, 12, false),entry(),
	add_button("add", o, entry, Cnv::Thread::add),
	sub_button("sub", o, entry, Cnv::Thread::sub),
	mul_button("mul", o, entry, Cnv::Thread::mul),
	div_button("div", o, entry, Cnv::Thread::div),
	pow_button("pow", o, entry, Cnv::Thread::pow),
	root_button("root", o, entry, Cnv::Thread::root),
	blur_button("blur", o, entry, Cnv::Thread::blur),
	trunc_button("trunc", o, entry, Cnv::Thread::trunc),
	cut_button("cut", o, entry, Cnv::Thread::cut),
	exp_button("exp", o, Cnv::Thread::exp),
	log_button("log", o, Cnv::Thread::log),
	erf_button("erf", o, Cnv::Thread::erf),
	rank_button("rank", o, Cnv::Thread::rank),
	abs_button("abs", o, Cnv::Thread::abs),
	avg_button("avg", o, Cnv::Thread::avg),
	sort_names_button("sort names", o, Cnv::Thread::sort_names),
	sort_values_button("sort values", o, Cnv::Thread::sort_values),
	stripXY_button("strip XY", o, Cnv::Thread::stripXY)
{
	entry.set_text("0");
	entry.set_width_chars(5);

	attach(add_button, 0, 1, 0, 1);
	attach(sub_button, 0, 1, 1, 2);
	attach(mul_button, 1, 2, 0, 1);
	attach(div_button, 1, 2, 1, 2);
	attach(pow_button, 2, 3, 0, 1);
	attach(root_button, 2, 3, 1, 2);
	attach(entry, 3, 4, 0, 1);
	attach(blur_button, 3, 4, 1, 2);
	attach(trunc_button, 4, 5, 0, 1);
	attach(cut_button, 4, 5, 1, 2);
	attach(separator, 5, 6, 0, 2);
	attach(exp_button, 6, 7, 0, 1);
	attach(log_button, 7, 8, 0, 1);
	attach(erf_button, 8, 9, 0, 1);
	attach(rank_button, 6, 9, 1, 2);
	attach(abs_button, 9, 10, 0, 1);
	attach(avg_button, 9, 10, 1, 2);
	attach(sort_names_button, 10, 11, 0, 1);
	attach(sort_values_button, 10, 11, 1, 2);
	attach(stripXY_button, 11, 12, 0, 2);
}

Panel::Dual::Dual(Outline& o):
	Gtk::Table(2, 6, false),
	add_button("add", o, Cnv::Thread::add),
	sub_button("sub", o, Cnv::Thread::sub),
	mul_button("mul", o, Cnv::Thread::mul),
	div_button("div", o, Cnv::Thread::div),
	sort_button("sort", o, Cnv::Thread::sort)
{
	attach(add_button, 1, 2, 0, 1);
	attach(sub_button, 1, 2, 1, 2);
	attach(mul_button, 2, 3, 0, 1);
	attach(div_button, 2, 3, 1, 2);
	attach(sort_button, 3, 4, 0, 2);
}

Panel::Multi::Multi(Outline& o):
	Gtk::Table(2, 6, false),
	add_button("add", o, Cnv::Thread::add),
	arithmetic_button("arithmetic", o, Cnv::Thread::arithmetic),
	mul_button("mul", o, Cnv::Thread::mul),
	geometric_button("geometric", o, Cnv::Thread::geometric),
	min_button("min", o, Cnv::Thread::min),
	max_button("max", o, Cnv::Thread::max),
	median_button("median", o, Cnv::Thread::median),
	deviation_button("deviation", o, Cnv::Thread::deviation),
	align_button("align", o, Cnv::Thread::align)
{
	attach(add_button, 0, 1, 0, 1);
	attach(arithmetic_button, 0, 1, 1, 2);
	attach(mul_button, 1, 2, 0, 1);
	attach(geometric_button, 1, 2, 1, 2);
	attach(min_button, 2, 3, 0, 1);
	attach(max_button, 2, 3, 1, 2);
	attach(median_button, 3, 4, 0, 1);
	attach(deviation_button, 3, 4, 1, 2);
	attach(separator, 4, 5, 0, 2);
	attach(align_button, 5, 6, 0, 2);
}

Cnv::Thread::Sequence panel_macro_deviation(
	const Cnv::Thread::Sequence& s)
{
	return Cnv::Thread::root(Cnv::Thread::avg(
		Cnv::Thread::pow(s, 2.0)), 2.0);
}

Cnv::Thread::Sequence panel_macro_eliminate(
	const Cnv::Thread::Sequence& s, const Cnv::Thread::Sequence& t)
{
	return s-(t*(Cnv::Thread::avg(s*t)/Cnv::Thread::avg(t*t)));
}

Panel::Macro::Macro(Outline& o):
	Gtk::Table(1, 3, false),
	button_1("deviation [single]\nroot( avg( pow( A, 2 ) ), 2 )",
		o, panel_macro_deviation),
	button_2("eliminate [dual]\nA - B * avg( A * B ) / avg( B * B )",
		o, panel_macro_eliminate)
{
	attach(button_1, 0, 1, 0, 1);
	attach(separator, 1, 2, 0, 1);
	attach(button_2, 3, 4, 0, 1);
}

}
