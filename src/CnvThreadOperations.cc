/*
 *      CnvThreadOperations.cc - this file is part of noise-free-cnv.
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

#include "CnvThreadOperations.hh"

#include "CnvThreadClasses.hh"
#include "CnvOperations.hh"
#include "CnvLoadSave.hh"
#include "PennCnvLoadSave.hh"

#include <glibmm.h>
#include <string>
#include <list>

/* This file defines threaded equivalents to the functions defined in
   CnvOperations. The actual functions consist of little more than locking the
   assosiated mutex, performing the underlying operatoion and unlocking. */

namespace Cnv { namespace Thread {

void load_namesvalues_thread(Sequence out, std::string f, StringPool pool)
{
	out.writer_lock();
	pool.writer_lock();
	if(out!=NULL) *out=Cnv::load(f, *pool);
	pool.writer_unlock();
	out.writer_unlock();
}
Sequence load_namesvalues(std::string f, StringPool pool)
{
	std::string name=f;
	if(name.rfind('/')<name.size())
		name=name.substr(name.rfind('/')+1, std::string::npos);
	if(name.rfind('\\')<name.size())
		name=name.substr(name.rfind('\\')+1, std::string::npos);

	Sequence out(name);
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		load_namesvalues_thread),pool),f),out), false);
	return out;
}

void save_namesvalues_thread(Sequence in, std::string f)
{
	in.reader_lock();
	if(in!=NULL) Cnv::save(*in, f);
	in.reader_unlock();
}
void save_namesvalues(const Sequence& in, std::string f)
{
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		save_namesvalues_thread),f),in), false);
}

void load_lrrbaf_thread(std::vector<Sequence> out, std::string f, StringPool pool)
{
	if(out.size()==2)
	{
		pool.writer_lock();
		std::vector<Cnv::Sequence> out_seq=PennCnv::load(f, *pool);
		pool.writer_unlock();
		if(out_seq.size()==2)
		{
			out[0].writer_lock();
			if(out[0]!=NULL) *out[0]=out_seq[0];
			out[0].writer_unlock();
			out[1].writer_lock();
			if(out[1]!=NULL) *out[1]=out_seq[1];
			out[1].writer_unlock();
		}
	}
}
std::vector<Sequence> load_lrrbaf(std::string f, StringPool pool)
{
	std::string name=f;
	if(name.rfind('/')<name.size())
		name=name.substr(name.rfind('/')+1, std::string::npos);
	if(name.rfind('\\')<name.size())
		name=name.substr(name.rfind('\\')+1, std::string::npos);

	std::vector<Sequence> out;
	out.push_back(Sequence(name+" - LRR"));
	out.push_back(Sequence(name+" - BAF"));
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		load_lrrbaf_thread),pool),f),out), false);
	return out;
}

void save_lrrbaf_thread(std::vector<Sequence> in, std::string s)
{
	if(in.size()==2)
	{
		std::vector<Cnv::Sequence> in_seqs;
		in_seqs.resize(2);
		in[0].reader_lock();
		if(in[0]!=NULL) in_seqs[0]=*in[0];
		in[0].reader_unlock();
		in[1].reader_lock();
		if(in[1]!=NULL) in_seqs[1]=*in[1];
		in[1].reader_unlock();
		PennCnv::save(in_seqs, s);
	}
}
void save_lrrbaf(const std::vector<Sequence>& o, std::string f)
{
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		save_lrrbaf_thread),f),o), false);
}

void add_thread(Sequence out, Sequence in, float p)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::add(*in, p);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence add(const Sequence& s, float p)
{
	std::string value_string;
	std::stringstream sstream; sstream<<p; sstream>>value_string;

	Sequence out(s.name+" + "+value_string);
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		add_thread),p),s),out), false);
	return out;
}

void mul_thread(Sequence out, Sequence in, float p)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::mul(*in, p);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence mul(const Sequence& s, float p)
{
	std::string value_string;
	std::stringstream sstream; sstream<<p; sstream>>value_string;

	Sequence out(s.name+" * "+value_string);
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		mul_thread),p),s),out), false);
	return out;
}

void sub_thread(Sequence out, Sequence in, float p)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::sub(*in, p);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence sub(const Sequence& s, float p)
{
	std::string value_string;
	std::stringstream sstream; sstream<<p; sstream>>value_string;

	Sequence out(s.name+" - "+value_string);
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		sub_thread),p),s),out), false);
	return out;
}

void div_thread(Sequence out, Sequence in, float p)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::div(*in, p);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence div(const Sequence& s, float p)
{
	std::string value_string;
	std::stringstream sstream; sstream<<p; sstream>>value_string;

	Sequence out(s.name+" / "+value_string);
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		div_thread),p),s),out), false);
	return out;
}

void pow_thread(Sequence out, Sequence in, float p)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::pow(*in, p);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence pow(const Sequence& s, float p)
{
	std::string value_string;
	std::stringstream sstream; sstream<<p; sstream>>value_string;

	Sequence out("pow( "+s.name+", "+value_string+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		pow_thread),p),s),out), false);
	return out;
}

void root_thread(Sequence out, Sequence in, float p)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::root(*in, p);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence root(const Sequence& s, float p)
{
	std::string value_string;
	std::stringstream sstream; sstream<<p; sstream>>value_string;

	Sequence out("root( "+s.name+", "+value_string+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		root_thread),p),s),out), false);
	return out;
}

void blur_thread(Sequence out, Sequence in, float p)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::blur(*in, p);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence blur(const Sequence& s, float p)
{
	std::string value_string;
	std::stringstream sstream; sstream<<p; sstream>>value_string;

	Sequence out("blur( "+s.name+", "+value_string+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		blur_thread),p),s),out), false);
	return out;
}

void trunc_thread(Sequence out, Sequence in, float p)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::trunc(*in, p);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence trunc(const Sequence& s, float p)
{
	std::string value_string;
	std::stringstream sstream; sstream<<p; sstream>>value_string;

	Sequence out("trunc( "+s.name+", "+value_string+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		trunc_thread),p),s),out), false);
	return out;
}

void cut_thread(Sequence out, Sequence in, float p)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::cut(*in, p);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence cut(const Sequence& s, float p)
{
	std::string value_string;
	std::stringstream sstream; sstream<<p; sstream>>value_string;

	Sequence out("cut( "+s.name+", "+value_string+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		cut_thread),p),s),out), false);
	return out;
}

void exp_thread(Sequence out, Sequence in)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::exp(*in);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence exp(const Sequence& s)
{
	Sequence out("exp( "+s.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		exp_thread),s),out), false);
	return out;
}

void log_thread(Sequence out, Sequence in)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::log(*in);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence log(const Sequence& s)
{
	Sequence out("log( "+s.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		log_thread),s),out), false);
	return out;
}

void abs_thread(Sequence out, Sequence in)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::abs(*in);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence abs(const Sequence& s)
{
	Sequence out("abs( "+s.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		abs_thread),s),out), false);
	return out;
}

void erf_thread(Sequence out, Sequence in)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::erf(*in);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence erf(const Sequence& s)
{
	Sequence out("erf( "+s.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		erf_thread),s),out), false);
	return out;
}

void sort_names_thread(Sequence out, Sequence in)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::sort_names(*in);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence sort_names(const Sequence& s)
{
	Sequence out("sort( "+s.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		sort_names_thread),s),out), false);
	return out;
}

void sort_values_thread(Sequence out, Sequence in)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::sort_values(*in);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence sort_values(const Sequence& s)
{
	Sequence out("sort( "+s.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		sort_values_thread),s),out), false);
	return out;
}

void avg_thread(Sequence out, Sequence in)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::avg(*in);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence avg(const Sequence& s)
{
	Sequence out("avg( "+s.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		avg_thread),s),out), false);
	return out;
}

void rank_thread(Sequence out, Sequence in)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::rank(*in);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence rank(const Sequence& s)
{
	Sequence out("rank( "+s.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		rank_thread),s),out), false);
	return out;
}

void stripXY_thread(Sequence out, Sequence in)
{
	out.writer_lock();
	in.reader_lock();
	if(in!=NULL&&out!=NULL) *out=Cnv::stripXY(*in);
	in.reader_unlock();
	out.writer_unlock();
}
Sequence stripXY(const Sequence& s)
{
	Sequence out("stripXY( "+s.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		stripXY_thread),s),out), false);
	return out;
}

void add_dual_thread(Sequence out, Sequence a, Sequence b)
{
	out.writer_lock();
	a.reader_lock();
	b.reader_lock();
	if(a!=NULL&&b!=NULL&&out!=NULL) *out=Cnv::add(*a, *b);
	b.reader_unlock();
	a.reader_unlock();
	out.writer_unlock();
}
Sequence add(const Sequence& a, const Sequence& b)
{
	Sequence out(a.name+" + "+b.name);
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		add_dual_thread),b),a),out), false);
	return out;
}

void mul_dual_thread(Sequence out, Sequence a, Sequence b)
{
	out.writer_lock();
	a.reader_lock();
	b.reader_lock();
	if(a!=NULL&&b!=NULL&&out!=NULL) *out=Cnv::mul(*a, *b);
	b.reader_unlock();
	a.reader_unlock();
	out.writer_unlock();
}
Sequence mul(const Sequence& a, const Sequence& b)
{
	Sequence out(a.name+" * "+b.name);
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		mul_dual_thread),b),a),out), false);
	return out;
}

void sub_dual_thread(Sequence out, Sequence a, Sequence b)
{
	out.writer_lock();
	a.reader_lock();
	b.reader_lock();
	if(a!=NULL&&b!=NULL&&out!=NULL) *out=Cnv::sub(*a, *b);
	b.reader_unlock();
	a.reader_unlock();
	out.writer_unlock();
}
Sequence sub(const Sequence& a, const Sequence& b)
{
	Sequence out(a.name+" - "+b.name);
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		sub_dual_thread),b),a),out), false);
	return out;
}

void div_dual_thread(Sequence out, Sequence a, Sequence b)
{
	out.writer_lock();
	a.reader_lock();
	b.reader_lock();
	if(a!=NULL&&b!=NULL&&out!=NULL) *out=Cnv::div(*a, *b);
	b.reader_unlock();
	a.reader_unlock();
	out.writer_unlock();
}
Sequence div(const Sequence& a, const Sequence& b)
{
	Sequence out(a.name+" / "+b.name);
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		div_dual_thread),b),a),out), false);
	return out;
}

void sort_dual_thread(Sequence out, Sequence a, Sequence b)
{
	out.writer_lock();
	a.reader_lock();
	b.reader_lock();
	if(a!=NULL&&b!=NULL&&out!=NULL) *out=Cnv::sort(*a, *b);
	b.reader_unlock();
	a.reader_unlock();
	out.writer_unlock();
}
Sequence sort(const Sequence& a, const Sequence& b)
{
	Sequence out("sort( "+a.name+", "+b.name+" )");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::bind(sigc::ptr_fun(
		sort_dual_thread),b),a),out), false);
	return out;
}

void add_multi_thread(Sequence out, std::vector<Sequence> s)
{
	out.writer_lock();
	std::vector<const Cnv::Sequence*> tmp;
	std::vector<Sequence>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		it->reader_lock();
		if(*it!=NULL) tmp.push_back(&**it);
	}
	if(out!=NULL) *out=Cnv::add(tmp);
	for(it=s.begin(); it!=s.end(); ++it) it->reader_unlock();

	out.writer_unlock();
}
Sequence add(const std::vector<Sequence>& s)
{
	Sequence out("add");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		add_multi_thread),s),out), false);
	return out;
}

void arithmetic_multi_thread(Sequence out, std::vector<Sequence> s)
{
	out.writer_lock();
	std::vector<const Cnv::Sequence*> tmp;
	std::vector<Sequence>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		it->reader_lock();
		if(*it!=NULL) tmp.push_back(&**it);
	}
	if(out!=NULL) *out=Cnv::arithmetic(tmp);
	for(it=s.begin(); it!=s.end(); ++it) it->reader_unlock();

	out.writer_unlock();
}
Sequence arithmetic(const std::vector<Sequence>& s)
{
	Sequence out("arithmetic");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		arithmetic_multi_thread),s),out), false);
	return out;
}

void mul_multi_thread(Sequence out, std::vector<Sequence> s)
{
	out.writer_lock();
	std::vector<const Cnv::Sequence*> tmp;
	std::vector<Sequence>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		it->reader_lock();
		if(*it!=NULL) tmp.push_back(&**it);
	}
	if(out!=NULL) *out=Cnv::mul(tmp);
	for(it=s.begin(); it!=s.end(); ++it) it->reader_unlock();

	out.writer_unlock();
}
Sequence mul(const std::vector<Sequence>& s)
{
	Sequence out("mul");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		mul_multi_thread),s),out), false);
	return out;
}

void geometric_multi_thread(Sequence out, std::vector<Sequence> s)
{
	out.writer_lock();
	std::vector<const Cnv::Sequence*> tmp;
	std::vector<Sequence>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		it->reader_lock();
		if(*it!=NULL) tmp.push_back(&**it);
	}
	if(out!=NULL) *out=Cnv::geometric(tmp);
	for(it=s.begin(); it!=s.end(); ++it) it->reader_unlock();

	out.writer_unlock();
}
Sequence geometric(const std::vector<Sequence>& s)
{
	Sequence out("geometric");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		geometric_multi_thread),s),out), false);
	return out;
}

void min_multi_thread(Sequence out, std::vector<Sequence> s)
{
	out.writer_lock();
	std::vector<const Cnv::Sequence*> tmp;
	std::vector<Sequence>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		it->reader_lock();
		if(*it!=NULL) tmp.push_back(&**it);
	}
	if(out!=NULL) *out=Cnv::min(tmp);
	for(it=s.begin(); it!=s.end(); ++it) it->reader_unlock();

	out.writer_unlock();
}
Sequence min(const std::vector<Sequence>& s)
{
	Sequence out("min");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		min_multi_thread),s),out), false);
	return out;
}

void max_multi_thread(Sequence out, std::vector<Sequence> s)
{
	out.writer_lock();
	std::vector<const Cnv::Sequence*> tmp;
	std::vector<Sequence>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		it->reader_lock();
		if(*it!=NULL) tmp.push_back(&**it);
	}
	if(out!=NULL) *out=Cnv::max(tmp);
	for(it=s.begin(); it!=s.end(); ++it) it->reader_unlock();

	out.writer_unlock();
}
Sequence max(const std::vector<Sequence>& s)
{
	Sequence out("max");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		max_multi_thread),s),out), false);
	return out;
}

void median_multi_thread(Sequence out, std::vector<Sequence> s)
{
	out.writer_lock();
	std::vector<const Cnv::Sequence*> tmp;
	std::vector<Sequence>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		it->reader_lock();
		if(*it!=NULL) tmp.push_back(&**it);
	}
	if(out!=NULL) *out=Cnv::median(tmp);
	for(it=s.begin(); it!=s.end(); ++it) it->reader_unlock();

	out.writer_unlock();
}
Sequence median(const std::vector<Sequence>& s)
{
	Sequence out("median");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		median_multi_thread),s),out), false);
	return out;
}

void deviation_multi_thread(Sequence out, std::vector<Sequence> s)
{
	out.writer_lock();
	std::vector<const Cnv::Sequence*> tmp;
	std::vector<Sequence>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		it->reader_lock();
		if(*it!=NULL) tmp.push_back(&**it);
	}
	if(out!=NULL) *out=Cnv::deviation(tmp);
	for(it=s.begin(); it!=s.end(); ++it) it->reader_unlock();

	out.writer_unlock();
}
Sequence deviation(const std::vector<Sequence>& s)
{
	Sequence out("deviation");
	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		deviation_multi_thread),s),out), false);
	return out;
}

void align_multi_thread(std::vector<Sequence> out, std::vector<Sequence> s)
{
	std::vector<const Cnv::Sequence*> tmp;
	std::vector<Sequence>::const_iterator it;
	for(it=s.begin(); it!=s.end(); ++it)
	{
		it->reader_lock();
		if(*it!=NULL) tmp.push_back(&**it);
	}
	std::vector<Cnv::Sequence> out_tmp=Cnv::align(tmp);
	for(it=s.begin(); it!=s.end(); ++it) it->reader_unlock();

	for(unsigned i=0; i<out.size()&&i<out_tmp.size(); ++i)
	{
		out[i].writer_lock();
		if(out[i]!=NULL) *(out[i])=out_tmp[i];
		out[i].writer_unlock();
	}
}
std::vector<Sequence> align(const std::vector<Sequence>& s)
{
	std::vector<Sequence> out; out.resize(s.size());
	for(unsigned i=0; i<s.size(); ++i)
		out[i]=Sequence("align( "+s[i].name+" )");

	Glib::Thread::create(sigc::bind(sigc::bind(sigc::ptr_fun(
		align_multi_thread),s),out), false);
	return out;
}

}}
