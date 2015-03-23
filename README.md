# Chess-AI
Chess AI by Theo Kanning

Version Notes: 
1.0 - All basic features working, plays first complete game in Arena 
1.1 - Piece square transitions and pawn structure evaluation added 
1.15 - Transposition table added 
1.16 - Fixed pv table bug in release mode 
1.20 - PV line maintained through hash table 
1.21 - Added null move 
1.22 - Added killer heuristic 
1.23 - Quiescent search includes queen promotions
1.24 - Fixed serious hash bug 
	 - Improved get_next_move function
	 - In check extension for depth > 0
	 - Moved hash logic out of alpha beta
	 - Added ply adjustment for mate scores
	 - Check for hash draw error in all pv nodes
	 - Fixed hash probe bug
1.25 - Changed time margin for ending
     - Late move reduction, reduce depth by 1
1.26 - Futility pruning at pre-frontier nodes (depth = 2) (not tuned)
	 - PVS researches are pv nodes 
	 - Uses incremental piece square tables
	 - Basic king safety pawn shield
	 - Store mate scores in table
	 - Options for endgame piece square tables (not tuned)
	 - Mate scores tracked correctly
1.27 - Score draw after 1 rep if not in root
	 - Removed redundant material  terms
	 - Added basic material draw detection
	 - Use only big pieces for phase
	 - Encourage trading while ahead
	 - Score adjustments for bishop, knight, and rook pairs
1.28 - Can be told to quit mid-search
	 - Won't null prune if mate found
	 - Can change options before entering UCI loop
	 - Tuned passed pawn and king endgame values
1.30 - Won't see if move causes check unless pruning is available
	 - Faster under attack function
	 - Better movelist clearing
	 - Piece-square move ordering
	 - SEE added, but not fully tested
	 - Two-tiered hash table, still depth-based
	 - No LMR in PV nodes
	 - Call PVS research as PV
	 - Mate score pruning
1.40 - SEE working
	 - Fixed futility pruning bug
	 - Increased mate score to prevent error with 9 queens (never going to happen)
	 - Full window search until alpha is raised (removed in 2.0)
2.00 - Magic bitboard move generation
	 - All 120 index data removed
	 - Pawn hash added
	 - Streamlined settings
	 - Check for draw in quiescent search
	 - 




