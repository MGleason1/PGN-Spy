This software is intended as a tool to help detect cheating in chess by providing
information on engine correlation and other statistics.  Information obtained using
this software should not be taken as evidence of engine use without proper statistical
analysis involving comparison to appropriate benchmarks and consideration of any other
relevant evidence.

Source code can be found at http://github.com/MGleason1/PGN-Spy

Before using this software to analyse the games of suspected cheaters, it is recommended
that you first analyse a large number of games from elite grandmasters.  This will give
you appropriate baselines for comparison.  It is important that your analysis of suspected
cheaters is done with the same settings as those for your baselines, or the results
cannot be compared meaningfully.

Sources of pgn files for elite tournaments and players include the following:
http://www.pgnmentor.com/files.html#events - numerous elite tournaments, including some
historical ones.  Appears to be exclusively those with slow time controls.
http://www.pgnmentor.com/files.html#players - samples from various elite players,
 including some historical ones.
http://www.serverchess.com/worldch.htm - correspondence chess world championship
tournaments; those from the late pre-computer era are perhaps more useful than the
more recent events, in part due to the possibility of cheating in more recent events.
http://theweekinchess.com/chessnews/events - downloads from all major recent tournaments,
including some at faster time controls.
http://theweekinchess.com/twic - massive weekly downloads of many tournaments and games.

This software will scan a collection of games in pgn format, and calculate various statistics.

It will display results from two different data sets: 1) positions where neither side has
a large advantage, and 2) positions where one side has an advantage but the game is not
over yet.  Suspicious results in the first data set would be indicative of a cheater who
uses an engine until he is ahead, and then perhaps plays normally.  Suspicious results
in the second data set would be indicative of a cheater who uses an engine to try to
recover a losing position.

SETTINGS

Note: all thresholds should be entered in centipawns - 1/100th of a pawn.  Thus, 50 centipawns
is 1/2 of a pawn.

Input File - select the collection of games you want to analyse.  They should be in pgn format.

Player to investigate - enter the name of the player whose moves you want to analyse.  Ensure
the name is spelled the same way as in the pgn file.  If you leave this blank, all moves from
all players will be analysed; this is useful for establishing baselines from elite tournaments.

Exclude Forced Moves/Forced Move Threshold - This option is intended to minimize false positives
in highly tactical samples by eliminating positions with only one reasonable move from T1/T2/T3/etc.
statistics.  If this is enabled, if the second best move is worse than the best move by the specified
threshold, the position will be excluded from T1 results.  If the third-best move is worse than the
best move by the specified threshold, the position will be exclude from T2 results; the logic is
similar for T3/T4/T5, etc.

Include only unclear positions/Unclear position threshold - this option is also intended to minimize
false positives in highly tactical samples.  If the second best move is worse than the best move by
the specified threshold, the position will be excluded from T1 results.  If the third best move is
worse than the best move by the specified threshold, the position will be excluded from T2 results.
This is similar to the forced move setting; however, the force moved setting compares each move to
the next-best option (i.e. comparing the 2nd move to the 3rd move, comparing the 3rd move to the 4th
move); the unclear position setting compares each move to the best option.

Undecided position threshold - positions that are evaluated such that neither side has an advantage
greater than the specified threshold are reported in the results for undecided positions.  Positions
where one side or the other has a large advantage are excluded.

Losing position threshold - positions that are evaluated such that the player in question is losing by
more than the undecided position threshold but less than the losing position threshold are reported in
the results for losing positions.  All other positions are excluded.

Book depth - the number of opening moves to exclude from analysis.  Strong players will often have a
large number of moves memorised in certain openings.  Additionally, in correspondence chess, the
use of static resources such as books and databases is usually permitted, and so a weaker player may
well make strong moves in the opening, without using an engine.  Thus, opening moves are best excluded
from analysis to reduce false positives.  Note: this value is entered in moves, not in plies.  Thus, a
value of 10 means ten moves for both sides, not five moves each.

Variations - the number of variations to report for T1/T2/T3/etc. analysis.

Threads - the number of games to be analysing simultaneously.  On computers with multiple cores,
analysing multiple games simultaneously will enable a large database to be scanned more quickly.
Note: chess engines do not do as well relying on hyper-threading, so it is not recommended to set
this to a larger value than the number of actual physical cores on your machine.  You may also consider
reducing this number by one or two if you intend to use your computer for other purposes while
the analysis is running.
Also note: the engine will be run in single-threaded mode to aid consistency of results.

Hash size - the size of the engine's hash table.  See documentation for your engine for the optimal
value for this setting.  If the Threads setting is set to more than one, remember that each
instance of the engine will have its own hash table; for best results, this value should be set
such that the total size does not exceed the physical memory available on your machine.  You
should also leave sufficient for the operating system and any other software currently running.

Min/Max time per move/Min search depth - these settings specify how long the engine should spend
analysing each position.  It will analyse each position for at least the minimum time.  Then it will
continue analysis until it reaches either the minimum search depth or the maximum search time.
Note that the search depth should be entered in plies: one ply is one move for one player; two plies
is one move for each side.

Engine - select a UCI-compatible engine.  Most strong engines are UCI-compatible.  Stockfish is a
very strong UCI-compatible engine that is available for free: http://stockfishchess.org/download/

RESULTS

Undecided positions - positions where neither side is winning by more than the "Undecided positions threshold"
setting are included in this category.  A player who cheats until he gets ahead may have suspicious
results in this category.

Losing positions - positions where the player in question is losing by more than the undecided positions
threshold but less than the losing positions threshold are included in this category.  A player who
cheats to try to save a lost position may have suspicious results in this category.

Positions - the number of positions included in this category.

T1/T2/T3/etc. - The number of positions where the player made the top 1, 2, 3, etc. engine move.
Note that the number of positions here is less than the total positions; this is due to the
"Exclude forced moves" and "Include only unclear positions" settings.  This is also why the
number of positions for T1 is not the same as for T2, and neither is the same as T3.

>0 CP Loss - the number of positions where the move the player made was worse than the top
engine move by more than the specified value (in centipawns).

Avg centipawn loss - the average difference between the evaluation of the best move and
the move that was actually played.  Note that a move that concedes a forced mate will be
evaluated as -1000 - equivalent to being down 10 pawns with no compensation.

Confidence intervals for T1/T2/T3/etc. and blunder frequencies can be calculated
at http://statpages.info/confint.html.