This directory contains the source code for a UCI-based analyser
which is available from: http://www.cs.kent.ac.uk/~djb/chessplag/
Copyright (C) David J. Barnes 2013-2014 d.j.barnes@kent.ac.uk
http://www.cs.kent.ac.uk/~djb/

Overview
========
The purpose of the analyser is to read a source file containing moves of
one or more chess games (properly encoded - see below) and pass them to
a UCI-compatible chess engine. The analyser's program arguments are used to
configure the engine for search depth, number of candidate moves, etc.

The analyser receives back the engine's evaluations and writes them out in
an XML format for processing by another program.

Installation
============
A Makefile is provided for installation on Unix/Linux/cygwin environments that
have a C++ compiler. There is no configuration file; type:

make

Usage
=====
The analyser takes 0 or more command-line options (see below) and 0 or more game files.
If no game files are provided then it reads games from standard input:

    analyse [optional-command-line-options] game-file ...

The analyser requires as input games written with PGN headers. However, UCI engines
expect the moves of a game to be formatted in long-algebraic notation, so games should
be provided to the analyser in the form shown below under Input Format.

The easiest way to convert a game into this format is to use a tool such as
pgn-extract, available from: http://www.cs.kent.ac.uk/~djb/pgn-extract/

Its -Wuci flag will output the moves of a game in a format suitable for the
analyser to pass on to a UCI engine. For instance:

pgn-extract -Wuci -oout.pgn games.pgn

outputs the PGN games from games.pgn to the file out.pgn, which can then be passed
to the analyser.

Command-line Options
====================
The UCI engine to use should be specified via --engine with either the full pathname of
the engine program, or a name that will be found in your environment's executable search
path. The default name is "stockfish".

--annotate
    output the game with evaluation annotations
--blackonly
    only analyse black's moves
--bookdepth depth
    depth in ply to skip at start of game
--searchdepth depth
    search depth in ply
--engine program
    program to use as the UCI engine
--help 
    show this usage message
--setoption optionName optionValue 
    set a UCI option
--variations vars
    number of variations to analyse per move
--whiteonly
    only analyse white's moves

Input Format
============
A game in the format expected by the analyser - note that the moves have been
(re)written into long-algebraic form.

[Event "World Championship 23th"]
[Site "Moscow"]
[Date "1960.03.17"]
[Round "2"]
[White "Botvinnik, Mikhail"]
[Black "Tal, Mihail"]
[Result "1/2-1/2"]
[BookDepth "17"]

d2d4 g8f6 c2c4 c7c5 d4d5 e7e6 b1c3 e6d5 c4d5 d7d6 g1f3 g7g6 c1g5 f8g7 f3d2 h7h6
g5h4 g6g5 h4g3 f6h5 d2c4 h5g3 h2g3 e8g8 e2e3 d8e7 f1e2 f8d8 e1g1 b8d7 a2a4 d7e5
c4e5 e7e5 a4a5 a8b8 a1a2 c8d7 c3b5 d7b5 e2b5 b7b6 a5a6 b8c8 d1d3 c8c7 b2b3 e5c3
d3c3 g7c3 a2c2 c3f6 g3g4 c7e7 c2c4 d8c8 g2g3 f6g7 f1d1 c8f8 d1d3 g8h7 g1g2 h7g6
d3d1 h6h5 g4h5 g6h5 g3g4 h5g6 c4c2 f8h8 b5d3 g6f6 g2g3 e7e8 d3b5 e8e4 c2c4 e4c4
b3c4 f6e7 b5a4 g7e5 g3f3 h8h4 d1g1 f7f5 1/2-1/2

Output Format
=============
The analysis of each game is output in XML format. The output includes the game details

<game>
<tags>
<tag name = "Event" value = "World Championship 23th" />
<tag name = "Site" value = "Moscow" />
<tag name = "Date" value = "1960.03.17" />
<tag name = "Round" value = "2" />
<tag name = "White" value = "Botvinnik, Mikhail" />
<tag name = "Black" value = "Tal, Mihail" />
<tag name = "Result" value = "1/2-1/2" />
<tag name = "BookDepth" value = "17" />
</tags>
<moves>
d2d4 g8f6 c2c4 c7c5 d4d5 e7e6 b1c3 e6d5 c4d5 d7d6 g1f3 g7g6 c1g5 f8g7 f3d2 h7h6
g5h4 g6g5 h4g3 f6h5 d2c4 h5g3 h2g3 e8g8 e2e3 d8e7 f1e2 f8d8 e1g1 b8d7 a2a4 d7e5
c4e5 e7e5 a4a5 a8b8 a1a2 c8d7 c3b5 d7b5 e2b5 b7b6 a5a6 b8c8 d1d3 c8c7 b2b3 e5c3
d3c3 g7c3 a2c2 c3f6 g3g4 c7e7 c2c4 d8c8 g2g3 f6g7 f1d1 c8f8 d1d3 g8h7 g1g2 h7g6
d3d1 h6h5 g4h5 g6h5 g3g4 h5g6 c4c2 f8h8 b5d3 g6f6 g2g3 e7e8 d3b5 e8e4 c2c4 e4c4
b3c4 f6e7 b5a4 g7e5 g3f3 h8h4 d1g1 f7f5 1/2-1/2
</moves>
...
</game>

This is following by the analysis, which includes the settings of the engine:

<analysis bookDepth = "17" searchDepth = "10" variations = "5" > ... </analysis>

The evaluation of each played move and the required number of alternatives follow:

<move>
<played>g6g5</played>
<evaluation move = "e8g8" value = "-88" />
<evaluation move = "d8e7" value = "-109" />
<evaluation move = "a7a6" value = "-121" />
<evaluation move = "b8d7" value = "-121" />
<evaluation move = "b8a6" value = "-133" />
<evaluation move = "g6g5" value = "-133" />
</move>

Evaluations are typically in centipawns.

Annotating a Game
=================
The analyser can be used to produce an annotated version of a game using the --annotate flag.
In this case, the game score is output with centipawn evaluations of the moves and alternatives
in comments. An example is found in example-annotated.xml.

David J. Barnes
10th April 2014
