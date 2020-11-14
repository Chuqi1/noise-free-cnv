/*
 *      GtkCnvOutline.cc - this file is part of noise-free-cnv.
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

#include "GtkCnvOutline.hh"

#include "GtkCnvChronoChooser.hh"
#include "GtkCnvMonitor.hh"
#include "CnvThreadClasses.hh"

#include <gtkmm.h>
#include <list>

/* The Outline widget is used in the noise-free-cnv-gtk interface. It shows all
   opened sequences as thumbnails one above the other and allows selection
   of the sequences to perform operations on them. The logic for rendering the
   thumbnails is provided in the GtkPainterStatic.hh file. */

namespace GtkCnv {

Outline::Outline(const ChronoChooser& c, Monitor& m):redraw_issued(false),
	spacing(5),border(2),chrono_chooser(c),monitor(m),last_click_y(0.0)
	{ timer.start(); }

void Outline::close()
{
	std::list<Thumbnail>::iterator it=thumbs.begin();
	while(it!=thumbs.end())
	{
		if(it->selection) it=thumbs.erase(it);
		else ++it;
	}
	monitor.set_sequences(std::vector<Cnv::Thread::Sequence>());
	if(!redraw_issued)
	{
		queue_draw();
		redraw_issued=true;
	}
}

std::vector<Cnv::Thread::Sequence> Outline::get_selection() const
{
	std::vector<Cnv::Thread::Sequence> selection;

	std::list<Thumbnail>::const_iterator it;
	for(it=thumbs.begin(); it!=thumbs.end(); ++it)
		if(it->selection) selection.push_back(it->object);

	return selection;
}

unsigned Outline::get_chronology() const
	{ return chrono_chooser.get_chronology(); }

void Outline::add_object(Cnv::Thread::Sequence o)
{
	thumbs.push_back(Thumbnail(o, 128, 64+1));
	if(!redraw_issued)
	{
		queue_draw();
		redraw_issued=true;
	}
}

void Outline::add_objects(const std::vector<Cnv::Thread::Sequence>& o)
{
	std::vector<Cnv::Thread::Sequence>::const_iterator it;
	for(it=o.begin(); it!=o.end(); ++it)
		thumbs.push_back(Thumbnail(*it, 128, 64+1));

	if(!redraw_issued)
	{
		queue_draw();
		redraw_issued=true;
	}
}

#ifndef GTKMM2
void Outline::get_preferred_width_vfunc(int& min, int& nat) const
	{ min=128+2*border; nat=128+2*border; }
void Outline::get_preferred_height_vfunc(int& min, int& nat) const
	{ min=128+2*border; nat=128+2*border; }
void Outline::get_preferred_height_for_width_vfunc
	(int width, int& min, int& nat) const
	{ min=128+2*border; nat=128+2*border; }
void Outline::get_preferred_width_for_height_vfunc
	(int height, int& min, int& nat) const
	{ min=128+2*border; nat=128+2*border; }
#else
void Outline::on_size_request(Gtk::Requisition* req)
{
	*req=Gtk::Requisition();
	req->width=128+2*border;
	req->height=128+2*border;
}
#endif

void Outline::on_size_allocate(Gtk::Allocation& alloc)
{
	set_allocation(alloc);
	if(m_refGdkWindow)
	{
		m_refGdkWindow->move_resize(alloc.get_x(), alloc.get_y(),
			alloc.get_width(), alloc.get_height());
	}
}

void Outline::on_realize()
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
		attr.event_mask=get_events()
			|Gdk::EXPOSURE_MASK|Gdk::BUTTON_PRESS_MASK;

		m_refGdkWindow=Gdk::Window::create(get_parent_window(), &attr,
			GDK_WA_X|GDK_WA_Y);
		set_window(m_refGdkWindow);

		#ifdef GTKMM2
		style_attach();
		#endif
		m_refGdkWindow->set_user_data(gobj());
	}
}

void Outline::on_unrealize()
{
	m_refGdkWindow.reset();
	Gtk::Widget::on_unrealize();
}

bool Outline::on_button_press_event(GdkEventButton* ev)
{
	if(ev->type==GDK_BUTTON_PRESS&&ev)
	{
		if(thumbs.size()>0)
		{
			double heightPerThumb=1.0/(double)thumbs.size()
				*((double)get_allocation().get_height()+spacing);

			if(heightPerThumb>(double)get_allocation().get_width())
				heightPerThumb=(double)get_allocation().get_width();

			double position=ev->y;

			std::list<Thumbnail>::iterator it;
			if(ev->state&GDK_SHIFT_MASK&&last_click_y!=0.0)
			{
				bool active=false;
				double last_pos=last_click_y;
				for(it=thumbs.begin(); it!=thumbs.end(); ++it)
				{
					bool this_active=active;
					if(position>=0.0&&position<=heightPerThumb-spacing)
					{
						if(!active) this_active=active=true;
						else active=false;
					}
					if(last_pos>=0.0&&last_pos<=heightPerThumb-spacing)
					{
						if(!active) this_active=active=true;
						else active=false;
					}

					if(this_active) it->selection=true;
					else if(!(ev->state&GDK_CONTROL_MASK))
						it->selection=false;

					position-=heightPerThumb;
					last_pos-=heightPerThumb;
				}
			}
			else
			{
				for(it=thumbs.begin(); it!=thumbs.end(); ++it)
				{
					if(position>=0.0&&position<=heightPerThumb-spacing)
					{
						if(ev->state&GDK_CONTROL_MASK)
						{
							it->selection=!it->selection;
							if(it->selection) last_click_y=ev->y;
						}
						else
						{
							it->selection=true;
							last_click_y=ev->y;
						}
					}
					else if(!(ev->state&GDK_CONTROL_MASK))
						it->selection=false;

					position-=heightPerThumb;
				}
			}
		}
		std::vector<Cnv::Thread::Sequence> select=get_selection();
		if(select.size()>3) select.resize(3);
		monitor.set_sequences(select);

		if(!redraw_issued)
		{
			queue_draw();
			redraw_issued=true;
		}
	}
	return true;
}

#ifdef GTKMM2
bool Outline::on_expose_event(GdkEventExpose* event)
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

bool Outline::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	redraw_issued=false;
	bool draw_again=false;
	double width=(double)get_allocation().get_width();
	double height=(double)get_allocation().get_height();

	if(thumbs.size()>0&&height>spacing*(double)(thumbs.size()-1))
	{
		double heightPerThumb=(height+spacing)/(double)thumbs.size();
		if(heightPerThumb>width) heightPerThumb=width;

		unsigned selections=0;
		std::list<Thumbnail>::iterator it;
		for(it=thumbs.begin(); it!=thumbs.end(); ++it)
			if(it->selection) ++selections;

		unsigned selection=0;
		for(it=thumbs.begin(); it!=thumbs.end(); ++it)
		{
			Cairo::Matrix matrix=cr->get_matrix();
			cr->scale(width, heightPerThumb-spacing);

			cr->set_source_rgb(0.0, 0.0, 0.0);
			cr->rectangle(0.0, 0.0, 1.0, 1.0);
			cr->fill();

			if(it->time<2.0) draw_again=true;
			if(it->time==0.0&&it->depict.finished()) it->time=0.2;
			if(it->time<2.0)
			{
				double intens=1.0-fmin(it->time/2.0, 1.0);
				double time=timer.elapsed();

				cr->set_matrix(matrix);
				cr->scale(width, heightPerThumb-spacing);
				cr->move_to(0.0, 0.5);
				for(unsigned i=0; i<20; i++)
				{
					double Pos=(double)i/19.0;
					cr->line_to(Pos, 0.75
						+sin((time-115.3*Pos)*1.234*2.0*M_PI)*0.0223
						+sin((time+151.2*Pos)*1.732*2.0*M_PI)*0.0412
						+sin((time-172.3*Pos)*1.623*2.0*M_PI)*0.0382);
				}
				for(unsigned i=0; i<20; i++)
				{
					double Pos=(double)(19-i)/19.0;
					cr->line_to(Pos, 0.25
						+sin((time-115.3*Pos)*1.234*2.0*M_PI)*0.0223
						+sin((time+151.2*Pos)*1.732*2.0*M_PI)*0.0412
						+sin((time-172.3*Pos)*1.623*2.0*M_PI)*0.0382);
				}
				cr->line_to(0.0, 0.5);

				cr->set_source_rgba(1.0, 1.0, 1.0, intens*0.1);
				cr->fill();

				cr->move_to(0.0, 0.5);
				for(unsigned i=0; i<20; i++)
				{
					double Pos=(double)i/19.0;
					cr->line_to(Pos, 0.55
						+sin((time+11.53*Pos)*1.234*2.0*M_PI)*0.0123
						+sin((time-15.12*Pos)*1.732*2.0*M_PI)*0.0212
						+sin((time+17.23*Pos)*1.623*2.0*M_PI)*0.0182);
				}
				for(unsigned i=0; i<20; i++)
				{
					double Pos=(double)(19-i)/19.0;
					cr->line_to(Pos, 0.45
						+sin((time-11.53*Pos)*1.234*2.0*M_PI)*0.0123
						+sin((time+15.12*Pos)*1.732*2.0*M_PI)*0.0212
						+sin((time-17.23*Pos)*1.623*2.0*M_PI)*0.0182);
				}
				cr->set_source_rgba(1.0, 1.0, 1.0, intens*0.1);
				cr->fill();
			}

			if(it->time>0.0)
			{
				double intens=fmin(it->time/2.0, 1.0);
				if(it->selection)
				{
					if((selection%3)==0)
						cr->set_source_rgba(0.5842696629213434, 0.8651685393258464, 0.5842696629213452, intens);
					else if((selection%3)==1)
						cr->set_source_rgba(0.946629213483152, 0.6657303370786495, 0.6657303370786528, intens);
					else if((selection%3)==2)
						cr->set_source_rgba(0.7191011235955049, 0.7191011235955046, 1.0, intens);
					++selection;
				}
				else cr->set_source_rgba(1.0, 1.0, 1.0, intens);

				cr->set_matrix(matrix);
				cr->translate(border, border);
				it->depict.draw(cr, width-2.0*border,
					heightPerThumb-(spacing+2.0*border));

				it->time+=0.2;
			}
			else if(it->selection) ++selection;

			cr->set_matrix(matrix);
				cr->set_source_rgb(1.0, 1.0, 1.0);
				cr->translate(border, heightPerThumb-(spacing+border));
				std::string string=it->object.name;
				cr->show_text(string);

			if(it->selection)
			{
				cr->set_matrix(matrix);
				cr->move_to(3.0*border, 3.0*border+10.0);
				cr->line_to(3.0*border, 3.0*border);
				cr->line_to(3.0*border+10.0, 3.0*border);
				cr->move_to(width-3.0*border, 3.0*border+10.0);
				cr->line_to(width-3.0*border, 3.0*border);
				cr->line_to(width-3.0*border-10.0, 3.0*border);
				cr->set_source_rgb(1.0, 0.0, 0.0);
				cr->stroke();
			}

			cr->set_matrix(matrix);
			cr->translate(0.0, heightPerThumb);
		}
	}
	if(draw_again)
	{
		redraw_issued=true;
		Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &Outline::on_timeout), 20);
		}

	return true;
}

bool Outline::on_timeout()
{
	queue_draw();
	return false;
}

}
