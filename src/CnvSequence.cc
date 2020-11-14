/*
 *      CnvSequence.cc - this file is part of noise-free-cnv.
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

#include "CnvSequence.hh"

#include <string>
#include <vector>
#include <list>

/* The CnvSequence class if used as the basic container of noise-free-cnv to
   store the actual data sequences. From the outside it is similar to
   std::vector<std::pair<std::string, double>> with reduced functionality.
   The SequenceSingleIterator does not offer serious functionality beside the
   obvious. The SequenceDualIterator and SequenceMultiIterator allow iterating
   through several data sequences at once and are able to work on Sequences that
   are not exactly identical. In that case, the increment operators will find
   the nearest match where the data points in all sequences share the same name
   string. */

namespace Cnv {

void Sequence::push_back(StringPointer name, float value)
{
	if(name||names.size()!=0) names.push_back(name);
	values.push_back(value);
}

void Sequence::push_back(float value)
{
	values.push_back(value);
	if(names.size()!=0) names.push_back(StringPointer());
}

const std::vector<StringPointer>& Sequence::get_names() const
{
	return names;
}

const std::vector<float>& Sequence::get_values() const
{
	return values;
}

SequenceSingleIterator::SequenceSingleIterator(const SequenceSingleIterator& m):
	name_it(m.name_it),name_end(m.name_end),
	value_it(m.value_it),value_end(m.value_end)
	{}

SequenceSingleIterator::SequenceSingleIterator(const Sequence& s):
	name_it(s.get_names().begin()),name_end(s.get_names().end()),
	value_it(s.get_values().begin()),value_end(s.get_values().end())
	{}

SequenceSingleIterator::operator bool() const
{
	return value_it!=value_end;
}

SequenceSingleIterator& SequenceSingleIterator::operator++()
{
	if(name_it!=name_end) ++name_it;
	if(value_it!=value_end) ++value_it;
	return *this;
}

SequenceSingleIterator SequenceSingleIterator::operator++(int)
{
	SequenceSingleIterator tmp=*this;
	operator++();
	return tmp;
}

float SequenceSingleIterator::value() const
{
	if(value_it!=value_end) return *value_it;
	else return 0.0f;
}

StringPointer SequenceSingleIterator::name() const
{
	if(name_it!=name_end) return *name_it;
	else return StringPointer();
}

SequenceDualIterator::SequenceDualIterator(const SequenceDualIterator& m)
	:name_it1(m.name_it1),name_it2(m.name_it2),
	name_end1(m.name_end1),name_end2(m.name_end2),
	value_it1(m.value_it1),value_it2(m.value_it2),
	value_end1(m.value_end1),value_end2(m.value_end2)
	{}

SequenceDualIterator::SequenceDualIterator(const Sequence& s, const Sequence& t)
	:name_it1(s.get_names().begin()),name_it2(t.get_names().begin()),
	name_end1(s.get_names().end()),name_end2(t.get_names().end()),
	value_it1(s.get_values().begin()),value_it2(t.get_values().begin()),
	value_end1(s.get_values().end()),value_end2(t.get_values().end())
{
	find_match();
}

SequenceDualIterator::operator bool() const
{
	return value_it1!=value_end1&&value_it2!=value_end2;
}

SequenceDualIterator& SequenceDualIterator::operator++()
{
	if(name_it1!=name_end1) ++name_it1;
	if(value_it1!=value_end1) ++value_it1;
	if(name_it2!=name_end2) ++name_it2;
	if(value_it2!=value_end2) ++value_it2;

	find_match();
	return *this;
}

SequenceDualIterator SequenceDualIterator::operator++(int)
{
	SequenceDualIterator tmp=*this;
	operator++();
	return tmp;
}

float SequenceDualIterator::first() const
{
	if(value_it1!=value_end1) return *value_it1;
	else return 0.0f;
}

float SequenceDualIterator::second() const
{
	if(value_it2!=value_end2) return *value_it2;
	else return 0.0f;
}

StringPointer SequenceDualIterator::name() const
{
	if(name_it1!=name_end1) return *name_it1;
	else return StringPointer();
}

void SequenceDualIterator::find_match()
{
	std::vector<StringPointer>::const_iterator old_it1=name_it1;
	std::vector<StringPointer>::const_iterator old_it2=name_it2;
	find_match_helper(name_it1, name_it2, name_end1, name_end2);

	if(name_it1-old_it1>value_end1-value_it1)
		value_it1=value_end1;
	else value_it1=value_it1+(name_it1-old_it1);

	if(name_it2-old_it2>value_end2-value_it2)
		value_it2=value_end2;
	else value_it2=value_it2+(name_it2-old_it2);
}

void SequenceDualIterator::find_match_helper(
	std::vector<StringPointer>::const_iterator& it1,
	std::vector<StringPointer>::const_iterator& it2,
	const std::vector<StringPointer>::const_iterator& end1,
	const std::vector<StringPointer>::const_iterator& end2)
{
	int off1, off2;
	for(off1=0; off1<std::min(end1-it1, end2-it2); ++off1)
	{
		for(off2=0; off2<off1; ++off2)
		{
			if(it1[off1]==it2[off2])
				{ it1+=off1; it2+=off2; return; }
			else if(it1[off2]==it2[off1])
				{ it1+=off2; it2+=off1; return; }
		}
		if(it1[off1]==it2[off1])
			{ it1+=off1; it2+=off1; return; }
	}

	for(; off1<end1-it1; ++off1)
		for(off2=0; off2<end2-it2; ++off2)
			if(it1[off1]==it2[off2])
				{ it1+=off1; it2+=off2; return; }

	for(; off1<end2-it2; ++off1)
		for(off2=0; off2<end1-it1; ++off2)
			if(it1[off2]==it2[off1])
				{ it1+=off2; it2+=off1; return; }
}

SequenceMultiIterator::SequenceMultiIterator(const SequenceMultiIterator& m):
	name_it(m.name_it),name_end(m.name_end),
	value_it(m.value_it),value_end(m.value_end)
	{}


SequenceMultiIterator::SequenceMultiIterator(const std::vector<const Sequence*>& s)
{
	std::vector<const Sequence*>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		if(*it!=NULL)
		{
			name_it.push_back((*it)->get_names().begin());
			name_end.push_back((*it)->get_names().end());
			value_it.push_back((*it)->get_values().begin());
			value_end.push_back((*it)->get_values().end());
		}
	}
	find_match();
}

SequenceMultiIterator::SequenceMultiIterator(const std::vector<Sequence*>& s)
{
	std::vector<Sequence*>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		if(*it!=NULL)
		{
			name_it.push_back((*it)->get_names().begin());
			name_end.push_back((*it)->get_names().end());
			value_it.push_back((*it)->get_values().begin());
			value_end.push_back((*it)->get_values().end());
		}
	}
	find_match();
}

SequenceMultiIterator::operator bool() const
{
	if(value_it.size()==0) return false;
	for(unsigned i=0; i<value_it.size()&&i<value_end.size(); ++i)
		if(value_it[i]==value_end[i]) return false;
	return true;
}

SequenceMultiIterator& SequenceMultiIterator::operator++()
{
	for(unsigned i=0; i<name_it.size()&&i<name_end.size(); ++i)
		if(name_it[i]!=name_end[i]) ++name_it[i];
	for(unsigned i=0; i<value_it.size()&&i<value_it.size(); ++i)
		if(value_it[i]!=value_end[i]) ++value_it[i];

	find_match();
	return *this;
}

SequenceMultiIterator SequenceMultiIterator::operator++(int)
{
	SequenceMultiIterator tmp=*this;
	operator++();
	return tmp;
}

float SequenceMultiIterator::operator[](unsigned i) const
{
	if(i<value_it.size()&&i<value_end.size()
		&&value_it[i]!=value_end[i])
		return *(value_it[i]);
	else return 0.0f;
}

StringPointer SequenceMultiIterator::name() const
{
	if(name_it.size()!=0&&name_end.size()!=0
		&&name_it[0]!=name_end[0])
		return *(name_it[0]);
	else return StringPointer();
}

void SequenceMultiIterator::find_match()
{
	std::vector<std::vector<StringPointer>::const_iterator>
		name_old=name_it;

	bool again=true;
	while(again)
	{
		again=false;

		for(unsigned i=0; i<name_it.size()&&i<name_end.size(); i+=2)
		{
			std::vector<StringPointer>::const_iterator
				it1=name_it[i];
			std::vector<StringPointer>::const_iterator
				it2=name_it[(i+1)%name_it.size()];

			SequenceDualIterator::find_match_helper(
				name_it[i], name_it[(i+1)%name_it.size()],
				name_end[i], name_end[(i+1)%name_it.size()]);

			if(it1!=name_it[i]||it2!=name_it[(i+1)%name_it.size()])
				again=true;
		}

		for(unsigned i=1; i<name_it.size()&&i<name_end.size(); i+=2)
		{
			std::vector<StringPointer>::const_iterator
				it1=name_it[i];
			std::vector<StringPointer>::const_iterator
				it2=name_it[(i+1)%name_it.size()];

			SequenceDualIterator::find_match_helper(
				name_it[i], name_it[(i+1)%name_it.size()],
				name_end[i], name_end[(i+1)%name_it.size()]);

			if(it1!=name_it[i]||it2!=name_it[(i+1)%name_it.size()])
				again=true;
		}
	}

	for(unsigned i=0; i<name_it.size()&&i<name_end.size()
		&&i<value_it.size()&&i<value_end.size(); i+=2)
	{

		if(name_it[i]-name_old[i]>value_end[i]-value_it[i])
			value_it[i]=value_end[i];
		else value_it[i]=value_it[i]+(name_it[i]-name_old[i]);
	}
}

}
