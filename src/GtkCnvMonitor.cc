/*
 *      GtkCnvMonitor.cc - this file is part of noise-free-cnv.
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

#include "GtkCnvMonitor.hh"

#include "CnvThreadClasses.hh"
#include "CnvEncodeDecode.hh"

#include <gtkmm.h>
#include <list>
#include <sstream>
#include <iostream>

/* This widget is used to visualize up to three data sequences in an interactive
   fashion. It is used in noise-free-cnv-gtk as the most prominent part of the
   interface. The logic for rendering the data sequences is provided in the
   GtkPainterDynamic.hh file. */

namespace GtkCnv {

void Monitor::Layer::compute_segments()
{
	if(object.reader_trylock()&&object!=NULL&&object->size()>0)
	{
		std::vector<Cnv::StringPointer> names=object->get_names();
		std::vector<unsigned> int_segments;
		int_segments.push_back(0);
		int_segments.push_back(names.size()-1);

		while(int_segments.size()<128)
		{
			bool redo=false;
			for(unsigned i=0; i+1<int_segments.size(); i++)
			{
				std::string id1, chr1, pos1;
				std::string id2, chr2, pos2;
				std::string id_new, chr_new, pos_new;

				Cnv::decompose_point_name(names[int_segments[i]],
					id1, chr1, pos1);
				Cnv::decompose_point_name(names[int_segments[i+1]],
					id2, chr2, pos2);

				if(int_segments[i+1]>int_segments[i]+1&&chr1!=chr2)
				{
					redo=true;
					unsigned new_point=(int_segments[i]+int_segments[i+1])/2;

					Cnv::decompose_point_name(names[new_point],
						id_new, chr_new, pos_new);

					if(chr_new==chr1) int_segments[i]=new_point;
					else if(chr_new==chr2) int_segments[i+1]=new_point;
					else
					{
						int_segments.insert(int_segments.begin()+(++i), new_point);
						int_segments.insert(int_segments.begin()+(++i), new_point);
					}
				}
			}
			if(!redo) break;
		}

		segments.push_back(0.0);
		if(int_segments.size()<128)
		{
			for(unsigned i=0; i<int_segments.size(); i+=2)
				segments.push_back((double)int_segments[i]/(double)(names.size()-1));
		}
		segments.push_back(1.0);

		segments_computed=true;
	}
}

Monitor::PickPoint Monitor::Layer::pick(double x, double y,
	double aspect_ratio, double max) const
{
	if(object.reader_trylock()&&object!=NULL&&object->size()>0)
	{
		double best_y_distance=max;
		unsigned best_point=0;

		double ratio=aspect_ratio/((double)object->size());

		unsigned start=(unsigned)(x*(double)object->size());
		if(start>=object->size()) start=object->size()-1;

		double top=y+max;
		double bottom=y-max;
		for(unsigned offset=0; top>bottom; ++offset)
		{
			if(start>=offset)
			{
				double point_value=object->get_values()[start-offset];
				point_value=std::min(std::max(point_value, -1.0), 1.0);

				if(point_value>bottom&&point_value<top)
				{
					best_point=start-offset;
					double y_distance=fabs(point_value-y);
					bottom+=best_y_distance-y_distance;
					top-=best_y_distance-y_distance;
					best_y_distance=y_distance;
				}
			}
			if(object->size()-offset>start)
			{
				double point_value=object->get_values()[start+offset];
				point_value=std::min(std::max(point_value, -1.0), 1.0);

				if(point_value>bottom&&point_value<top)
				{
					best_point=start+offset;
					double y_distance=fabs(point_value-y);
					bottom+=best_y_distance-y_distance;
					top-=best_y_distance-y_distance;
					best_y_distance=y_distance;
				}
			}
			else break;
			top-=ratio;
			bottom+=ratio;
		}

		if(best_y_distance<max)
		{
			double best_dist=best_y_distance
				+fabs((double)best_point-(double)start)*ratio;
			double best_x=((double)best_point+0.5)/(double)object->size();
			double best_y=object->get_values()[best_point];

			std::string best_string;
			if(object->get_names().size()>best_point)
			{
				best_string=(std::string)object->get_names()[best_point];
				while(best_string.find('/')<best_string.size())
					best_string.replace(best_string.find('/'), 1, " | ");

				std::string::iterator it;
				for(it=best_string.begin(); it!=best_string.end(); ++it)
					if(*it<' ') *it=' ';
				best_string+=" = ";
			}

			std::string value_string;
			std::stringstream sstream;
			sstream<<best_y;
			sstream>>value_string;
			best_string+=value_string;

			object.reader_unlock();
			return PickPoint(best_dist, best_x, best_y, best_string);
		}
	}
	return PickPoint(max, 0.0, 0.0, "");
}

void Monitor::set_sequences(const std::vector<Cnv::Thread::Sequence>& o)
{
	std::list<Layer> new_layers;
	std::vector<Cnv::Thread::Sequence>::const_iterator it;
	for(it=o.begin(); it!=o.end(); ++it)
	{
		std::list<Layer>::const_iterator jt;
		for(jt=layers.begin(); jt!=layers.end(); ++jt)
		{
			if(*it==jt->object)
			{
				new_layers.push_back(*jt);
				break;
			}
		}
		if(jt==layers.end())
			new_layers.push_back(Layer(*it));
	}
	layers.clear();
	layers=new_layers;
	if(buffer_surface) buffer_surface.clear();
	if(!redraw_issued)
	{
		queue_draw();
		redraw_issued=true;
	}
}

void Monitor::set_boundary(bool right_left,
	unsigned char chr, unsigned pos)
{
	if(layers.size()!=0&&layers.front().object.reader_trylock())
	{
		const std::vector<Cnv::StringPointer>& identifiers=
			layers.front().object->get_names();

		unsigned window_left=0;
		unsigned window_right=identifiers.size()-1;

		std::string id_left, id_right;
		unsigned char chr_left, chr_right;
		unsigned pos_left, pos_right;
		Cnv::decompose_point_name(
			(const std::string&)identifiers[window_left],
			id_left, chr_left, pos_left);
		Cnv::decompose_point_name(
			(const std::string&)identifiers[window_right],
			id_right, chr_right, pos_right);

		while(window_left+1<window_right)
		{
			if(chr_left==chr&&pos_left==pos)
				{ window_right=window_left; break; }
			else if(chr_right==chr&&pos_right==pos)
				{ window_left=window_right; break; }
			else if(chr_left>chr||(chr_left==chr
				&&pos_left>pos)) return;
			else if(chr_right<chr||(chr_right==chr
				&&pos_right<pos)) return;
			else
			{
				std::string id_center;
				unsigned char chr_center;
				unsigned pos_center;
				decompose_point_name(
					identifiers[window_left+(window_right-window_left)/2],
					id_center, chr_center, pos_center);

				if(chr_center<chr||(chr_center==chr
					&&pos_center<pos))
				{
					window_left+=(window_right-window_left)/2;
					id_left=id_center;
					chr_left=chr_center;
					pos_left=pos_center;
				}
				else
				{
					window_right=window_left+(window_right-window_left)/2;
					id_right=id_center;
					chr_right=chr_center;
					pos_right=pos_center;
				}
			}
		}

		if(right_left) right=old_right=(double)(window_left-1)
			/(double)(identifiers.size()-1);
		else left=old_left=(double)(window_left-1)
			/(double)(identifiers.size()-1);
	}
	if(buffer_surface) buffer_surface.clear();
	if(!redraw_issued)
	{
		queue_draw();
		redraw_issued=true;
	}
}

void Monitor::set_boundary(unsigned char chr_left, unsigned pos_left,
	unsigned char chr_right, unsigned pos_right)
{
	set_boundary(false, chr_left, pos_left);
	set_boundary(true, chr_right, pos_right);

}

void Monitor::draw_bubble(const Cairo::RefPtr<Cairo::Context>& cr,
	double px, double py, double x1, double x2, double y1, double y2,
	double offset, bool first, bool last, bool closed)
{
	double yOff=offset;
	double xOff=(x2>x1)?offset:-offset;

	cr->move_to(px, py);
	if(first)
	{
		cr->curve_to(x1-2.0*xOff, (y1+y2)/2.0, x1-2.0*xOff, y1-yOff, x1, y1-yOff);
		cr->line_to(x2, y1-yOff);
		cr->curve_to(x2+xOff, y1-yOff, x2+xOff, y1-yOff, x2+xOff, y1);
	}
	else
	{
		cr->curve_to(x1-2.0*xOff, (y1+y2)/2.0, x1-2.0*xOff, y1, x1-xOff, y1-yOff/2.0);
		if(closed) cr->line_to(x2+xOff, y1-yOff/2.0);
		else cr->move_to(x2+xOff, y1-yOff/2.0);
	}

	if(last)
	{
		cr->line_to(x2+xOff, y2);
		cr->curve_to(x2+xOff, y2+yOff, x2+xOff, y2+yOff, x2, y2+yOff);
		cr->line_to(x1, y2+yOff);
		cr->curve_to(x1-2.0*xOff, y2+yOff, x1-2.0*xOff, (y1+y2)/2.0, px, py);
	}
	else
	{
		cr->line_to(x2+xOff, y2+yOff/2.0);
		if(closed) cr->line_to(x1-xOff, y2+yOff/2.0);
		else cr->move_to(x1-xOff, y2+yOff/2.0);
		cr->curve_to(x1-2.0*xOff, y2, x1-2.0*xOff, (y1+y2)/2.0, px, py);
	}
}

void Monitor::draw_background(const Cairo::RefPtr<Cairo::Context>& cr,
	double width, double height)
{
	Cairo::Matrix matrix=cr->get_matrix();

	bool draw_again=false;
	if(buffer_surface)
	{
		cr->set_source(buffer_surface, 0.0, 0.0);
		cr->scale(width/(double)buffer_surface->get_width(),
			height/(double)buffer_surface->get_height());
		cr->paint();
	}
	else
	{
		std::list<Layer>::iterator it;
		for(it=layers.begin(); it!=layers.end(); ++it)
		{
			if(it->time<0.30) draw_again=true;
			if(it->time==0.0&&it->depict.finished()) it->time=0.01;
		}

		if(draw_again)
		{
			for(it=layers.begin(); it!=layers.end(); ++it)
			{
				if(!it->segments_computed) it->compute_segments();
				if(it->time>0.0)
				{
					double intens=fmin(it->time/0.30, 1.0);
					cr->save();
					for(unsigned i=1; i+1<it->segments.size(); i+=2)
					{
						double left_segm=it->segments[i];
						double right_segm=it->segments[i+1];

						if(left_segm<left) left_segm=left;
						if(right_segm>right) right_segm=right;

						if(left_segm<right_segm)
						{
							left_segm=(left_segm-left)/(right-left)*width;
							right_segm=(right_segm-left)/(right-left)*width;

							cr->move_to((unsigned)left_segm, 0.0);
							cr->line_to((unsigned)right_segm, 0.0);
							cr->line_to((unsigned)right_segm, height);
							cr->line_to((unsigned)left_segm, height);
							cr->close_path();
						}
					}
					cr->clip();

					if((std::distance(layers.begin(), it)%3)==0)
						cr->set_source_rgba(0.667134831, 0.80758427, 0.667134831, intens);
					else if((std::distance(layers.begin(), it)%3)==1)
						cr->set_source_rgba(0.848314607, 0.707865169, 0.707865169, intens);
					else if((std::distance(layers.begin(), it)%3)==2)
						cr->set_source_rgba(0.734550562, 0.734550562, 0.875, intens);
					cr->set_source_rgba(0.75, 0.75, 0.75, intens);
					it->depict.draw(cr, width, height/(double)layers.size(), left, right);

					cr->restore();
					cr->save();

					for(unsigned i=0; i+1<it->segments.size(); i+=2)
					{
						double left_segm=it->segments[i];
						double right_segm=it->segments[i+1];

						if(left_segm<left) left_segm=left;
						if(right_segm>right) right_segm=right;

						if(left_segm<right_segm)
						{
							left_segm=(left_segm-left)/(right-left)*width;
							right_segm=(right_segm-left)/(right-left)*width;

							cr->move_to((unsigned)left_segm, 0.0);
							cr->line_to((unsigned)right_segm, 0.0);
							cr->line_to((unsigned)right_segm, height);
							cr->line_to((unsigned)left_segm, height);
							cr->close_path();
						}
					}
					cr->clip();

					if((std::distance(layers.begin(), it)%3)==0)
						cr->set_source_rgba(0.5842696629213434, 0.8651685393258464, 0.5842696629213452, intens);
					else if((std::distance(layers.begin(), it)%3)==1)
						cr->set_source_rgba(0.946629213483152, 0.6657303370786495, 0.6657303370786528, intens);
					else if((std::distance(layers.begin(), it)%3)==2)
						cr->set_source_rgba(0.7191011235955049, 0.7191011235955046, 1.0, intens);
					it->depict.draw(cr, width, height/(double)layers.size(), left, right);

					cr->restore();

					it->time+=0.02;
				}
				cr->translate(0.0, height/(double)layers.size());
			}
		}
		else
		{
			buffer_surface=Cairo::ImageSurface::create(
				Cairo::FORMAT_RGB24, width, height);

			Cairo::RefPtr<Cairo::Context> cr2=
				Cairo::Context::create(buffer_surface);

			for(it=layers.begin(); it!=layers.end(); ++it)
			{
				if(it->time>0.0)
				{
					double intens=fmin(it->time/0.30, 1.0);

					cr2->save();
					for(unsigned i=1; i+1<it->segments.size(); i+=2)
					{
						double left_segm=it->segments[i];
						double right_segm=it->segments[i+1];

						if(left_segm<left) left_segm=left;
						if(right_segm>right) right_segm=right;

						if(left_segm<right_segm)
						{
							left_segm=(left_segm-left)/(right-left)*width;
							right_segm=(right_segm-left)/(right-left)*width;

							cr2->move_to((unsigned)left_segm, 0.0);
							cr2->line_to((unsigned)right_segm, 0.0);
							cr2->line_to((unsigned)right_segm, height);
							cr2->line_to((unsigned)left_segm, height);
							cr2->close_path();
						}
					}
					cr2->clip();

					if((std::distance(layers.begin(), it)%3)==0)
						cr2->set_source_rgba(0.667134831, 0.80758427, 0.667134831, intens);
					else if((std::distance(layers.begin(), it)%3)==1)
						cr2->set_source_rgba(0.848314607, 0.707865169, 0.707865169, intens);
					else if((std::distance(layers.begin(), it)%3)==2)
						cr2->set_source_rgba(0.734550562, 0.734550562, 0.875, intens);
					cr2->set_source_rgba(0.75, 0.75, 0.75, intens);
					it->depict.draw(cr2, width, height/(double)layers.size(), left, right);

					cr2->restore();
					cr2->save();

					for(unsigned i=0; i+1<it->segments.size(); i+=2)
					{
						double left_segm=it->segments[i];
						double right_segm=it->segments[i+1];

						if(left_segm<left) left_segm=left;
						if(right_segm>right) right_segm=right;

						if(left_segm<right_segm)
						{
							left_segm=(left_segm-left)/(right-left)*width;
							right_segm=(right_segm-left)/(right-left)*width;

							cr2->move_to((unsigned)left_segm, 0.0);
							cr2->line_to((unsigned)right_segm, 0.0);
							cr2->line_to((unsigned)right_segm, height);
							cr2->line_to((unsigned)left_segm, height);
							cr2->close_path();
						}
					}
					cr2->clip();

					if((std::distance(layers.begin(), it)%3)==0)
						cr2->set_source_rgba(0.5842696629213434, 0.8651685393258464, 0.5842696629213452, intens);
					else if((std::distance(layers.begin(), it)%3)==1)
						cr2->set_source_rgba(0.946629213483152, 0.6657303370786495, 0.6657303370786528, intens);
					else if((std::distance(layers.begin(), it)%3)==2)
						cr2->set_source_rgba(0.7191011235955049, 0.7191011235955046, 1.0, intens);
					it->depict.draw(cr2, width, height/(double)layers.size(), left, right);

					cr2->restore();


					it->time+=0.02;
				}
				cr2->translate(0.0, height/(double)layers.size());
			}

			cr->set_source(buffer_surface, 0.0, 0.0);
			cr->scale(width/(double)buffer_surface->get_width(),
				height/(double)buffer_surface->get_height());
			cr->paint();
		}
	}

	cr->set_matrix(matrix);

	std::list<Layer>::iterator it;
	for(it=layers.begin(); it!=layers.end(); ++it)
	{
		unsigned i=std::distance(layers.begin(), it);
		cr->set_line_width(1.5);
		cr->set_source_rgba(0.5, 0.0, 0.0, 0.75);
		cr->move_to(0.0, height/(double)layers.size()*((double)i+0.5));
		cr->line_to((double)width, height/(double)layers.size()*((double)i+0.5));
		cr->stroke();
	}

	if(draw_again)
	{
		redraw_issued=true;
		Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &Monitor::on_timeout), 20);
	}
}

void Monitor::draw_foreground(const Cairo::RefPtr<Cairo::Context>& cr,
	double width, double height)
{
	Cairo::Matrix matrix=cr->get_matrix();
	if(layers.size()>0)
	{
		unsigned layer_nr=0;
		PickPoint picked=layers.begin()->pick(
			left+cursor_x*(right-left), 1.0-2.0*cursor_y*layers.size(),
			(width/(right-left))/(height/(double)layers.size()), 0.1);

		unsigned layer_nr_new=0;
		std::list<Layer>::const_iterator it;
		for(it=layers.begin(); it!=layers.end(); ++it)
		{
			PickPoint picked_new=it->pick(
				left+cursor_x*(right-left), 1.0-2.0*(cursor_y
				*(double)layers.size()-(double)layer_nr_new),
				(width/(right-left))/(height/(double)layers.size()),
				picked.dist);

			if(picked_new.dist<picked.dist)
			{
				picked=picked_new;
				layer_nr=layer_nr_new;
			}
			layer_nr_new++;
		}

		for(unsigned i=0; i<layers.size(); i++)
		{
			cr->set_line_cap(Cairo::LINE_CAP_ROUND);
			cr->set_source_rgba(0.5, 0.0, 0.0, 0.75);
			cr->set_line_width(1.5);
			cr->move_to(cursor_x*width, height/(double)layers.size()*((double)i));
			cr->line_to(cursor_x*width, height/(double)layers.size()*((double)(i+1)));
			cr->stroke();
		}


		if(picked.dist<0.1)
		{
			cr->set_font_size(12.0);
			Cairo::TextExtents extents;
			cr->get_text_extents(picked.str, extents);

			double xPoint=(picked.x-left)/(right-left)*width;
			double yPoint=height/(double)layers.size()
				*((double)layer_nr+(1.0-std::min(std::max(picked.y, -1.0),1.0))/2.0);

			double xStart;
			if(cursor_x<0.5) xStart=cursor_x*width+60.0;
			else xStart=cursor_x*width-60.0;

			double xEnd;
			if(cursor_x<0.5) xEnd=xStart+extents.x_advance;
			else xEnd=xStart-extents.x_advance;

			double yStart=cursor_y*height
				-(extents.y_bearing+extents.height*2.0)*cursor_y;

			double yEnd=yStart-extents.y_bearing;

			cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
			draw_bubble(cr, xPoint, yPoint, xStart, xEnd, yStart, yEnd,
				(extents.height+extents.y_bearing)*2.0, true, true, true);
			cr->set_line_width(3.5);
			cr->stroke_preserve();
			cr->set_source_rgba(0.0, 0.0, 0.0, 0.75);
			cr->fill_preserve();

			if(layers.size()==1)
				cr->set_source_rgb(1.0, 1.0, 1.0);
			else if((layer_nr%3)==0)
				cr->set_source_rgb(0.5842696629213434, 0.8651685393258464, 0.5842696629213452);
			else if((layer_nr%3)==1)
				cr->set_source_rgb(0.946629213483152, 0.6657303370786495, 0.6657303370786528);
			else if((layer_nr%3)==2)
				cr->set_source_rgb(0.7191011235955049, 0.7191011235955046, 1.0);

			cr->set_line_width(1.5);
			cr->set_operator(Cairo::OPERATOR_ADD);
			cr->stroke();
			cr->set_operator(Cairo::OPERATOR_OVER);
			cr->move_to(fmin(xStart, xEnd), yEnd);
			cr->show_text(picked.str);
		}
	}
	cr->set_matrix(matrix);
}

#ifndef GTKMM2
void Monitor::get_preferred_width_vfunc(int& min, int& nat) const
	{ min=384+2*border; nat=768+2*border; }
void Monitor::get_preferred_height_vfunc(int& min, int& nat) const
	{ min=257+2*border; nat=514+2*border; }
void Monitor::get_preferred_height_for_width_vfunc
	(int width, int& min, int& nat) const
	{ min=384+2*border; nat=768+2*border; }
void Monitor::get_preferred_width_for_height_vfunc
	(int height, int& min, int& nat) const
	{ min=257+2*border; nat=514+2*border; }
#else
void Monitor::on_size_request(Gtk::Requisition* req)
{
	*req=Gtk::Requisition();
	req->width=384+2*border;
	req->height=257+2*border;
}
#endif

void Monitor::on_size_allocate(Gtk::Allocation& alloc)
{
	set_allocation(alloc);
	if(m_refGdkWindow)
	{
		m_refGdkWindow->move_resize(alloc.get_x(), alloc.get_y(),
			alloc.get_width(), alloc.get_height());
	}
	if(buffer_surface) buffer_surface.clear();
}

void Monitor::on_realize()
{
	set_realized();
	if(!m_refGdkWindow)
	{
		GdkWindowAttr attr;
		memset(&attr, 0, sizeof(attr));

		attr.event_mask=get_events();
		attr.x=get_allocation().get_x();
		attr.y=get_allocation().get_y();
		attr.width=get_allocation().get_width();
		attr.height=get_allocation().get_height();
		attr.wclass=GDK_INPUT_OUTPUT;
		attr.window_type=GDK_WINDOW_CHILD;
		attr.event_mask=get_events()|Gdk::EXPOSURE_MASK
			|Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK
			|Gdk::POINTER_MOTION_MASK|Gdk::LEAVE_NOTIFY_MASK;

		m_refGdkWindow=Gdk::Window::create(get_parent_window(), &attr,
			GDK_WA_X|GDK_WA_Y);
		set_window(m_refGdkWindow);

		#ifdef GTKMM2
		style_attach();
		#endif
		m_refGdkWindow->set_user_data(gobj());
	}
}

void Monitor::on_unrealize()
{
	m_refGdkWindow.reset();
	Gtk::Widget::on_unrealize();
}

bool Monitor::on_button_press_event(GdkEventButton* ev)
{
	cursor_x=ev->x/(double)get_allocation().get_width();
	cursor_y=ev->y/(double)get_allocation().get_height();
	mouse_down=true;
	old_cursor_x=cursor_x;
	old_cursor_y=cursor_y;
	old_left=left;
	old_right=right;
	return true;
}

bool Monitor::on_button_release_event(GdkEventButton* ev)
{
	cursor_x=ev->x/(double)get_allocation().get_width();
	cursor_y=ev->y/(double)get_allocation().get_height();
	mouse_down=false;
	if(!redraw_issued)
	{
		queue_draw();
		redraw_issued=true;
	}
	return true;
}

bool Monitor::on_motion_notify_event(GdkEventMotion* ev)
{
	cursor_x=ev->x/(double)get_allocation().get_width();
	cursor_y=ev->y/(double)get_allocation().get_height();
	if(mouse_down)
	{
		double oldWidth=old_right-old_left;
		left=old_left+(old_cursor_x-cursor_x)*oldWidth;
		right=old_right+(old_cursor_x-cursor_x)*oldWidth;

		double fixpoint=left*(1.0-cursor_x)+right*cursor_x;
		left=fixpoint+(left-fixpoint)*pow(1000.0, cursor_y-old_cursor_y);
		right=fixpoint+(right-fixpoint)*pow(1000.0, cursor_y-old_cursor_y);

		if(left<=0.0&&right>=1.0)
		{
			left=0.0; right=1.0;
		}
		else if(left<0.0)
		{
			right=fixpoint+(right-fixpoint)*(fixpoint/(fixpoint-left));
			left=0.0;
		}
		else if(right>1.0)
		{
			left=fixpoint+(left-fixpoint)*((1.0-fixpoint)/(right-fixpoint));
			right=1.0;
		}

		if(left<0.0) left=0.0;
		if(right>1.0) right=1.0;
		if(left>=right) { left=0.0; right=1.0; }
		if(buffer_surface) buffer_surface.clear();
	}
	if(!redraw_issued)
	{
		queue_draw();
		redraw_issued=true;
	}
	return true;
}

bool Monitor::on_leave_notify_event(GdkEventCrossing* ev)
{
	cursor_x=(ev->x-2.0)/(double)(get_allocation().get_width()-4.0);
	cursor_y=(ev->y-2.0)/(double)(get_allocation().get_height()-4.0);
	if(!redraw_issued)
	{
		queue_draw();
		redraw_issued=true;
	}
	return true;
}

#ifdef GTKMM2
bool Monitor::on_expose_event(GdkEventExpose* event)
{
	redraw_issued=false;
	if(m_refGdkWindow)
	{
		Cairo::RefPtr<Cairo::Context> cr=
			m_refGdkWindow->create_cairo_context();

		if(event)
		{
			cr->rectangle(event->area.x, event->area.y,
				  event->area.width, event->area.height);
			cr->clip();
		}
		Gdk::Cairo::set_source_color(cr,
			get_style()->get_bg(Gtk::STATE_NORMAL));
		cr->paint();

		return on_draw(cr);
	}
	else return true;
}
#endif

bool Monitor::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	redraw_issued=false;
	double width=(double)get_allocation().get_width();
	double height=(double)get_allocation().get_height();

	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->rectangle(0.0, 0.0, width, height);
	cr->fill();

	cr->translate(border, border);
	draw_background(cr, width-2.0*border, height-2.0*border);
	if(!mouse_down&&cursor_x>0.0&&cursor_x<1.0&&cursor_y>0.0&&cursor_y<1.0)
		draw_foreground(cr, width-2.0*border, height-2.0*border);

	return true;
}

bool Monitor::on_timeout()
{
	queue_draw();
	return false;
}

}
