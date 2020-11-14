/*
 *      CnvLoadSave.hh - this File is part of noise-free-cnv.
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

#ifndef _CNVLOADSAVE_
#define _CNVLOADSAVE_
#include "CnvSequence.hh"
#include "CnvStringPool.hh"

#include <string>
#include <list>

/* This file defines the fundamental load and save routines for data sequences.
   Both the load and the save function make use of the native file format
   exclusively. The corresponding function for loading PennCNV files are
   outsourced to the PennCnvLoadSave.hh file.

   In addition to the filename, the load function receives a StringPool object
   reference which it uses to minimize memory footprint. */

namespace Cnv {

Sequence load(std::string f, StringPool& pool);

void save(const Sequence& s, std::string f);

}


#endif
