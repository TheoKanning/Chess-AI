/* globals.h
* Theo Kanning
* Contains all global definitions of functions, macros, and data types
*/

#define DEBUG //Define debug mode for full assert function

#define PROGRAM_NAME "Chess-AI"
#define AUTHOR	"Theo Kanning"
#define VERSION_NO	0.1

#ifndef DEBUG
        #define ASSERT(x)
#else
#define ASSERT(x) \
               if (! (x)) \
               { \
                  cout << "ERROR!! Assert " << #x << " failed\n"; \
                  cout << " on line " << __LINE__  << "\n"; \
                  cout << " in file " << __FILE__ << "\n";  \
				  system("PAUSE"); \
			   }
#endif
