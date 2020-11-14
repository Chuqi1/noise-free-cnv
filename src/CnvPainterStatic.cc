/*
 *      CnvPainterStatic.cc - this file is part of noise-free-cnv.
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

#include "CnvPainterStatic.hh"

#include <gtkmm.h>
#include <algorithm>

/* The PainterDynamic class is responsible for rendering the thumbnails of the
   data sequences in the catalogue at the left edge of the noise-free-cnv-gtk
   interface. */

namespace Cnv {

PainterStatic::PainterStatic(const std::vector<float>& s,
	unsigned width, unsigned height)
{
	if(width>0&&height>0&&s.size()>0)
	{
		unsigned points_per_pixel=s.size()/width;
		if((s.size()%width)!=0) points_per_pixel++;

		width=s.size()/points_per_pixel;
		if((s.size()%points_per_pixel)!=0) width++;

		std::vector<unsigned> pixels;
		pixels.resize((unsigned)width*(unsigned)height);

		std::vector<float>::const_iterator it;
		for(it=s.begin(); it!=s.end(); ++it)
		{
			unsigned column=(it-s.begin())/points_per_pixel;
			if(!std::isnan(*it))
			{
				float yValue=std::min(std::max((1.0-*it)/2.0, 0.0), 1.0);
				unsigned line=(unsigned)(yValue*(float)(height-1)+0.5);
				pixels[line*width+column]++;
			}
		}
		cairo_surf=create_surface(width, pixels);
	}
}

void PainterStatic::draw(Cairo::RefPtr<Cairo::Context> cr,
	unsigned width, unsigned height) const
{
	if(cairo_surf)
	{
		Cairo::Matrix matrix=cr->get_matrix();
		cr->scale((double)width/(double)cairo_surf->get_width(),
			(double)height/(double)cairo_surf->get_height());
		cr->mask(cairo_surf, 0.0, 0.0);
		cr->set_matrix(matrix);
	}
}

}

