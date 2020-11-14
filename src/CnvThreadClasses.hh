/*
 *      CnvThreadClasses.hh - this file is part of noise-free-cnv.
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

#ifndef _CNVTHREADCLASSES_
#define _CNVTHREADCLASSES_
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

class Sequence: public RefPtr<Cnv::Sequence>
{
public:

	Sequence();
	Sequence(const std::string& n);

	std::string name;
};

class PainterStatic: public RefPtr<Cnv::PainterStatic>
{
public:

	PainterStatic(const Sequence& s, unsigned w, unsigned h);
	bool finished();
	bool draw(Cairo::RefPtr<Cairo::Context> cr, unsigned w, unsigned h);
};

class PainterDynamic: public RefPtr<Cnv::PainterDynamic>
{
public:

	PainterDynamic(const Sequence& s);
	bool finished();
	bool draw(Cairo::RefPtr<Cairo::Context> cr,
		unsigned width, unsigned height, float l, float r);
};

class StringPool: public RefPtr<Cnv::StringPool> {};

}}

#endif
