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

#include <iostream>
#include <sstream>

#include "evaluation.h"
#include "utils.h"

using namespace std;

/*
 * Extract the evaluation details from an "info"
 * output from the engine.
 */
void Evaluation::extractInfo(const vector<string> &tokens, const string& info) {
    int numTokens = tokens.size();
    this->cpValue = 0;
    this->lowerBound = false;
    this->upperBound = false;
    this->variation = 0;
    this->forcedMate = false;
    this->mateInMoves = 0;

    for (int t = 1; t < numTokens;) {
        string token = tokens[t];
        t++;
        if (token == "depth") {
            string value = tokens[t];
            t++;
            this->depth = strToInt(value);
        } else if (token == "seldepth") {
            string depth = tokens[t];
            t++;
        } else if (token == "score") {
            string scoreType = tokens[t];
            t++;
            if (scoreType == "cp") {
                string value = tokens[t];
                t++;
                cpValue = strToInt(value);
            } else if (scoreType == "mate") {
                string moves = tokens[t];
                t++;

                forcedMate = true;
                mateInMoves = strToInt(moves);
            }
            string nextToken = tokens[t];
            if (nextToken == "lowerbound") {
                lowerBound = true;
                t++;
            } else if (nextToken == "upperbound") {
                upperBound = true;
                t++;
            }
        } else if (token == "multipv") {
            string multipv = tokens[t];
            t++;
            this->variation = strToInt(multipv);
        } else if (token == "nodes") {
            this->nodes = strToInt(tokens[t]);
            t++;
        } else if (token == "nps") {
            string nps = tokens[t];
            t++;
        } else if (token == "pv") {
            // Remaining tokens are the moves of the line.
            for (; t < numTokens; t++) {
                this->moves.push_back(tokens[t]);
            }
        } else if (token == "time") {
            this->time = tokens[t];
            t++;
        } else {
           //unrecognised token; ignore instead of reporting error
//            cout << "# Unrecognised token " << token << " in: " << endl;
//            cout << "# " << info << endl;
        }
    }
}
