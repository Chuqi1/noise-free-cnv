/*
 *      GtkCnvOutline.hh - this file is part of noise-free-cnv.
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

#ifndef _GTKCNVOUTLINE_
#define _GTKCNVOUTLINE_
#include "GtkCnvChronoChooser.hh"
#include "GtkCnvMonitor.hh"
#include "CnvThreadClasses.hh"

#include <gtkmm.h>
#include <list>

#define GTKMM2

/* The Outline widget is used in the noise-free-cnv-gtk interface. It shows all
   opened sequences as thumbnails one above the other and allows selection
   of the sequences to perform operations on them. The logic for rendering the
   thumbnails is provided in the GtkPainterStatic.hh file. */

namespace GtkCnv {

class Outline: public Gtk::Widget
{
public:

	Outline(const ChronoChooser& c, Monitor& m);
	void close();
	std::vector<Cnv::Thread::Sequence> get_selection() const;
	unsigned get_chronology() const;
	void add_object(Cnv::Thread::Sequence o);
	void add_objects(const std::vector<Cnv::Thread::Sequence>& o);

private:

	bool redraw_issued;
	unsigned spacing, border;
	const ChronoChooser& chrono_chooser;
	Monitor& monitor;

	Glib::Timer timer;
	double last_click_y;

	class Thumbnail
	{
	public:

		Thumbnail(const Cnv::Thread::Sequence& o, unsigned w, unsigned h):
			time(0.0),selection(false),object(o),depict(o, w, h) {};

		double time;
		bool selection;
		Cnv::Thread::Sequence object;
		Cnv::Thread::PainterStatic depict;
	};

	std::list<Thumbnail> thumbs;

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
	#ifdef GTKMM2
	virtual bool on_expose_event(GdkEventExpose* ev);
	#endif
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
	virtual bool on_timeout();

	Glib::RefPtr<Gdk::Window> m_refGdkWindow;
};

}

#endif
