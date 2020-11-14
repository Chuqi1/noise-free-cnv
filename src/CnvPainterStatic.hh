/*
 *      CnvPainterStatic.hh - this file is part of noise-free-cnv.
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

#ifndef _VPAINTERSTATIC_
#define _VPAINTERSTATIC_
#include "CnvPainterCore.hh"

#include <gtkmm.h>

/* The PainterDynamic class is responsible for rendering the thumbnails of the
   data sequences in the catalogue at the left edge of the noise-free-cnv-gtk
   interface. */

namespace Cnv {

class PainterStatic: private PainterCore
{
public:
	PainterStatic() {};
	PainterStatic(const std::vector<float>& s, unsigned width, unsigned height);
	void draw(Cairo::RefPtr<Cairo::Context> cr,
		unsigned width, unsigned height) const;
private:
	Cairo::RefPtr<Cairo::ImageSurface> cairo_surf;
};

}

#endif
