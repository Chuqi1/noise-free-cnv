/*
 *      CnvPainterDynamic.cc - this file is part of noise-free-cnv.
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

#include "CnvPainterDynamic.hh"

#include <gtkmm.h>
#include <algorithm>
#include <vector>

#include <iostream>

/* The PainterDynamic class is responsible for rendering the data sequences in
   the big interactive browser window. The logic for displaying the labels and
   coordinate system are not part of PainterDynamic but are implemented in the
   Gtk::Monitor widget in the GtkMonitor.hh file. */

using namespace std;

namespace Cnv {

PainterDynamic::PainterDynamic() {}

PainterDynamic::PainterDynamic(const vector<float>& s)
{
	vector<float>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		if(isnan(*it)) points.push_back(255);
		else
		{
			float yValue=min(max((1.0f-*it)/2.0f, 0.0f), 1.0f);
			unsigned line=(unsigned)(yValue*(float)(255-1)+0.5f);
			points.push_back(line);
		}
	}

	mipmaps.resize(1);
	compute_mipmap(points, mipmaps[0]);
	for(unsigned i=0; mipmaps[i].pos.size()>mipmaps[i].clustsize; ++i)
	{
		mipmaps.resize(mipmaps.size()+1);
		shrink_mipmap(mipmaps[i], mipmaps[i+1]);
	}
}

unsigned PainterDynamic::size() const { return points.size(); }

void PainterDynamic::draw(Cairo::RefPtr<Cairo::Context> cr,
	unsigned width, unsigned height, double left, double right) const
{
	unsigned resX=width/3;
	if(width>0&&height>0&&resX>0&&size()>0)
	{
		unsigned left_point=(unsigned)(left*(double)size());
		unsigned right_point=(unsigned)(right*(double)size());
		if(left_point>=size()) left_point=size()-1;
		if(right_point>=size()) right_point=size()-1;

		if(left_point<right_point)
		{
			unsigned points_per_pixel=(right_point-left_point)/resX;
			if(((right_point-left_point)%resX)!=0) points_per_pixel++;

			left_point=left_point/points_per_pixel;
			right_point=right_point/points_per_pixel+1;

			double actual_left=(double)(left_point*points_per_pixel)
				/(double)(size());
			double actual_right=(double)(right_point*points_per_pixel)
				/(double)(size());

			vector<unsigned> pixels;
			if(height<256+128)
				pixels.resize((right_point-left_point)*85);
			else
				pixels.resize((right_point-left_point)*255);

			vector<unsigned> buffer;
			buffer.resize(256);

			for(unsigned i=0; i<right_point-left_point; ++i)
			{
				buffer.assign(256, 0);
				draw_buffer((left_point+i)*points_per_pixel,
					points_per_pixel, buffer);

				if(height<256+128)
				{
					for(unsigned j=0; j<85; j++)
						pixels[j*(right_point-left_point)+i]=
							buffer[j*3+0]+buffer[j*3+1]+buffer[j*3+2];
				}
				else
				{
					for(unsigned j=0; j<255; j++)
						pixels[j*(right_point-left_point)+i]=buffer[j];
				}
			}

			Cairo::RefPtr<Cairo::ImageSurface> cairo_surf
				=create_surface(right_point-left_point, pixels);

			Cairo::RefPtr<Cairo::SurfacePattern> cairo_pattern
				=Cairo::SurfacePattern::create(cairo_surf);
			cairo_pattern->set_filter(Cairo::FILTER_BEST);

			Cairo::Matrix matrix=cr->get_matrix();
			cr->scale((double)width*(actual_right-left)/(right-left), (double)height);
			cr->scale(1.0/(1.0+(actual_left-left)/(actual_right-actual_left)), 1.0);
			cr->translate((actual_left-left)/(actual_right-actual_left), 0.0);
			if(height<256+128)
				cr->scale(1.0/(double)(right_point-left_point), 1.0/85.0);
			else
				cr->scale(1.0/(double)(right_point-left_point), 1.0/255.0);
			cr->mask(cairo_pattern);
			cr->set_matrix(matrix);
		}
	}
}

void PainterDynamic::draw_buffer(unsigned start, unsigned size,
	vector<unsigned>& buffer) const
{
	if(points.size()>start)
	{
		if(start+size>points.size()) size=points.size()-start;

		if(size<8)
		{
			for(unsigned i=start; i<start+size; ++i)
				buffer[points[i]]++;
		}
		else
		{
			for(; start&7u; --size) buffer[points[start++]]++;
			for(; size&7u; ) buffer[points[start+(--size)]]++;

			start=start/8; size=size/8;

			vector<Mipmap>::const_iterator layer=mipmaps.begin();
			while(size>0&&layer!=mipmaps.end())
			{
				if(start&1)
				{
					unsigned offset=start*layer->clustsize;
					for(unsigned i=0; i<layer->clustsize; ++i)
					{
						buffer[layer->pos[offset+i]]
							+=layer->count[offset+i];
					}
					++start; --size;
				}
				else if(size&1)
				{
					unsigned offset=(start+size-1)*layer->clustsize;
					for(unsigned i=0; i<layer->clustsize; ++i)
					{
						buffer[layer->pos[offset+i]]
							+=layer->count[offset+i];
					}
					--size;
				}
				else
				{
					size/=2;
					start/=2;
					++layer;
				}
			}
		}
	}
}

void PainterDynamic::compute_mipmap(vector<unsigned char> points, Mipmap& mip) const
{
	mip.pos.reserve(points.size());
	mip.count.reserve(points.size());

	mip.clustsize=0;

	for(unsigned i=0; i+8<=points.size(); i+=8)
	{
		sort(points.begin()+i, points.begin()+i+8);

		unsigned written_points=1;

		mip.pos.push_back(points[i+7]);
		mip.count.push_back(1);
		for(unsigned j=1; j<8; j++)
		{
			if(points[i+7-j]==points[i+8-j]) ++*mip.count.rbegin();
			else
			{
				++written_points;
				mip.pos.push_back(points[i+7-j]);
				mip.count.push_back(1);
			}
		}

		if(written_points>mip.clustsize)
			mip.clustsize=written_points;

		for(; written_points<8; ++written_points)
		{
			mip.pos.push_back(0);
			mip.count.push_back(0);
		}
	}

	for(unsigned i=0; i<mip.pos.size()/8; ++i)
	{
		for(unsigned j=0; j<mip.clustsize; ++j)
		{
			mip.pos[i*mip.clustsize+j]=mip.pos[i*8+j];
			mip.count[i*mip.clustsize+j]=mip.count[i*8+j];
		}
	}
	mip.pos.resize(mip.pos.size()/8*mip.clustsize);
	mip.count.resize(mip.count.size()/8*mip.clustsize);
}

void PainterDynamic::shrink_mipmap(const Mipmap& old_mip, Mipmap& new_mip) const
{
	unsigned temp_clustsize=old_mip.clustsize*2;
	new_mip.pos.reserve(old_mip.pos.size()/temp_clustsize*temp_clustsize);
	new_mip.count.reserve(old_mip.count.size()/temp_clustsize*temp_clustsize);

	new_mip.clustsize=0;
	for(unsigned i=0; i<old_mip.pos.size()/temp_clustsize; ++i)
	{
		unsigned written_points=0;

		unsigned read1=(i*2)*old_mip.clustsize;
		unsigned read2=(i*2+1)*old_mip.clustsize;

		unsigned end1=(i*2+1)*old_mip.clustsize;
		unsigned end2=(i*2+2)*old_mip.clustsize;

		while(end1!=read1&&old_mip.count[end1-1]==0) --end1;
		while(end2!=read2&&old_mip.count[end2-1]==0) --end2;

		while(read1!=end1&&read2!=end2)
		{
			if(old_mip.pos[read1]>old_mip.pos[read2])
			{
				new_mip.pos.push_back(old_mip.pos[read1]);
				new_mip.count.push_back(old_mip.count[read1]);
				read1++;
			}
			else if(old_mip.pos[read1]<old_mip.pos[read2])
			{
				new_mip.pos.push_back(old_mip.pos[read2]);
				new_mip.count.push_back(old_mip.count[read2]);
				read2++;
			}
			else
			{
				new_mip.pos.push_back(old_mip.pos[read1]);
				new_mip.count.push_back(old_mip.count[read1]+old_mip.count[read2]);
				read1++; read2++;
			}
			written_points++;
		}
		while(read1<end1)
		{
			new_mip.pos.push_back(old_mip.pos[read1]);
			new_mip.count.push_back(old_mip.count[read1]);
			read1++;
			written_points++;
		}
		while(read2<end2)
		{
			new_mip.pos.push_back(old_mip.pos[read2]);
			new_mip.count.push_back(old_mip.count[read2]);
			read2++;
			written_points++;
		}

		if(written_points>new_mip.clustsize)
			new_mip.clustsize=written_points;

		for(; written_points<temp_clustsize; ++written_points)
		{
			new_mip.pos.push_back(0);
			new_mip.count.push_back(0);
		}
	}

	for(unsigned i=0; i<new_mip.pos.size()/temp_clustsize; ++i)
	{
		for(unsigned j=0; j<new_mip.clustsize; ++j)
		{
			new_mip.pos[i*new_mip.clustsize+j]=new_mip.pos[i*temp_clustsize+j];
			new_mip.count[i*new_mip.clustsize+j]=new_mip.count[i*temp_clustsize+j];
		}
	}
	new_mip.pos.resize(new_mip.pos.size()/temp_clustsize*new_mip.clustsize);
	new_mip.count.resize(new_mip.count.size()/temp_clustsize*new_mip.clustsize);
}

}
