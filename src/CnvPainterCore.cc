/*
 *      CnvPainterCore.cc - this file is part of noise-free-cnv.
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

#include "CnvPainterCore.hh"

#include <gtkmm.h>
#include <vector>
#include <map>

/* The PainterCore class is used for rendering the data sequences in both the
   thumbnail view as well as the big interactive browser window. Its
   create_surface member constructs a Cairo::ImageSurface and uses an algorithm
   map the 0-inf integer value that it receives in the pixels array to
   brightness values. */

namespace Cnv {

Cairo::RefPtr<Cairo::ImageSurface> PainterCore::create_surface(
	unsigned width, const std::vector<unsigned>& pixels) const
{
	Cairo::RefPtr<Cairo::ImageSurface> cairo_surf;

	if(pixels.size()/width>0&&pixels.size()%width==0)
	{
		unsigned TotalPoints=0;
		std::vector<unsigned> Map;

		std::vector<unsigned>::const_iterator it;
		for(it=pixels.begin(); it!=pixels.end(); ++it)
		{
			if(*it>=Map.size()) Map.resize(*it+1);
			if(*it>0) { ++Map[*it]; ++TotalPoints; }
		}

		if(TotalPoints>0)
		{
			unsigned CountedPoints=TotalPoints;
			std::vector<unsigned>::reverse_iterator jt;
			for(jt=Map.rbegin(); jt!=Map.rend()-1; ++jt)
			{
				unsigned AmountToSubtract=*jt;
				*jt=64+CountedPoints*191/TotalPoints;
				CountedPoints-=AmountToSubtract;
			}
		}

		cairo_surf=Cairo::ImageSurface::create(Cairo::FORMAT_A8,
			width, pixels.size()/width);
		unsigned stride=cairo_surf->get_stride();
		unsigned char* imageData=cairo_surf->get_data();

		if(imageData!=NULL)
		{
			for(unsigned i=0; i<pixels.size()/width; ++i)
				for(unsigned j=0; j<width; ++j)
					imageData[i*stride+j]=Map[pixels[i*width+j]];
		}

		cairo_surf->mark_dirty();
	}
	return cairo_surf;
}

}
