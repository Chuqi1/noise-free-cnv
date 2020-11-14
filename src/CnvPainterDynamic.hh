/*
 *      CnvPainterDynamic.hh - this file is part of noise-free-cnv.
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

#ifndef _CNVPAINTERDYNAMIC_
#define _CNVPAINTERDYNAMIC_
#include "CnvPainterCore.hh"

#include <gtkmm.h>
#include <vector>

/* The PainterDynamic class is responsible for rendering the data sequences in
   the big interactive browser window. The logic for displaying the labels and
   coordinate system are not part of PainterDynamic but are implemented in the
   Gtk::Monitor widget in the GtkMonitor.hh file. */

using namespace std;

namespace Cnv {

class PainterDynamic: private PainterCore
{
public:

	PainterDynamic();
	PainterDynamic(const vector<float>& s);

	unsigned size() const;

	void draw(Cairo::RefPtr<Cairo::Context> cr,
		unsigned width, unsigned height, double l, double r) const;

private:

	class Mipmap
	{
	public:
		vector<unsigned char> pos;
		vector<unsigned short> count;
		unsigned clustsize;
	};

	void draw_buffer(unsigned l, unsigned n,
		vector<unsigned>& buffer) const;

	void compute_mipmap(vector<unsigned char> points, Mipmap& mip) const;
	void shrink_mipmap(const Mipmap& old_map, Mipmap& new_mip) const;

	vector<unsigned char> points;
	vector<Mipmap> mipmaps;
};

}

#endif
