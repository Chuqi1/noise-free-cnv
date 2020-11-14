/*
 *      CnvThreadOperations.hh - this file is part of noise-free-cnv.
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

#ifndef _CNVTHREADOPERATIONS_
#define _CNVTHREADOPERATIONS_
#include "CnvThreadClasses.hh"

#include <string>
#include <list>

/* This file defines threaded equivalents to the functions defined in
   CnvOperations. The actual functions consist of little more than locking the
   assosiated mutex, performing the underlying operatoion and unlocking. */

namespace Cnv { namespace Thread {

Sequence 	load_namesvalues	(std::string, StringPool);
void		save_namesvalues	(const Sequence&, std::string);

void		save_lrrbaf	(const std::vector<Sequence>&, std::string);
std::vector<Sequence> 	load_lrrbaf(std::string, StringPool);

Sequence	add		(const Sequence&, float);
Sequence	mul		(const Sequence&, float);
Sequence	sub		(const Sequence&, float);
Sequence	div		(const Sequence&, float);
Sequence	pow		(const Sequence&, float);
Sequence	root	(const Sequence&, float);
Sequence	blur	(const Sequence&, float);
Sequence	trunc	(const Sequence&, float);
Sequence	cut		(const Sequence&, float);
Sequence	exp			(const Sequence&);
Sequence	log			(const Sequence&);
Sequence	abs			(const Sequence&);
Sequence	erf			(const Sequence&);
Sequence	sort_names	(const Sequence&);
Sequence	sort_values	(const Sequence&);
Sequence	avg			(const Sequence&);
Sequence	rank		(const Sequence&);
Sequence	stripXY		(const Sequence&);
Sequence	add			(const Sequence&, const Sequence&);
Sequence	mul			(const Sequence&, const Sequence&);
Sequence	sub			(const Sequence&, const Sequence&);
Sequence	div			(const Sequence&, const Sequence&);
Sequence 	sort		(const Sequence&, const Sequence&);
Sequence	add			(const std::vector<Sequence>&);
Sequence	arithmetic	(const std::vector<Sequence>&);
Sequence	mul			(const std::vector<Sequence>&);
Sequence	geometric	(const std::vector<Sequence>&);
Sequence 	min			(const std::vector<Sequence>&);
Sequence 	max			(const std::vector<Sequence>&);
Sequence 	median		(const std::vector<Sequence>&);
Sequence 	deviation	(const std::vector<Sequence>&);
std::vector<Sequence> 	align(const std::vector<Sequence>&);

inline Sequence operator+(const Sequence& a, float p)
	{ return add(a, p); }
inline Sequence operator*(const Sequence& a, float p)
	{ return mul(a, p); }
inline Sequence operator-(const Sequence& a, float p)
	{ return sub(a, p); }
inline Sequence operator/(const Sequence& a, float p)
	{ return div(a, p); }

inline Sequence operator+(float p, const Sequence& a)
	{ return add(a, p); }
inline Sequence operator*(float p, const Sequence& a)
	{ return mul(a, p); }

inline Sequence operator+(const Sequence& s, const Sequence& t)
	{ return add(s, t); }
inline Sequence operator-(const Sequence& s, const Sequence& t)
	{ return sub(s, t); }
inline Sequence operator*(const Sequence& s, const Sequence& t)
	{ return mul(s, t); }
inline Sequence operator/(const Sequence& s, const Sequence& t)
	{ return div(s, t); }

}}

#endif
