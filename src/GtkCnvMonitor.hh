/*
 *      GtkCnvMonitor.hh - this file is part of noise-free-cnv.
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

#ifndef _GTKCNVMONITOR_
#define _GTKCNVMONITOR_
#include "CnvThreadClasses.hh"

#include <gtkmm.h>
#include <list>

/* This widget is used to visualize up to three data sequences in an interactive
   fashion. It is used in noise-free-cnv-gtk as the most prominent part of the
   interface. The logic for rendering the data sequences is provided in the
   GtkPainterDynamic.hh file. */

#define GTKMM2

namespace GtkCnv {

class Monitor: public Gtk::Widget
{
public:

	Monitor(): border(2),mouse_down(false),redraw_issued(false),
		left(0.0),right(1.0),cursor_x(0.0),cursor_y(0.0) {}
	void set_sequences(const std::vector<Cnv::Thread::Sequence>& o);

	void set_boundary(bool right_left, unsigned char chr, unsigned pos);
	void set_boundary(unsigned char chr_left, unsigned pos_left,
		unsigned char chr_right, unsigned pos_right);

private:

	unsigned border;

	bool mouse_down;
	bool redraw_issued;
	double old_left, old_right;
	double left, right;
	double old_cursor_x, old_cursor_y;
	double cursor_x, cursor_y;

	Cairo::RefPtr<Cairo::ImageSurface> buffer_surface;

	class PickPoint
	{
	public:

		PickPoint(double d, double px, double py, const std::string& s):
			dist(d),x(px),y(py),str(s) {}

		double dist;
		double x, y;
		std::string str;
	};

	class Layer
	{
	public:

		Layer(const Cnv::Thread::Sequence& o):
			time(0.0),object(o),depict(o),segments_computed(false) {};
		void compute_segments();
		PickPoint pick(double x, double y, double aspect_ratio,
			double max) const;

		double time;
		Cnv::Thread::Sequence object;
		Cnv::Thread::PainterDynamic depict;
		bool segments_computed;
		std::vector<double> segments;

	};

	void draw_bubble(const Cairo::RefPtr<Cairo::Context>& cr,
		double px, double py, double x1, double x2, double y1, double y2,
		double offset,  bool first, bool last, bool closed);

	void draw_background(const Cairo::RefPtr<Cairo::Context>& cr,
		double width, double height);
	void draw_foreground(const Cairo::RefPtr<Cairo::Context>& cr,
		double width, double height);

	std::list<Layer> layers;

protected:

	#ifndef GTKMM2
	virtual void get_preferred_width_vfunc(int& min, int& nat) const;
	virtual void get_preferred_height_vfunc(int& min, int& nat) const;
	virtual void get_preferred_height_for_width_vfunc
		(int width, int& min, int& nat) const;
	virtual void get_preferred_width_for_height_vfunc
		(int height, int& min, int& nat) const;
	#else
	virtual void on_size_request(Gtk::Requisition* req);
	#endif
	virtual void on_size_allocate(Gtk::Allocation& alloc);
	virtual void on_realize();
	virtual void on_unrealize();

	virtual bool on_button_press_event(GdkEventButton* ev);
	virtual bool on_button_release_event(GdkEventButton* ev);
	virtual bool on_motion_notify_event(GdkEventMotion* ev);
	virtual bool on_leave_notify_event(GdkEventCrossing* event);
	#ifdef GTKMM2
	virtual bool on_expose_event(GdkEventExpose* ev);
	#endif
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
	virtual bool on_timeout();

	Glib::RefPtr<Gdk::Window> m_refGdkWindow;
};

}

#endif
