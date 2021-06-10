/*
 *      CnvOperations.cc - this file is part of noise-free-cnv.
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

#include "CnvOperations.hh"

#include "CnvSequence.hh"

#include <glibmm.h>
#include <algorithm>
#include <map>
#include <set>
#include <fftw3.h>
#include <cmath>

/* This file defines all the basic operations that can be performed on data
   sequences. Only the load and save routines are outsourced to the
   CnvLoadSave.hh and PennCnvLoadSave.hh files. More detailed information about
   the single operations can be found on the project homepage. */

namespace Cnv {

Sequence add(const Sequence& s, float p)
{
	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		out.push_back(iter.name(), iter.value()+p);
	return out;
}

Sequence mul(const Sequence& s, float p)
{
	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		out.push_back(iter.name(), iter.value()*p);
	return out;
}

Sequence sub(const Sequence& s, float p)
	{ return add(s, -p); }
Sequence div(const Sequence& s, float p)
	{ return mul(s, 1.0f/p); }

Sequence pow(const Sequence& s, float p)
{
	float f=cosf(M_PI*p);
	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		out.push_back(iter.name(),
			::pow(fabs(iter.value()), p)*((iter.value()>=0)?1.0f:f));
	return out;
}

Sequence root(const Sequence& s, float p)
	{ return pow(s, 1.0f/p); }

Sequence blur(const Sequence& s, float p)
{
	static Glib::Threads::Mutex Mutex;
	Mutex.lock();

	Sequence out; out.reserve(s.size());
	if(s.size()>0)
	{
		double* Real=(double*)fftw_malloc(s.size()*sizeof(double));
		fftw_complex* Complex=(fftw_complex*)
			fftw_malloc((s.size()/2+1)*sizeof(fftw_complex));

		if(Real!=NULL&&Complex!=NULL)
		{
			unsigned newSize=0;
			for(SequenceSingleIterator iter(s); iter; ++iter)
				if(!std::isnan(iter.value())&&!std::isinf(iter.value()))
					Real[newSize++]=iter.value();

			fftw_plan Plan;
			Plan=fftw_plan_dft_r2c_1d(newSize,
				Real, Complex, FFTW_ESTIMATE);
			fftw_execute(Plan);
			fftw_destroy_plan(Plan);

			double MaxFreq=(double)newSize/p;
			for(unsigned i=0; i<newSize/2+1; i++)
			{
				double Temp=(double)i*(double)i/(MaxFreq*MaxFreq)/2.0;
				Complex[i][0]*=::exp(-Temp)/(double)newSize;
				Complex[i][1]*=::exp(-Temp)/(double)newSize;
			}

			Plan=fftw_plan_dft_c2r_1d(newSize,
				Complex, Real, FFTW_ESTIMATE);
			fftw_execute(Plan);
			fftw_destroy_plan(Plan);

			unsigned counter=0;
			for(SequenceSingleIterator iter(s); iter; ++iter)
				if(std::isnan(iter.value())||std::isinf(iter.value()))
					out.push_back(iter.name(), iter.value());
				else out.push_back(iter.name(), Real[counter++]);
		}
		if(Complex!=NULL) fftw_free(Complex);
		if(Real!=NULL) fftw_free(Real);
	}
	Mutex.unlock();
	return out;
}

Sequence trunc(const Sequence& s, float p)
{
	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		if(std::isnan(iter.value())) out.push_back(iter.name(), iter.value());
		else out.push_back(iter.name(), std::min(std::max((float)iter.value(), -p), p));
	return out;
}

Sequence cut(const Sequence& s, float p)
{
	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		if(!std::isnan(iter.value())&&iter.value()>=-p&&iter.value()<=p)
			out.push_back(iter.name(), iter.value());
	return out;
}

Sequence exp(const Sequence& s)
{
	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		out.push_back(iter.name(), ::exp(iter.value()));
	return out;
}

Sequence log(const Sequence& s)
{
	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		out.push_back(iter.name(), ::log(iter.value()));
	return out;
}

Sequence abs(const Sequence& s)
{
	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		out.push_back(iter.name(), ::fabs(iter.value()));
	return out;
}

Sequence erf(const Sequence& s)
{
	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		out.push_back(iter.name(), ::erf(iter.value()));
	return out;
}

bool sort_names_compare(
	const std::pair<StringPointer,float>& p1,
	const std::pair<StringPointer,float>& p2)
{
	return p1.first<p2.first
		||(p1.first==p2.first&&p1.second<p2.second);
}

bool sort_values_compare(
	const std::pair<StringPointer,float>& p1,
	const std::pair<StringPointer,float>& p2)
{
	return p1.second<p2.second
		||(p1.second==p2.second&&p1.first<p2.first);
}

Sequence sort_names(const Sequence& s)
{
	std::vector<std::pair<StringPointer,float> > tmp;

	for(SequenceSingleIterator iter(s); iter; ++iter)
		tmp.push_back(std::pair<StringPointer,float>(iter.name(), iter.value()));

	std::sort(tmp.begin(), tmp.end(), sort_names_compare);

	Sequence out; out.reserve(s.size());

	std::vector<std::pair<StringPointer,float> >::const_iterator it;

	for(it=tmp.begin(); it!=tmp.end(); ++it)
		out.push_back(it->first, it->second);

	return out;
}

Sequence sort_values(const Sequence& s)
{
	std::vector<std::pair<StringPointer,float> > tmp;

	for(SequenceSingleIterator iter(s); iter; ++iter)
		tmp.push_back(std::pair<StringPointer,float>(iter.name(), iter.value()));

	std::sort(tmp.begin(), tmp.end(), sort_values_compare);

	Sequence out; out.reserve(s.size());

	std::vector<std::pair<StringPointer,float> >::const_iterator it;

	for(it=tmp.begin(); it!=tmp.end(); ++it)
		out.push_back(it->first, it->second);

	return out;
}

Sequence avg(const Sequence& s)
{
	double value=0.0;
	unsigned nanValues=0;
	for(SequenceSingleIterator iter(s); iter; ++iter)
		if(std::isnan(iter.value())) ++nanValues;
		else value+=iter.value();

	if(nanValues<s.size())
		value/=(double)(s.size()-nanValues);

	Sequence out; out.reserve(s.size());
	for(SequenceSingleIterator iter(s); iter; ++iter)
		if(std::isnan(iter.value())) out.push_back(iter.name(), iter.value());
		else out.push_back(iter.name(), value);

	return out;
}

Sequence rank(const Sequence& s)
{
	std::map<float,unsigned> tmp;

	for(SequenceSingleIterator iter(s); iter; ++iter)
		tmp[iter.value()]++;

	std::map<float,float> tmp2;

	float current_value=-1.0;
	std::map<float,unsigned>::const_iterator jt;
	for(jt=tmp.begin(); jt!=tmp.end(); ++jt)
	{
		current_value+=(float)jt->second/(float)s.size();
		tmp2[jt->first]=current_value;
		current_value+=(float)jt->second/(float)s.size();
	}

	Sequence out;
	for(SequenceSingleIterator iter(s); iter; ++iter)
		out.push_back(iter.name(), tmp2[iter.value()]);

	return out;
}

Sequence stripXY(const Sequence& s)
{
	Sequence out;
	for(SequenceSingleIterator iter(s); iter; ++iter)
	{
		std::string s=iter.name();
		size_t Finding=s.find('/');
		if(Finding+2<s.size()&&(s[Finding+2]=='/'
			||(s[Finding+2]>='0'&&s[Finding+2]<='9'&&s[Finding+3]=='/'))
			&&s[Finding+1]>='0'&&s[Finding+1]<='9')
			out.push_back(iter.name(), iter.value());
	}
	return out;
}

Sequence add(const Sequence& s, const Sequence& t)
{
	Sequence out;
	for(SequenceDualIterator iter(s, t); iter; ++iter)
		out.push_back(iter.name(), iter.first()+iter.second());
	return out;
}

Sequence mul(const Sequence& s, const Sequence& t)
{
	Sequence out;
	for(SequenceDualIterator iter(s, t); iter; ++iter)
		out.push_back(iter.name(), iter.first()*iter.second());
	return out;
}

Sequence sub(const Sequence& s, const Sequence& t)
{
	Sequence out;
	for(SequenceDualIterator iter(s, t); iter; ++iter)
		out.push_back(iter.name(), iter.first()-iter.second());
	return out;
}

Sequence div(const Sequence& s, const Sequence& t)
{
	Sequence out;
	for(SequenceDualIterator iter(s, t); iter; ++iter)
		out.push_back(iter.name(), iter.first()/iter.second());
	return out;
}

Sequence sort(const Sequence& s, const Sequence& t)
{
	std::map<std::string,float> temp;
	for(SequenceSingleIterator iter(s); iter; ++iter)
		temp[iter.name()]=iter.value();

	Sequence out;
	for(SequenceSingleIterator iter(t); iter; ++iter)
		out.push_back(iter.name(), temp[iter.name()]);

	return out;
}

Sequence add(const std::vector<const Sequence*>& s)
{
	Sequence out;
	for(SequenceMultiIterator iter(s); iter; ++iter)
	{
		float value=0.0;
		for(unsigned i=0; i<s.size(); ++i) value+=iter[i];
		out.push_back(iter.name(), value);
	}
	return out;
}

Sequence arithmetic(const std::vector<const Sequence*>& s)
{
	Sequence out;
	float divisor=(s.size()!=0)?((float)s.size()):1.0;
	for(SequenceMultiIterator iter(s); iter; ++iter)
	{
		float value=0.0;
		for(unsigned i=0; i<s.size(); ++i) value+=iter[i];
		out.push_back(iter.name(), value/divisor);
	}
	return out;
}

Sequence mul(const std::vector<const Sequence*>& s)
{
	Sequence out;
	for(SequenceMultiIterator iter(s); iter; ++iter)
	{
		float value=1.0;
		for(unsigned i=0; i<s.size(); ++i) value*=iter[i];
		out.push_back(iter.name(), value);
	}
	return out;
}

Sequence geometric(const std::vector<const Sequence*>& s)
{
	Sequence out;
	float divisor=(s.size()!=0)?((float)s.size()):1.0;
	float factor=::cos(M_PI/divisor);
	for(SequenceMultiIterator iter(s); iter; ++iter)
	{
		float value=1.0;
		for(unsigned i=0; i<s.size(); ++i) value*=iter[i];
		out.push_back(iter.name(), ::pow(::fabs(value), 1.0/divisor)
			*((value>=0.0)?1.0:factor));
	}
	return out;
}

Sequence min(const std::vector<const Sequence*>& s)
{
	Sequence out;
	for(SequenceMultiIterator iter(s); iter; ++iter)
	{
		float value=(s.size()>0)?iter[0]:0.0;
		for(unsigned i=1; i<s.size(); ++i)
			if(iter[i]<value) value=iter[i];
		out.push_back(iter.name(), value);
	}
	return out;
}

Sequence max(const std::vector<const Sequence*>& s)
{
	Sequence out;
	for(SequenceMultiIterator iter(s); iter; ++iter)
	{
		float value=(s.size()>0)?iter[0]:0.0;
		for(unsigned i=1; i<s.size(); ++i)
			if(iter[i]>value) value=iter[i];
		out.push_back(iter.name(), value);
	}
	return out;
}

Sequence median(const std::vector<const Sequence*>& s)
{
	Sequence out;
	std::vector<float> buffer; buffer.resize(s.size());
	for(SequenceMultiIterator iter(s); iter; ++iter)
	{
		for(unsigned i=0; i<s.size(); ++i) buffer[i]=iter[i];

		std::sort(buffer.begin(), buffer.end());
		if(buffer.size()>0&&buffer.size()%2==0)
			out.push_back(iter.name(), (buffer[(buffer.size()-1)/2]
				+buffer[buffer.size()/2])/2.0);
		else if(buffer.size()>0&&buffer.size()%2==1)
			out.push_back(iter.name(), buffer[buffer.size()/2]);
		else out.push_back(iter.name(), 0.0);
	}
	return out;
}

Sequence deviation(const std::vector<const Sequence*>& s)
{
	Sequence out;
	for(SequenceMultiIterator iter(s); iter; ++iter)
	{
		float value=0.0;
		for(unsigned i=0; i<s.size(); ++i) value+=iter[i]*iter[i];
		out.push_back(iter.name(), ::sqrt(value));
	}
	return out;
}

std::vector<Sequence> align(const std::vector<const Sequence*>& s)
{
	std::vector<Sequence> out;
	out.resize(s.size());
	for(SequenceMultiIterator iter(s); iter; ++iter)
	{
		std::vector<Sequence>::iterator it=out.begin();
		for(unsigned i=0; i<s.size(); ++i)
			(it++)->push_back(iter.name(), iter[i]);
	}
	return out;
}

}
