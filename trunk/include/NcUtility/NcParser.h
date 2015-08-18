#ifndef NC_PARSER
#define NC_PARSER

#include "NcUtility\NcGlobalDefinitions.h"

#include	<iostream>
#include	<stdlib.h>
#include	<assert.h>

#include	<QtOpenGl>
using namespace std;

using namespace DiscreteSimulator;


namespace DiscreteSimulator
{

	typedef enum _Symbol
	{
		eof=-10000,	
		eol,
		plus,
		minus,
		times,
		slash,
		eql,
		lparan,
		rparan,
		comma,
		semicolon,
		period,//-9990
		becomes,
		nul,
		neq,
		leq,
		lss,
		geq,
		gtr,
		hash,
		dollar,
		beginsym,
		callsym,//-9980
		constsym,
		dosym,
		endsym,
		ifsym,
		oddsym,
		procsym,
		thensym,
		varsym,
		whilesym,
		openb,//9970
		closeb,
		opensqb,
		closesqb,
		quote,
		
		
		G00sym=-8000,
		G01sym,
		G02sym,
		G03sym,
		G04sym,
		G06sym,
		G07sym,
		G08sym,
		G20sym,
		G21sym,
		G22sym,
		G28sym,
		G29sym,
		G30sym,
		G32sym,
		G76sym,
		G86sym,
		G90sym,
		G91sym,
		G92sym,
		G93sym,
		G94sym,
		G96sym,
		G97sym,

		M01sym,				//M-code
		M02sym,				
		M03sym,				
		M04sym,				
		M05sym,			
		M07sym,				
		M08sym,				
		M09sym,				
		M30sym,
		M98sym,				
		M99sym,				

		T01sym,			//Tool change
		T02sym,
		T03sym,
		T04sym,

		Xsym=-7900,
		Zsym,
		Dsym,
		Fsym=-7850,
		Usym,
		Wsym,
		Isym,
		Ksym,
		Nsym,
		Msym,
		LENsym,
		Asym,
		DIA_Xsym,
		DIA_Zsym,
		STARTsym,
		ENDsym,
		REPEATsym,
		PITCHsym,
		DELTA_Rsym,
		DELTA_Ksym,
		Ssym,
		//BlockSkipsym,


		solidsym=-7000,
			facetsym,
			normalsym,
			outersym,
			loopsym,
			vertexsym,
			endloopsym,
			endfacetsym,
			endsolidsym,


		number=6000,
		fnum,
		ident=-500

	}Symbol;

	typedef int Entry;

	typedef struct _Hashtab
	{
		char *lexptr;
		Symbol token;
	}Hashtab;


	class NcParser
	{
		public:
								NcParser();
			virtual				~NcParser();
			void				error(char msg[]);						/*For Error Reporting*/
			Hashtab*			create_htab(int estimate);				/*Create a lookup table*/
			int					enter_htab( Hashtab*	ht,				/*Add an entry to lookup table*/
											char		name[],
											Symbol		token);	
			Entry				find_htab(	Hashtab*		ht,			/*Find an entry in a lookup table*/
											char*			s);
			Symbol				get_htab_data(Entry entry);				/*Return data from a lookup table*/
			Symbol				getsym(void);
			int					init_scan(const char fn[]);
			int					read_ch (void);
			void				put_back(int );
			void				shutdown_scan();
			/*STATUS				get_values(Symbol token,codes* glist);*/
			int					getSign()			{return sign;}
			void				setSign(int _sign)	{sign = _sign;}
			int					getFid()			{return fid;}
			void				setFid(int id)		{fid = id;}
			FILE*				getFile()			{return source;}

		private:
			FILE*				source;									//the source file
			char				lastchar;
			int					lastentry;
			int					cur_line;
			int					cur_col;
			int					err_line;
			int					err_col;								//for error reporting
			int					sign;
			int					num;									//Last number read stored here for the parser
			char				id[MAX_ID];								//last identifier read stored here, for the parser
			double				fid;
			Hashtab*			keywords;
			Hashtab*			ht;
	};
}


#endif
