/*
*  Program: uci-analyser: a UCI-based Chess Game Analyser
*  Copyright (C) 1994-2015 David Barnes
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 1, or (at your option)
*  any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*  David Barnes may be contacted as D.J.Barnes@kent.ac.uk
*  http://www.cs.kent.ac.uk/people/staff/djb/
*
*  This software has been modified for compatibility with PGN Spy.  The original may be found at:
*  https://www.cs.kent.ac.uk/people/staff/djb/chessplag/
*
*/

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <assert.h>

using namespace std;

// Don't know whether these should be picked up from?
#define ASSERT_IS(x, y) assert((x) == (y))
#define ASSERT_NOT(x, y) assert((x) != (y))
#define FAIL(mesg) { cerr << mesg << endl; exit(-1); }

int strToInt(const string &str);

#endif
