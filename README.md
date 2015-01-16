# Chess-AI
Chess AI by Theo Kanning

Version Notes: 
1.0 - All basic features working, plays first complete game in Arena 
1.1 - Piece square transitions and pawn structure evaluation added 
1.15 - Transposition table added 
1.16 - Fixed pv table bug in release mode 
1.2 - PV line maintained through hash table 
1.21 - Added null move 
1.22 - Added killer heuristic 
1.23 - Quiescent search includes queen promotions
1.24 - Fixed serious hash bug 
	 - Improved get_next_move function
	 - In check extension for depth > 0
	 - Moved hash logic out of alpha beta
	 - Added ply adjustment for mate scores

