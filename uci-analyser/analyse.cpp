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

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <errno.h>
#include <stdlib.h>

#include "string.h"
#include "engine.h"
#include "evaluation.h"
#include "utils.h"

using namespace std;

static const char *VERSION = "2014.06.02";

// The length of algebraic moves expected by a UCI engine.
#define ALGEBRAIC_MOVELEN 4

void extractInfo(string& info, vector<string> infoTokens, int searchDepth, int searchMaxTime, int searchMinTime, bool &reachedDepth, bool &beingMated, bool &engineCouldBeStuck);
bool readGame(istream &movestream, vector<string> &movelist,
              string& fenstring, int& bookDepth);
void sendGame(vector<string> &movelist, const string& fenstring, int bookDepth);
void tokenise(string &text, vector<string> &tokens);

void clearEvaluations(void);
void escapeForXML(string& str, const string& pattern, const string& escape_str);
void obtainEvaluations();
bool processMovesFile(const string& movesFile);
bool processStdin(void);
bool runEngine(vector<string> files);
void outputTag(const string& tagLine);
void saveEvaluation(Evaluation *ev, const string& info);
bool haveEvaluationForMove(const string &move);
bool showEvaluationsForMove(const string &move);
bool annotateMove(const string &playedMove);
void showUsage(const char *programName);
bool isResult(const string& move);

// Evaluations of the current move.
static vector<Evaluation *> evaluations;

// Settable by arguments.
static int defaultBookDepth = 8;
static unsigned numVariations = 5;
//the next three are related: we search for searchMinTime ms or until we reach searchDepth, whichever is longer, but always stop when we reach searchMaxTime
static int searchDepth = 20;
static int searchMaxTime = 5000;
static int searchMinTime = 1000;
static string engineName = "stockfish";
static map<string, string> engineOptions;
// Which colours we wish to analyse.
static bool analyseWhite = true;
static bool analyseBlack = true;

// A pseudo PGN tag giving the depth, in plies, of
// previous known play at the point this game was played.
static string BOOK_DEPTH_TAG = "BookDepth";
static string FEN_TAG = "FEN";

// Whether to output the game with annotations.
static bool annotate = false;

// The evaluation engine.
static Engine *engine;

#ifdef __unix__
int main(int argc, char *argv[]) {
#else
int main(int argc, char *argv[]) {
#endif

//    Sleep(30000); //to allow attaching the debugger
    bool ok = true;
    int argnum = 1;
    if (strncmp(argv[argnum], "--", 2) != 0)
       argnum = 0; //depending on how the process is launched, sometimes the exe path comes in argv[0]


    // Process --arguments.
    for (; argnum < argc && strncmp(argv[argnum], "--", 2) == 0;) {
        string arg(argv[argnum]);
        argnum++;

        if (arg == "--annotate") {
            annotate = true;
        } else if (arg == "--blackonly") {
            if (analyseBlack) {
                analyseWhite = false;
                analyseBlack = true;
            } else {
                cerr << arg << " << conflicts with --whiteonly" << endl;
                ok = false;
            }
        } else if (arg == "--bookdepth") {
            if (argnum < argc) {
                string depth(argv[argnum]);
                argnum++;
                defaultBookDepth = strToInt(depth);
            } else {
                cerr << "Missing value argument to " << arg << endl;
                ok = false;
            }
        } else if (arg == "--searchdepth") {
            if (argnum < argc) {
                string depth(argv[argnum]);
                argnum++;
                searchDepth = strToInt(depth);
            } else {
                cerr << "Missing value argument to " << arg << endl;
                ok = false;
            }
        } else if (arg == "--searchmaxtime") {
            if (argnum < argc) {
                string time(argv[argnum]);
                argnum++;
                searchMaxTime= strToInt(time);
            } else {
                cerr << "Missing value argument to " << arg << endl;
                ok = false;
            }
        } else if (arg == "--searchmintime") {
            if (argnum < argc) {
                string time(argv[argnum]);
                argnum++;
                searchMinTime = strToInt(time);
            } else {
                cerr << "Missing value argument to " << arg << endl;
                ok = false;
            }
        } else if (arg == "--engine") {
            if (argnum < argc) {
                engineName = argv[argnum];
                argnum++;
            } else {
                cerr << "Missing engine name argument to " << arg << endl;
                ok = false;
            }
        } else if (arg == "--help") {
            showUsage(argv[0]);
        } else if (arg == "--setoption") {
            if (argnum + 1 < argc) {
                string name(argv[argnum]);
                string value(argv[argnum + 1]);
                argnum += 2;

                engineOptions[name] = value;
            } else {
                cerr << "Missing name and/or value argument to " << arg << endl;
                ok = false;
            }
        } else if (arg == "--variations") {
            if (argnum < argc) {
                string vars(argv[argnum]);
                argnum++;
                numVariations = strToInt(vars);
            } else {
                cerr << "Missing value argument to " << arg << endl;
                ok = false;
            }
        } else if (arg == "--version") {
	    cout << "Version: " << VERSION << endl;
        } else if (arg == "--whiteonly") {
            if (analyseWhite) {
                analyseWhite = true;
                analyseBlack = false;
            } else {
                cerr << arg << " << conflicts with --blackonly" << endl;
                ok = false;
            }
        } else {
            cerr << "Unknown argument: " << arg << endl;
            ok = false;
        }
    }

    if (ok) {
        vector<string> files;
        for (; argnum < argc; argnum++) {
            files.push_back(argv[argnum]);
        }
        ok = runEngine(files);
        return ok ? 0 : -1;
    } else {
        showUsage(argv[0]);
        return -2;
    }
}

void showUsage(const char *programName) {
    cerr << "Usage: " << programName << "\n" <<
            "    [--annotate]\n" <<
            "        output the game with evaluation annotations\n" <<
            "    [--blackonly]\n" <<
            "        only analyse black's moves\n" <<
            "    [--bookdepth depth]\n" <<
            "        depth in ply to skip at start of game\n" <<
            "    [--searchdepth depth]\n" <<
            "        search depth in ply\n" <<
            "    [--searchmaxtime depth]\n" <<
            "        search max time in milliseconds\n" <<
            "    [--searchmintime depth]\n" <<
            "        search min time in milliseconds\n" <<
            "    [--engine program]\n" <<
            "        program to use as the UCI engine\n" <<
            "    [--help]\n" <<
            "        show this usage message\n" <<
            "    [--setoption optionName optionValue]\n " << 
            "        set a UCI option\n" <<
            "    [--variations vars]\n" << 
            "        number of variations to analyse per move\n" <<
	    "    [--version]\n" <<
	    "        show the current version\n" <<
            "    [--whiteonly]\n" << 
            "        only analyse white's moves\n" <<
            "    file [...]\n" <<
            "        file(s) to be analysed." <<
            endl;
    exit(-2);
}

/*
 * Run the engine on the given files.
 * Return true if everything was ok.
 */
bool runEngine(vector<string> files) {
    bool ok = true;
    try {
        engine = new Engine(engineName);
        if (engine->initEngine(numVariations, searchDepth, searchMaxTime, searchMinTime, engineOptions)) {
	    cout << "<gamelist>" << endl;
            if (files.size() > 0) {
                // Process each file of moves.
                for (unsigned i = 0; i < files.size(); i++) {
                    const string& movesFile = files[i];
                    if (!annotate) {
                        cout << "<!-- Processing " << movesFile << "-->" << endl;
                    }
                    if (!processMovesFile(movesFile)) {
                        cerr << "Failed to process " << movesFile << endl;
                        ok = false;
                    }
                    clearEvaluations();
                }
            } else {
                if (!processStdin()) {
                    ok = false;
                }
                clearEvaluations();
            }
        } else {
            cerr << "Failed to initialise the chess engine." << endl;
            ok = false;
        }
		cout << "</gamelist>" << endl;

        engine->quitEngine();
        delete engine;
        engine = NULL;
    }    catch (int e) {
        e; //squelch compiler warning
        cerr << "Failed to start " << engineName << endl;
        ok = false;
    }

    return ok;
}

/*
 * Process the long-algebraic moves in movesFile.
 */
bool processMovesFile(const string& movesFile) {
    ifstream movestream(movesFile.c_str());
    if (movestream.is_open()) {
        vector<string> moves;
	string fenstring;
	int bookDepth;
        while (readGame(movestream, moves, fenstring, bookDepth)) {
            sendGame(moves, fenstring, bookDepth);
        }
        movestream.close();
        return true;
    } else {
        cerr << "Unable to open " << movesFile << endl;
        return false;
    }
}

/*
 * Process the long-algebraic moves on stdin
 */
bool processStdin(void) {
    vector<string> moves;
    string fenstring;
    int bookDepth;
    while (readGame(cin, moves, fenstring, bookDepth)) {
        sendGame(moves, fenstring, bookDepth);
    }
    return true;
}

/* Extract the string between quotes as the value of a tag. */
static string extractTagValue(const string& line)
{
    unsigned first_quote = line.find_first_of("\"");
    if (first_quote != string::npos) {
	unsigned second_quote = line.find_first_of("\"", first_quote + 1);
	if (second_quote != string::npos) {
	    return string(line.substr(first_quote + 1, second_quote - first_quote - 1).c_str());
	} else {
	    cerr << "Missing second quote in " << line << endl;
	    return "";
	}
    } else {
	cerr << "Missing first quote in " << line << endl;
	return "";
    }
}

/*
 * Read moves for one game from movestream and store them in the moves vector.
 * Set any FEN tag value and book depth.
 * The end of a game is marked by a zero-length line or the end of file.
 * Return true on success; false otherwise.
 */
bool readGame(istream &movestream, vector<string> &movelist,
              string& fenstring, int& bookDepth) {
    string line;
    bool endOfGame = false;
    movelist.clear();
    fenstring = "";
    bookDepth = defaultBookDepth;

    // Skip to the tags (if any).
    while (movestream.good() && getline(movestream, line) &&
            (line.size() == 0 || line[0] != '[')) {
        if (line.size() > 0) {
            cerr << "Unknown text before game: " << line << endl;
        }
    }
    if (!movestream.good() || line.size() == 0 || line[0] != '[') {
        return false;
    }
    cout << "<game>" << endl;
    cout << "<tags>" << endl;

    while (line.size() > 0 && line[0] == '[' && !endOfGame) {
        if (line.find(BOOK_DEPTH_TAG) != string::npos) {
            // Set the bookdepth from this tag.
	    string tag_value = extractTagValue(line);
	    if(tag_value.length() > 0) {
	      int depth = atoi(tag_value.c_str());
	      // Sanity check.
	      if(depth >= 0 && depth < 100) {
		  bookDepth = depth;
	      }
	      else {
		  cerr << "Unrealistic book depth: " << depth << endl;
	      }
	    }
        }
        else if (line.find(FEN_TAG) != string::npos) {
	    fenstring = extractTagValue(line);
	}
        // Output tags.
	outputTag(line);
        if (!movestream.good() || !getline(movestream, line)) {
            endOfGame = true;
        }
    }

    cout << "</tags>" << endl;

    if (!annotate) {
        cout << "<moves>" << endl;
    }

    while (movestream.good() && !endOfGame) {
        if (!annotate && line.size() > 0) {
            cout << line << endl;
        }
        if (line.size() > 0) {
            stringstream moves(line);
            string move;
            while (moves >> move) {
                // Drop any suffixes, such as + and #
                // but retain promotions.
                if (isResult(move)) {
                    movelist.push_back(move);
                    endOfGame = true;
                } else {
                    if (move.length() > ALGEBRAIC_MOVELEN) {
                        char ch = move[ALGEBRAIC_MOVELEN];
                        if (ch == '+' || ch == '#') {
                            move.resize(ALGEBRAIC_MOVELEN);
                        } else {
                            // Enforce lowercase.
                            ch = tolower(ch);
                            move[ALGEBRAIC_MOVELEN] = ch;
                            move.resize(ALGEBRAIC_MOVELEN + 1);
                        }
                    }
                    movelist.push_back(move);
                }
            }
        } else {
            // Blank line.
        }
        if (!movestream.good() || !getline(movestream, line)) {
            endOfGame = true;
        }
    }

    if (!annotate) {
        cout << "</moves>" << endl;
    }
    return movelist.size() > 0;
}

/*
 * Send the moves of a game, one by one, to the engine.
 */
void sendGame(vector<string> &movelist, const string& fenstring, int bookDepth) {
    if (annotate) {
        cout << "<annotation>" << endl;
    } else {
        cout << "<analysis " <<
                "bookDepth = \"" << bookDepth << "\" " <<
                "searchDepth = \"" << searchDepth << "\" " <<
                "searchMaxTime = \"" << searchMaxTime << "\" " <<
                "searchMinTime = \"" << searchMinTime << "\" " <<
                "variations = \"" << numVariations << "\" " <<
                ">" << endl;
    }
    engine->startNewGame();

    // Don't include the result.
    int numMoves = movelist.size() - 1;
    // The string of moves that is gradually built up.
    string moves = "";
    bool white = true;
    // Index into moves from the list.
    int moveCount = 0;

    int movesToSkip = 0;
    // Beware of very short games.
    if (bookDepth > numMoves) {
        movesToSkip = numMoves;
    }
    else {
        movesToSkip = bookDepth;
    }
    // bookDepth does not really apply if a non-starting-position
    // FEN string has been included.
    // NB: Strictly speaking, we should look at the move number of
    // the FEN string and adjust book depth appropriately if the
    // starting position is still considered to be within book,
    // but this is complex to do and would apply in very few cases.
    // So be pragmatic.
    if(fenstring.length() > 0) {
        movesToSkip = 0;

        int moveFlag = fenstring.find(" ") + 1;
        if (fenstring[moveFlag] == 'b' || fenstring[moveFlag] == 'B')
           white = false;
    }
    // Skip over the portion considered to be book.
    while (moveCount < movesToSkip) {
        if (annotate) {
            cout << movelist[moveCount] << " ";
        }
        moves.append(movelist[moveCount]);
        moves.append(" ");
        moveCount++;
        white = !white;
    }

    for (; moveCount < numMoves; moveCount++) {
        const string& playedMove = movelist[moveCount];
        // Only analyse a move for a particular colour if required.
        if ((white && analyseWhite) || (!white && analyseBlack)) {
            // Ask the engine to analyse the current position.
            engine->clearHash();
            engine->setPosition(moves, fenstring);
            engine->go();

            // Start with a fresh set of evaluations.
            clearEvaluations();
            obtainEvaluations();

            // Make sure we have an analysis for the played move.
            if (!haveEvaluationForMove(playedMove)) {
                // The played move wasn't analysed, so force it to be.
                engine->clearHash();
                engine->setPosition(moves, fenstring);
                engine->searchMoves(playedMove);
                // Add it to the current ones.
                obtainEvaluations();
            }

	    bool playedMoveEvaluated;
            if (annotate) {
                playedMoveEvaluated = annotateMove(playedMove);
            } else {
                playedMoveEvaluated = showEvaluationsForMove(playedMove);
            }
	    if(!playedMoveEvaluated) {
		cerr << "Internal error: " << playedMove <<
			" was not evaluated." << endl;
	    }
        }
        moves.append(playedMove);
        moves.append(" ");
        white = !white;
    }
    if (annotate) {
        // Output the result.
        cout << movelist[movelist.size() - 1] << endl;
        cout << "</annotation>" << endl;
    } else {
        cout << "</analysis>" << endl;
    }
    cout << "</game>" << endl;
}

/*
 * Extract the information from an info line returned
 * by the engine.
 */
void extractInfo(string &info, vector<string> infoTokens, int searchDepth, int searchMaxTime, int searchMinTime, bool &reachedDepth, bool &beingMated, bool &engineCouldBeStuck) {
    reachedDepth = false;
    ASSERT_IS("info", infoTokens[0]);
    if (info.find("multipv ") != string::npos) {
        int numTokens = infoTokens.size();
        int t = 1;
        while (t < numTokens && infoTokens[t] != "depth") {
            t++;
        }
        int i = 1;
        while (i < numTokens && infoTokens[i] != "time") {
            i++;
        }
        int multipvIndex = 1, multipv;
        while (multipvIndex +1 < numTokens && infoTokens[multipvIndex] != "multipv")
           multipvIndex++;
        multipv = atoi(infoTokens[multipvIndex + 1].c_str());
        if (multipv != 1)
           engineCouldBeStuck = false; //if we're stuck, we only ever report one variation
        if (info.find("mate") != string::npos)
        {
           //there's a forced mate; check if it's happening to us on our best move
           int mate = 1;
           while (mate+1 < numTokens && infoTokens[mate] != "mate")
              mate++;
           if (multipv == 1 && atoi(infoTokens[mate + 1].c_str()) < 0)
              beingMated = true;
        }
        if (t < numTokens) {
            int depth = strToInt(infoTokens[t + 1]);
            int time = strToInt(infoTokens[i + 1]);
            bool engineStuck = engineCouldBeStuck && depth > 50; //if we've at depth 50 and never been given another variation, assume engine's stuck and bail out
            if (time < searchMinTime && !beingMated && !engineStuck)
                return; //keep searching at least until we hit min time - unless we're mated by force or engine's stuck
            if (depth < searchDepth && time <= searchMaxTime && !beingMated && !engineStuck)
                return; //keep searching until we hit either search depth or max time - unless we're mated by force or engine's stuck
            reachedDepth = true;
            Evaluation *ev = new Evaluation(infoTokens, info);
            saveEvaluation(ev, info);
        }
    }
}

/*
 * Do we have an evaluation for the played move?
 */
bool haveEvaluationForMove(const string &move) {
    for (vector<Evaluation *>::iterator it = evaluations.begin();
            it != evaluations.end(); it++) {
        const Evaluation *ev = *it;
        if (ev->getFirstMove() == move) {
            return true;
        }
    }
    return false;
}

/*
 * Show the evaluations for the position before the
 * given played move.
 * Return whether the played move was evaluated.
 */
bool showEvaluationsForMove(const string &playedMove) {
    cout << "<move>" << endl;
    cout << "<played";
    // cout << " time = " << '"' << evaluations[0]->getTime() << '"' ;
    cout << '>';
    cout << playedMove << "</played>" << endl;

    // Whether the playedMove was one of those evaluated.
    bool playedMoveEvaluated = false;

    for (vector<Evaluation *>::iterator it = evaluations.begin();
            it != evaluations.end(); it++) {
        const Evaluation *ev = *it;
        string firstMove = ev->getFirstMove();
        if (ev != NULL) {
            if (!annotate) {
                cout << "<evaluation move = " << '"' << firstMove << '"';
                cout << " depth = " << '"' << ev->getDepth() << '"';
                cout << " time = " << '"' << ev->getTime() << '"';
		cout << " value = " << '"';
                if (ev->isForcedMate()) {
                    cout << "mate " << ev->getNumMateMoves();
                } else {
                    cout << ev->getValue();
                    if (ev->isLowerBound()) {
                        cout << " L";
                    } else if (ev->isUpperBound()) {
                        cout << " U";
                    }
                }
		cout << "\" />";
                cout << endl;
            }
            if (firstMove == playedMove) {
                playedMoveEvaluated = true;
            }
        }
    }
    cout << "</move>" << endl;
    return playedMoveEvaluated;
}

/*
 * Show the evaluation for the played move and the best alternative.
 * Return whether the played move was evaluated.
 */
bool annotateMove(const string &playedMove) {
    vector<Evaluation *>::iterator it = evaluations.begin();
    while (it != evaluations.end() && (*it)->getFirstMove() != playedMove) {
        it++;
    }
    if (it != evaluations.end()) {
        const Evaluation *ev = *it;

        cout << playedMove << " ";
        cout << "{ " << ev->getValue() << " } ";

        // Print out the better alternative moves and their evaluations.
        it = evaluations.begin();
        int bestValue = (*it)->getValue();
        do {
            ev = *it;
            if (ev->getFirstMove() != playedMove) {
                cout << "( " << ev->getFirstMove() <<
                        " { " << ev->getValue() << " }) ";
            }
            it++;
        } while (it != evaluations.end() && (*it)->getValue() >= bestValue);
	cout << endl;
	return true;
    } else {
	cout << endl;
	return false;
    }
}

/**
 * Output the name and value of a tag.
 * Ensure that special characters are escaped to fit with
 * XML.
 */
void outputTag(const string& tagLine)
{
    // The tag name.
    int first_quote = tagLine.find_first_of("\"");
    int last_quote = tagLine.find_last_of("\"");
    cout << "<tag name = \"";
    cout << tagLine.substr(1, first_quote - 2) << "\"";

    // The value.
    cout << " value = ";
    string value = tagLine.substr(first_quote + 1, last_quote - first_quote - 1);
    escapeForXML(value, "&", "&amp;");
    // Escape any quote characters.
    string escaped_quote("\\\"");
    size_t qindex = value.find_first_of(escaped_quote);
    while(qindex != string::npos) {
        value.replace(qindex, 2, "&quot;");
	qindex = value.find_first_of(escaped_quote);
    }
    cout << "\"" << value << "\"" << " />" << endl;
}

/*
 * Escape occurrences of pattern in str with escape_str.
 */
void escapeForXML(string& str, const string& pattern, const string& escape_str)
{
    size_t pattern_length = pattern.length();
    size_t escape_str_length = escape_str.length();
    size_t qindex = str.find_first_of(pattern);
    while(qindex != string::npos) {
        str.replace(qindex, pattern_length, escape_str);
	qindex = str.find_first_of(pattern, qindex + escape_str_length);
    }
}

/*
 * Store the given evaluation in the evaluations array.
 */
void saveEvaluation(Evaluation *ev, const string& info) {
    bool inserted = false;
    vector<Evaluation *>::iterator it = evaluations.begin();
    // Look for an existing occurrence.
    const string& move = ev->getFirstMove();
    while (it != evaluations.end() && (*it)->getFirstMove() != move) {
        it++;
    }
    if (it != evaluations.end()) {
        // Delete the previous one.
        evaluations.erase(it);
    }
    // Insert in decreasing order.
    it = evaluations.begin();
	while (!inserted && it != evaluations.end()) {
        Evaluation *current = *it;
        // Mating moves require special treatment.
        // Can't just compare scores.
        if (ev->isForcedMate() || current->isForcedMate()) {
            bool insert_here = false;

            if (ev->isForcedMate()) {
                if (current->isForcedMate()) {
                    // Both forced mate.
                    bool ev_wins = ev->getNumMateMoves() >= 0;
                    bool current_wins = current->getNumMateMoves() >= 0;
                    if (ev_wins == current_wins) {
                        // Both either win or lose.
                        // Prefer shortest win or slowest loss.
                        insert_here = ev->getNumMateMoves() <=
                                current->getNumMateMoves();
                    } else {
                        if (ev_wins) {
                            insert_here = true;
                        } else {
                            // Prefer the current win.
                        }
                    }
                } else {
                    // ev is forced but current is not.
                    if (ev->getNumMateMoves() >= 0) {
                        // Forced win.
                        insert_here = true;
                    } else {
                        // Forced loss.
                    }
                }
            } else {
                // current is mate but ev is not.
                if (current->getNumMateMoves() < 0) {
                    // current is a forced loss, so prefer ev.
                    insert_here = true;
                } else {
                    // current is a forced win.
                }
            }
            if (insert_here) {
                evaluations.insert(it, ev);
                inserted = true;
            }
        } else if (ev->getValue() > current->getValue()) {
            evaluations.insert(it, ev);
            inserted = true;
        } else {
            // No insertion here.
        }
        if (!inserted) {
            it++;
        }
    }

    if (!inserted) {
        evaluations.push_back(ev);
    }
}

/*
 * Obtain the evaluations from the engine.
 * Fill out the evaluations array.
 */
void obtainEvaluations() {
    string reply;
    vector<string> tokens;
    bool bestMoveFound = false;
    bool beingMated = false;
    bool engineCouldBeStuck = true; //when being mated by force or drawn by force, sometimes the engine gets stuck
    bool eof = false;
    bool engineStopped = false;

    do {
        reply = engine->getResponse(eof);
        if (!eof) {
            // Break up the reply.
            tokens.clear();
            tokenise(reply, tokens);
            if (tokens.size() > 0) {
                string tokenType = tokens[0];
                if (tokenType == "info") {
                    bool reachedDepth = false;
                    extractInfo(reply, tokens, searchDepth, searchMaxTime, searchMinTime, reachedDepth, beingMated, engineCouldBeStuck);
                    if ((reachedDepth || beingMated) && !engineStopped)
                    {
                        engineStopped = true;
                        engine->stop();
                    }
                } else if (tokenType == "bestmove") {
                    bestMoveFound = true;
                }
            }
        }
    } while (!bestMoveFound && !eof);
}

/*
 * Clear the evaluations array.
 */
void clearEvaluations(void) {
    for (vector<Evaluation *>::iterator it = evaluations.begin();
            it != evaluations.end(); it++) {
        Evaluation *eval = *it;
        if (eval != NULL) {
            delete eval;
        }
    }
    evaluations.clear();
}

/*
 * Tokenise text into tokens.
 * tokens is not cleared by this function.
 */
void tokenise(string &text, vector<string> &tokens) {
    stringstream ss(text);
    string token;
    while (getline(ss, token, ' ')) {
        if (token != "") {
            tokens.push_back(token);
        }
    }
}

bool isResult(const string& move) {
    return move == "1-0" ||
            move == "0-1" ||
            move == "1/2-1/2" ||
            move == "*";
}
