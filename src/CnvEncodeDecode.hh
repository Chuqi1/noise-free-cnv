/*
 *      CnvEncodeDecode.hh - this File is part of noise-free-cnv.
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

#ifndef _CNVENCODEDECODE_
#define _CNVENCODEDECODE_
#include <string>

/* This file is used for encoding data into character strings and to decode
   character strings of that kind. */

namespace Cnv {

unsigned char decode_chr(std::string::const_iterator i,
	std::string::const_iterator end);

unsigned char decode_chr(const std::string& s);

std::string encode_chr(unsigned char c);

unsigned decode_pos(std::string::const_iterator i,
	std::string::const_iterator end);

unsigned decode_pos(const std::string& s);

std::string encode_pos(unsigned p);

float decode_float_value(std::string::const_iterator i,
	std::string::const_iterator end);

float decode_float_value(const std::string s);

std::string encode_float_value(float v);

std::string compose_point_name(const std::string& id,
	const std::string& c, const std::string& p);

std::string compose_point_name(const std::string& n,
	unsigned char chr, unsigned pos);

void decompose_point_name(const std::string& n, std::string& id,
	std::string& chr, std::string& pos);

void decompose_point_name(const std::string& n, std::string& id,
	unsigned char& chr, unsigned& pos);

}
#endif
