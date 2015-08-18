#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<math.h>
#include	<QDebug>

#include "NcUtility\NcParser.h"

NcParser::NcParser()
{
	source			=	0;	//the source file
	lastchar		=	-1;
	lastentry		=	0;
	cur_line		=	0 ;
	cur_col			=	0;
	err_line		=	0;
	err_col			=	0;	//for error reporting
	sign			=	0;
	num				=	0;	//Last number read stored here for the parser
	fid				=	0;
	keywords		=	NULL;
	ht				=	NULL;
}
NcParser::~NcParser()
{
}

int NcParser::read_ch (void)
{
	int ch = fgetc(source);
	//printf("Char read %c\n",ch);
	cur_col++;
	
	if (ch=='\n')
	{
		cur_line++;
		cur_col=0;
	}
	return ch;
}

void NcParser::put_back(int ch)
{
	ungetc(ch,source);
	cur_col--;
	if(ch == '\n')
		cur_line--;
}

Symbol NcParser::getsym(void)
{
	int ch,id_len;
	while((ch = read_ch()) != EOF && (ch <= ' ' && ch != 10));
	err_line = cur_line;
	err_col = cur_col;
	char r = (char)(ch);
	QString str = ".3";
     double val = str.toDouble(); 
	switch(ch)
		{
			
		case 10: return eol;
		case EOF : return eof;
		case '+' : return plus;
		case '-' : sign=1;
			return minus;
		case '*' : return times;
		case '/' : return slash;
		case '=' : return eql;
		case '(' : return lparan;
		case ')' : return rparan;
		case ',' : return comma;
		case ';' : return semicolon;
		case '.' : return period;
		case '#' : return hash;
		case '$' : return dollar;
		case '{' : return openb;
		case '}' : return closeb;
		case '[' : return opensqb;
		case ']' : return closesqb;
		case '"' : return quote;
		case 'N' : return Nsym;
		case 'M' : return Msym;
		case 'F' : return Fsym;
	//	case 'X' : return Xsym;
	//	case 'Z' : return Zsym;
	//	case 'I' : return Isym;
		case ':' :
			ch=read_ch();
			return (ch == '=') ? becomes : nul;
		case '<' :
			ch = read_ch();
			if (ch == '>')
				return neq;
			if(ch == '=')
				return leq;
			put_back(ch);
			return lss;
		case '>' : 
			ch=read_ch();
			if(ch == '=')
				return geq;
			put_back(ch);
			return gtr;
		default :
			if (isdigit(ch))
			{
				num=0;
				do
				{
					//no checking for overflow
					num=10 * num + ch - '0';
				}while((ch = read_ch())!=EOF && isdigit(ch));
				
				
				if(ch=='.')
				{	
					int cn=1;
					double mantesa=0;
					
					if(isdigit(ch=read_ch())){
					do
					{
					//no checking for overflow
				 	mantesa=mantesa+(double)(ch-'0')/(double)(pow(10.0,cn));
					cn++;
					}while((ch = read_ch())!=EOF && isdigit(ch));
					}
					
					
					if(ch=='e'||ch=='E'||ch=='D')
					{
						int exp=0; 
						int flag=0;
						if((ch=read_ch())== '+' )flag=0;
						else if (ch== '-' )flag=1;
						
						if(isdigit(ch=read_ch()))
					do{
						exp=10*exp + ch -'0';
						}while((ch=read_ch())!=EOF && isdigit(ch));
						
						flag==1?
						(fid=(num+mantesa)*(double)pow(10.0,-exp)):
						(fid=(num+mantesa)*(double)pow(10.0,exp));
						
						put_back(ch);
						return fnum;
					}
					
					else{
						fid=(double)(num+mantesa);
						put_back(ch);
						return fnum;
					}
				}
				
				else if(ch=='e'||ch=='E')
				{
					int exp=0; 
					int flag=0;
					
					if((ch=read_ch())=='+')flag=0;
					else if (ch=='-')flag=1;
					
					if(isdigit(ch=read_ch())){
						do{
							exp=10*exp + ch -'0';
						}while((ch=read_ch())!=EOF && isdigit(ch));
						if(flag==1){
						fid=num*(double)pow(10.0,-exp);
						put_back(ch);
						return fnum;
						}
					}
					put_back(ch);	
					return fnum ;
					
				}	
				fid=num;				
				put_back(ch);
				return fnum;
			}

			if (isalpha(ch))
			{
				Entry entry;
				id_len = 0;
				do
				{
					if(id_len < MAX_ID)
					{
						id[id_len] = (char) ch;
						id_len++;
					}
				}while ((ch=read_ch()) != EOF && isalnum(ch));
				id[id_len]='\0';
				put_back(ch);
				entry = find_htab(keywords,id);
				return entry?get_htab_data(entry): ident ;
			}
			error ("getsym : invalid character '%c'");
			printf("char not identified %d\n",ch);
			return nul;
	}
}
Hashtab* NcParser::create_htab(int estimate)
{
	Hashtab *p=NULL;
	p=(Hashtab *)calloc(estimate,sizeof(Hashtab));
	assert(p);
	return p;
} 

int	NcParser::enter_htab(Hashtab *ht, char name[],Symbol token)
{
	int len;
	len=strlen(name);
	lastentry++;

	ht[lastentry].token = token;
	ht[lastentry].lexptr = (char *)malloc(len * sizeof(char));
	strncpy(ht[lastentry].lexptr, name, len);
	return lastentry;
}

Entry NcParser::find_htab(Hashtab *ht,char *s)
{
	int i;
	for(i=1;i<=lastentry;i++)
		if(strncmp(ht[i].lexptr,s,strlen(s))==0)
			return i;
	
	return 0;
}

Symbol NcParser::get_htab_data(Entry entry)
{
	return  keywords[entry].token;
}

void NcParser::error(char m[])
{
	fprintf(stderr,"line %d %s\n",cur_line,m);
	return;
}

int NcParser::init_scan(const char fn[])
{
		source = fopen(fn, "r");
	
        if (source == NULL)
		{
            qDebug()<<fn<<"file could not be opened"<<endl;
            return 0;
        }
	cur_line = 1;
	cur_col = 0;
	keywords = create_htab(1000);
	enter_htab (keywords, "G00", G00sym);
	enter_htab (keywords, "G01", G01sym);
	enter_htab (keywords, "G02", G02sym);
	enter_htab (keywords, "G03", G03sym);
	enter_htab (keywords, "G04", G04sym);
	enter_htab (keywords, "G06", G06sym);
	enter_htab (keywords, "G07", G07sym);
	enter_htab (keywords, "G08", G08sym);
	enter_htab (keywords, "G20", G20sym);
	enter_htab (keywords, "G21", G21sym);
	enter_htab (keywords, "G22", G22sym);
	enter_htab (keywords, "G28", G28sym);
	enter_htab (keywords, "G29", G29sym);
	enter_htab (keywords, "G30", G30sym);
	enter_htab (keywords, "G32", G32sym);
	enter_htab (keywords, "G76", G76sym);
	enter_htab (keywords, "G86", G86sym);
	enter_htab (keywords, "G90", G90sym);
	enter_htab (keywords, "G91", G91sym);
	enter_htab (keywords, "G92", G92sym);
	enter_htab (keywords, "G93", G93sym);
	enter_htab (keywords, "G94", G94sym);
	enter_htab (keywords, "G96", G96sym);

	enter_htab (keywords, "M01", M01sym);
	enter_htab (keywords, "M02", M02sym);
	enter_htab (keywords, "M03", M03sym);
	enter_htab (keywords, "M04", M04sym);
	enter_htab (keywords, "M05", M05sym);
	enter_htab (keywords, "M07", M07sym);
	enter_htab (keywords, "M08", M08sym);
	enter_htab (keywords, "M09", M09sym);
	enter_htab (keywords, "M30", M30sym);
	enter_htab (keywords, "M98", M98sym);
	enter_htab (keywords, "M99", M99sym);
	
	enter_htab (keywords, "T01", T01sym);
	enter_htab (keywords, "T02", T02sym);
	enter_htab (keywords, "T03", T03sym);
	enter_htab (keywords, "T04", T04sym);

	enter_htab (keywords, "X", Xsym);
	enter_htab (keywords, "Z", Zsym);
	enter_htab (keywords, "D", Dsym);
	enter_htab (keywords, "F", Fsym);
	enter_htab (keywords, "U", Usym);
	enter_htab (keywords, "W", Wsym);
	enter_htab (keywords, "I", Isym);
	enter_htab (keywords, "K", Ksym);
	enter_htab (keywords, "N", Nsym);
	enter_htab (keywords, "M", Msym);
	enter_htab (keywords, "LEN", LENsym);
	enter_htab (keywords, "A", Asym);
	enter_htab (keywords, "DIA_X", DIA_Xsym);
	enter_htab (keywords, "DIA_Z", DIA_Zsym);
	enter_htab (keywords, "START", STARTsym);
	enter_htab (keywords, "END", ENDsym);
	enter_htab (keywords, "REPEAT", REPEATsym);
	enter_htab (keywords, "PITCH", PITCHsym);
	enter_htab (keywords, "DELTA_R", DELTA_Rsym);
	enter_htab (keywords, "DELTA_K", DELTA_Ksym);
	enter_htab (keywords, "S", Ssym);

	enter_htab (keywords, "solid", solidsym);
        enter_htab (keywords, "facet", facetsym);
        enter_htab (keywords, "normal", normalsym);
        enter_htab (keywords, "outer", outersym);
        enter_htab (keywords, "loop", loopsym);
        enter_htab (keywords, "vertex", vertexsym);
        enter_htab (keywords, "endloop", endloopsym);
        enter_htab (keywords, "endfacet", endfacetsym);
        enter_htab (keywords, "endsolid", endsolidsym);	
	return 1;
}

void NcParser::shutdown_scan()
{
	lastchar=-1;
	lastentry=0;
	cur_line = 0;
	cur_col = 0;
	assert(keywords);
	free(keywords);
	assert(source);
	fclose(source);
}	
