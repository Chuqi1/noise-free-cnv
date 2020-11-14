/*
 *      CnvThreadClasses.cc - this file is part of noise-free-cnv.
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

#include "CnvThreadClasses.hh"

#include "CnvPainterStatic.hh"
#include "CnvPainterDynamic.hh"
#include "CnvSequence.hh"
#include "CnvStringPool.hh"
#include "CnvThreadWrap.hh"

#include <gtkmm.h>
#include <string>

/* This file defines threaded encapsulations for the Sequence, PainterStatic and
   PainterDynamic classes. It uses the RefPtr template class defined in
   CnvThreadWrap and does not add any other significant functionality. */

namespace Cnv { namespace Thread {

Sequence::Sequence() {}
Sequence::Sequence(const std::string& n): name(n) {}

void PainterStatic_thread_func(PainterStatic ps, Sequence s,
	unsigned w, unsigned h)
{
	ps.writer_lock();
	s.reader_lock();
	if(s!=NULL&&ps!=NULL) *ps=Cnv::PainterStatic(s->get_values(), w, h);
	s.reader_unlock();
	ps.writer_unlock();
}

PainterStatic::PainterStatic(const Sequence& s, unsigned w, unsigned h)
{
	Glib::Thread::create(
		sigc::bind(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
			PainterStatic_thread_func),h),w),s),*this), false);
}

bool PainterStatic::finished()
{
	if(reader_trylock())
	{
		reader_unlock();
		return true;
	}
	else return false;
}

bool PainterStatic::draw(Cairo::RefPtr<Cairo::Context> cr,
	unsigned width, unsigned height)
{
	if(reader_trylock()&&(*this)!=NULL)
	{
		(*this)->draw(cr, width, height);
		reader_unlock();
		return true;
	}
	else return false;
}

void PainterDynamic_thread_func(PainterDynamic ps, Sequence s)
{
	ps.writer_lock();
	s.reader_lock();
	if(s!=NULL&&ps!=NULL) *ps=Cnv::PainterDynamic(s->get_values());
	s.reader_unlock();
	ps.writer_unlock();
}

PainterDynamic::PainterDynamic(const Sequence& s)
{
	Glib::Thread::create(
		sigc::bind(sigc::bind(sigc::ptr_fun(
			PainterDynamic_thread_func),s),*this), false);
}

bool PainterDynamic::finished()
{
	if(reader_trylock())
	{
		reader_unlock();
		return true;
	}
	else return false;
}

bool PainterDynamic::draw(Cairo::RefPtr<Cairo::Context> cr,
	unsigned width, unsigned height, float l, float r)
{
	if(reader_trylock()&&(*this)!=NULL)
	{
		(*this)->draw(cr, width, height, l, r);
		reader_unlock();
		return true;
	}
	else return false;
}

}}
