/*
 *      CnvPainterCore.hh - this file is part of noise-free-cnv.
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

#ifndef _CNVPAINTERCORE_
#define _CNVPAINTERCORE_
#include <gtkmm.h>
#include <vector>

/* The PainterCore class is used for rendering the data sequences in both the
   thumbnail view as well as the big interactive browser window. Its
   create_surface member constructs a Cairo::ImageSurface and uses an algorithm
   map the 0-inf integer value that it receives in the pixels array to
   brightness values. */

namespace Cnv {

class PainterCore
{
public:

	Cairo::RefPtr<Cairo::ImageSurface> create_surface(unsigned width,
		const std::vector<unsigned>& pixels) const;
};

}

#endif
