/*
 *      CnvThreadWrap.hh - this file is part of noise-free-cnv.
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

#ifndef _CNVTHREADWRAP_
#define _CNVTHREADWRAP_
#include <glibmm.h>

/* The RefPtr template is used in the CnvThreadClasses.hh file to implement
   threaded versions of Cnv::Sequence, Cnv::PainterStatic and
   Cnv::PainterDynamic. It is basically a smart pointer with an read write
   lock to handle multi threaded access. */

namespace Cnv { namespace Thread {

template<class T> class RefPtr
{
public:

	RefPtr();
	RefPtr(const RefPtr& w);
	~RefPtr();

	RefPtr<T>& operator=(const RefPtr<T>& w);

	void reader_lock() const;
	bool reader_trylock() const;
	void reader_unlock() const { core->rwlock.reader_unlock(); }
	void writer_lock() { core->rwlock.writer_lock(); }
	bool writer_trylock() { return core->rwlock.writer_trylock(); }
	void writer_unlock();

	T* operator->() { return &core->data; }
	const T* operator->() const { return &core->data; }
	T& operator*() { return core->data; }
	const T& operator*() const { return core->data; }
	operator T*() { return &core->data; }
	operator const T*() const { return &core->data; }

	void allow_reading()
	{
		core->ref_count_mutex.lock();
		core->allow_reading=true;
		core->ref_count_mutex.unlock();
		core->cond.broadcast();
	}

private:

	class Core
	{
	public:

		T data;
		unsigned ref_count;
		bool allow_reading;

		mutable Glib::RWLock rwlock;
		mutable Glib::Mutex ref_count_mutex;
		mutable Glib::Cond cond;
	};

	Core* core;
};

template<class T> RefPtr<T>::RefPtr()
{
	core=new Core;
	core->ref_count=1;
	core->allow_reading=false;
}

template<class T> RefPtr<T>::RefPtr(const RefPtr<T>& w)
{
	w.core->ref_count_mutex.lock();
	w.core->ref_count++;
	w.core->ref_count_mutex.unlock();
	core=w.core;
}

template<class T> RefPtr<T>::~RefPtr()
{
	core->ref_count_mutex.lock();
	unsigned ref_count=--core->ref_count;
	core->ref_count_mutex.unlock();

	if(ref_count==0) delete core;
	else core->cond.broadcast();
}

template<class T> RefPtr<T>& RefPtr<T>::operator=(const RefPtr<T>& w)
{
	w.core->ref_count_mutex.lock();
	w.core->ref_count++;
	w.core->ref_count_mutex.unlock();

	core->ref_count_mutex.lock();
	unsigned ref_count=--core->ref_count;
	core->ref_count_mutex.unlock();
	if(ref_count==0) delete core;
	else core->cond.broadcast();

	core=w.core;
	return *this;
}

template<class T> void RefPtr<T>::reader_lock() const
{
	core->ref_count_mutex.lock();
	while(!core->allow_reading)
		core->cond.wait(core->ref_count_mutex);
	core->ref_count_mutex.unlock();
	core->rwlock.reader_lock();
}

template<class T> bool RefPtr<T>::reader_trylock() const
{
	core->ref_count_mutex.lock();
	bool allow_reading=core->allow_reading;
	core->ref_count_mutex.unlock();

	if(allow_reading) return core->rwlock.reader_trylock();
	else return false;
}

template<class T> void RefPtr<T>::writer_unlock()
{
	core->rwlock.writer_unlock();
	allow_reading();
}

}}

#endif
