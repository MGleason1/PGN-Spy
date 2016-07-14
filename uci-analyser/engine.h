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

#ifndef ENGINE_H
#define ENGINE_H


#include <iostream>
#ifdef __unix__
#include <stdio.h>
#include <unistd.h>
#else
#include "stdafx.h"
#include <atlstr.h>
#include <windows.h>
#endif

#include <string>
#include <map>

using namespace std;

class Engine {
public:

    Engine(const string& engineName) {
        if (!startEngine(engineName)) {
            throw -1;
        }
    }

    virtual ~Engine() {
    }

    bool checkIsReady(void);
    string getResponse(bool& eof);
    void clearHash(void);
    void go(void);
    void stop(void);
    bool initEngine(int variations, int searchDepth, int searchMaxTime, int searchMinTime,
            map<string, string>& options);
    void quitEngine(void);
    void searchMoves(const string& moves);
    void send(const string& str);
    void send(const char *str);
    void setPosition(const string& moves, const string& fenstring);
    void setFENPosition(const string& fenstring, const string& moves);
    void setOption(const string& name, const string& value);
    void setOption(const string& name, int value);
    void setOptions(map<string, string>& options);

    inline void startNewGame(void) {
        send("ucinewgame");
        checkIsReady();
    }
    bool waitForResponse(const char *str);

private:
#ifdef __unix__
    // The PID of the engine process.
    pid_t enginePID;
    // Communication to and from the engine.
    FILE *toEngine, *fromEngine;
#else
	HANDLE writeToEngine;
	HANDLE readFromEngine;
#endif
//    string communications;
    // The number of variations to analyse.
    int variations;
    // The depth to analyse.
    int searchDepth;
    int searchMaxTime;
    int searchMinTime;

    bool startEngine(const string&);
};

#endif

