/*
 *      GtkCnvInterface.cc - this file is part of noise-free-cnv.
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

/* This Widget combines the Menu, ChronoChooser, Monitor, Outline, Panel and
   Navigator widget into the Interface widget that makes up the complete
   noise-free-cnv-gtk interface. */

namespace GtkCnv {

Interface::Interface():
	outline(chrono_chooser, monitor),menu(outline),panel(outline),
	navi(monitor)
{
	box_left.pack_start(menu, false, false, 0);
	box_left.pack_start(chrono_chooser, false, false, 0);
	box_left.pack_start(outline, true, true, 0);

	box_right.pack_start(panel, false, false, 0);
	box_right.pack_start(navi, false, false, 0);
	box_right.pack_start(monitor, true, true, 0);

	pack1(box_left, false, false);
	pack2(box_right, true, false);
}

}
