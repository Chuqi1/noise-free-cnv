/*
 *      CnvCalling.hh - this File is part of noise-free-cnv.
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


#ifndef _CNVCALLING_
#define _CNVCALLING_
#include "CnvSequence.hh"
#include "CnvOperations.hh"
#include "CnvCallingReport.hh"
#include <vector>
#include <string>

/* This file implements calling algorithms to autonomously find copy number
   variations in DNA-microarray data sequences. 

   The calling part of noise-free-cnv is largely experimental and not included
   in the GUI program noise-free-cnv-gtk. */

namespace Cnv { namespace Call {

class Configuration
{
public:
	Configuration();

	bool verbose;

	bool search_delets;
	bool search_duplis;

	double delet_expect;
	double dupli_expect;
	unsigned border_width;
	unsigned max_width;

	double trigger_threshold;
	double final_threshold;
};

double score_base_generic(signed char delet_usual_dupli, double dev,
	std::vector<float>::const_iterator first,
	std::vector<float>::const_iterator last,
	const Configuration& config);

double score_inner(bool dupli_delet, double dev,
	const std::vector<float>& points, unsigned begin, unsigned end,
	const Configuration& config);

double score_enter(bool dupli_delet, double dev,
	const std::vector<float>& points, unsigned begin, unsigned end,
	const Configuration& config);

double score_leave(bool dupli_delet, double dev,
	const std::vector<float>& points, unsigned begin, unsigned end,
	const Configuration& config);

void calling(const Sequence& seq, bool dupli_delet,
	std::vector<ReportEntry>& report, const Configuration& config);

void calling(const Sequence& seq,
	std::vector<ReportEntry>& report, const Configuration& config);

}}

#endif
