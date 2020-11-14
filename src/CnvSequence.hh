/*
 *      CnvSequence.hh - this file is part of noise-free-cnv.
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

#ifndef _CNVSEQUENCE_
#define _CNVSEQUENCE_
#include "CnvStringPool.hh"
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

class Sequence
{
public:
	typedef float value_type;

	void push_back(StringPointer s, value_type value);
	void push_back(value_type value);

	const std::vector<StringPointer>& get_names() const;
	const std::vector<value_type>& get_values() const;

	void reserve(size_t size) { values.reserve(size); };
	unsigned size() const {return values.size(); }

private:
	std::vector<StringPointer> names;
	std::vector<value_type> values;
};


class SequenceSingleIterator
{
public:

	SequenceSingleIterator(const SequenceSingleIterator& m);
	SequenceSingleIterator(const Sequence& s);

	operator bool() const;

	SequenceSingleIterator& operator++();
	SequenceSingleIterator operator++(int);

	float value() const;
	StringPointer name() const;

private:

	std::vector<StringPointer>::const_iterator name_it;
	std::vector<StringPointer>::const_iterator name_end;
	std::vector<float>::const_iterator value_it;
	std::vector<float>::const_iterator value_end;
};

class SequenceDualIterator
{
public:

	SequenceDualIterator(const SequenceDualIterator& m);
	SequenceDualIterator(const Sequence& s, const Sequence& t);

	operator bool() const;

	SequenceDualIterator& operator++();
	SequenceDualIterator operator++(int);

	float first() const;
	float second() const;
	StringPointer name() const;

	static void find_match_helper(
		std::vector<StringPointer>::const_iterator& it1,
		std::vector<StringPointer>::const_iterator& it2,
		const std::vector<StringPointer>::const_iterator& end1,
		const std::vector<StringPointer>::const_iterator& end2);

private:

	void find_match();

	std::vector<StringPointer>::const_iterator name_it1, name_it2;
	std::vector<StringPointer>::const_iterator name_end1, name_end2;
	std::vector<float>::const_iterator value_it1, value_it2;
	std::vector<float>::const_iterator value_end1, value_end2;
};

class SequenceMultiIterator
{
public:

	SequenceMultiIterator(const SequenceMultiIterator& m);
	SequenceMultiIterator(const std::vector<const Sequence*>& s);
	SequenceMultiIterator(const std::vector<Sequence*>& s);

	operator bool() const;

	SequenceMultiIterator& operator++();
	SequenceMultiIterator operator++(int);

	float operator[](unsigned i) const;
	StringPointer name() const;

private:

	void find_match();

	std::vector<std::vector<StringPointer>::const_iterator> name_it;
	std::vector<std::vector<StringPointer>::const_iterator> name_end;
	std::vector<std::vector<float>::const_iterator> value_it;
	std::vector<std::vector<float>::const_iterator> value_end;
};

}

#endif
