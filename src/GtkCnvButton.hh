/*
 *      GtkCnvButton.hh - this file is part of noise-free-cnv.
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

#ifndef _GTKCNVBUTTON_
#define _GTKCNVBUTTON_
#include "GtkCnvOutline.hh"
#include "CnvThreadClasses.hh"

#include <gtkmm.h>

/* These Widgets are used as Buttons in the GtkCnvPanel.hh file. Further
   explanation can be found there. */

namespace GtkCnv {

class MenuItemOpen1: public Gtk::MenuItem
{
public:
	MenuItemOpen1(const Glib::ustring& s, Outline& o,
		Cnv::Thread::StringPool sp, Cnv::Thread::Sequence(*f)
			(std::string,Cnv::Thread::StringPool))
		:Gtk::MenuItem(s),outline(o),string_pool(sp),function(f) {};

private:
	Outline& outline;
	Cnv::Thread::StringPool string_pool;
	Cnv::Thread::Sequence(*function)
		(std::string,Cnv::Thread::StringPool);

protected:
	virtual void on_activate();
};

class MenuItemOpen2: public Gtk::MenuItem
{
public:
	MenuItemOpen2(const Glib::ustring& s, Outline& o,
		Cnv::Thread::StringPool sp,
		std::vector<Cnv::Thread::Sequence>(*f)
			(std::string,Cnv::Thread::StringPool))
		:Gtk::MenuItem(s),outline(o),string_pool(sp),function(f) {};

private:
	Outline& outline;
	Cnv::Thread::StringPool string_pool;
	std::vector<Cnv::Thread::Sequence>(*function)
		(std::string,Cnv::Thread::StringPool);

protected:
	virtual void on_activate();
};

class MenuItemSave1: public Gtk::MenuItem
{
public:
	MenuItemSave1(const Glib::ustring& s, Outline& o,
		void(*f)(const Cnv::Thread::Sequence&,std::string))
		:Gtk::MenuItem(s),outline(o),function(f) {};

private:
	Outline& outline;
	void(*function)(const Cnv::Thread::Sequence&,std::string);

protected:
	virtual void on_activate();
};

class MenuItemSave2: public Gtk::MenuItem
{
public:
	MenuItemSave2(const Glib::ustring& s, Outline& o,
		void(*f)(const std::vector<Cnv::Thread::Sequence>&,std::string))
		:Gtk::MenuItem(s),outline(o),function(f) {};

private:
	Outline& outline;
	void(*function)
		(const std::vector<Cnv::Thread::Sequence>&,std::string);

protected:
	virtual void on_activate();
};

class ButtonSingle1: public Gtk::Button
{
public:
	ButtonSingle1(const Glib::ustring& s, Outline& o, Gtk::Entry& e,
		Cnv::Thread::Sequence(*f)(const Cnv::Thread::Sequence&,float))
		:Gtk::Button(s),outline(o),entry(e),function(f) {};

private:
	Outline& outline;
	Gtk::Entry& entry;
	Cnv::Thread::Sequence(*function)
		(const Cnv::Thread::Sequence&,float);

protected:
	virtual void on_clicked();
};

class ButtonSingle2: public Gtk::Button
{
public:
	ButtonSingle2(const Glib::ustring& s, Outline& o,
		Cnv::Thread::Sequence(*f)(const Cnv::Thread::Sequence&))
		:Gtk::Button(s),outline(o),function(f) {};

private:
	Outline& outline;
	Cnv::Thread::Sequence(*function)(const Cnv::Thread::Sequence&);

protected:
	virtual void on_clicked();
};

class ButtonDual: public Gtk::Button
{
public:
	ButtonDual(const Glib::ustring& s, Outline& o,
		Cnv::Thread::Sequence(*f)
			(const Cnv::Thread::Sequence&,const Cnv::Thread::Sequence&))
		:Gtk::Button(s),outline(o),function(f) {};

private:
	Outline& outline;
	Cnv::Thread::Sequence(*function)
		(const Cnv::Thread::Sequence&,const Cnv::Thread::Sequence&);

protected:
	virtual void on_clicked();
};

class ButtonMulti1: public Gtk::Button
{
public:
	ButtonMulti1(const Glib::ustring& s, Outline& o,
		Cnv::Thread::Sequence(*f)
			(const std::vector<Cnv::Thread::Sequence>&))
		:Gtk::Button(s),outline(o),function(f) {};

private:
	Outline& outline;
	Cnv::Thread::Sequence(*function)
		(const std::vector<Cnv::Thread::Sequence>&);

protected:
	virtual void on_clicked();
};

class ButtonMulti2: public Gtk::Button
{
public:
	ButtonMulti2(const Glib::ustring& s, Outline& o,
		std::vector<Cnv::Thread::Sequence>(*f)(const std::vector<Cnv::Thread::Sequence>&))
		:Gtk::Button(s),outline(o),function(f) {};

private:
	Outline& outline;
	std::vector<Cnv::Thread::Sequence>(*function)(const std::vector<Cnv::Thread::Sequence>&);

protected:
	virtual void on_clicked();
};

}

#endif
