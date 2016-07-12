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

#ifndef EVALUATION_H
#define EVALUATION_H

#include <string>
#include <vector>

using namespace std;

/*
 * Details of the evaluation of a line.
 */
class Evaluation
{
  public:
    Evaluation(const vector<string> &tokens, const string &info)
    {
        extractInfo(tokens, info);
    }

    virtual ~Evaluation()
    {
    }

    inline unsigned getVariation() const
    {
        return variation;
    }

    inline int getDepth() const
    {
        return depth;
    }

    inline int getValue() const
    {
        return cpValue;
    }

    inline bool isForcedMate() const
    {
        return forcedMate;
    }

    inline bool isLowerBound() const
    {
        return lowerBound;
    }

    inline bool isUpperBound() const
    {
        return upperBound;
    }

    inline int getNumMateMoves() const
    {
        return mateInMoves;
    }

    inline string getFirstMove() const
    {
        return moves[0];
    }

    inline string getTime() const
    {
        return time;
    }

  private:
      // The variation number.
      unsigned variation;
      // The depth.
      int depth;
      // The value in centi-pawns.
      int cpValue;
      // This value is a lower bound.
      bool lowerBound;
      // This value is an upper bound.
      bool upperBound;
      // The number of nodes.
      int nodes;
      // The time
      string time;
      // Whether the move gives forced mate.
      bool forcedMate;
      // Number of moves to mate (if forcedMate)
      int mateInMoves;
      // The moves of the line being evaluated.
      vector<string> moves;

      void extractInfo(const vector<string> &tokens, const string &info);
};

#endif
