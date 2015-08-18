#include "NcGeomKernel\NcProfile.h"
#include <iostream>
using namespace std;
using namespace DiscreteSimulator;

Profile::Profile()
{
		P = NULL;
		S = NULL;
		unitnormal = NULL;
		no_pts = 0;
		type = UNKNOWN;
		coolant1 = false;
		coolant2 = false;
		typeTool = UNKNOWNTOOL;

		mAssocitedDBDLIndexes = new QList<GLuint>;
		mAssociated2DDLIndexes = new QList<GLuint>;

		mNoOfDBDL = 0;	//number of deformed body display lists
						//to avoid keeping track of no of DB DL in individual codes
}


void Profile::allocate()
{
	if(P)
	{
		#ifdef debug
       	std::cout << "Already allocated\n";
		#endif
	}

    try{
            P = new double* [no_pts];

            for(int i=0;i<no_pts;i++)
			{
            	P[i]= new double [3];
            }
        }
	catch(std::bad_alloc xa)
	{
		#ifdef debug
       	std::cout<<"Memory allocation failed for P\n";
		#endif
    }
    
	try{
           unitnormal = new double** [MAX + 1];

			for(int i = 0; i < MAX + 1; i++)
			{
                unitnormal[i] = new double* [no_pts];
            	for(int j = 0; j < no_pts; j++)
				{
            		unitnormal[i][j] = new double [3];
            	}
		}

	}
	catch(std::bad_alloc xa)
	{
		#ifdef debug
       	std::cout<<"Memory allocation failed for normal\n";
		#endif
    }

	try
	{	
		S = new double ** [MAX+1];
        for(int i = 0; i < MAX+1; i++)
		{
            S[i] = new double* [no_pts];
            for(int j=0;j<no_pts;j++)
			{
                    S[i][j]=new double [3];
			}
         }
     }
	catch(std::bad_alloc xa)
	{
		#ifdef debug
       	std::cout<<"Memory allocation failed for S\n";
		#endif
    }
}


void Profile::free_allocate()
{
	if(P)
	{	
    	for(int i=0;i<no_pts;i++)       //deleting P
            delete [] P[i];
    	delete [] P;
	}

	if(unitnormal)
	{
		for(int i=0;i<MAX+1;i++)
		{       //deleting unitnormal
			for(int j=0;j<no_pts;j++)
				delete [] unitnormal[i][j];
                   	delete [] unitnormal[i];
		}
		delete [] unitnormal;
	}
	
	if(S)
	{
       	for(int i=0;i<(MAX+1);i++)
		{         //deleting S
			for(int j=0;j<no_pts;j++)
				delete [] S[i][j];
                   	delete [] S[i];
         }
	     delete [] S;
	}

	P = NULL;
	S = NULL;
	unitnormal = NULL;
	no_pts = 0;
}

Profile::~Profile()
{
	free_allocate();
}

