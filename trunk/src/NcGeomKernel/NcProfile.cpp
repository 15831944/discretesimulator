#include "NcGeomKernel\NcProfile.h"
//#include"NcUtility\NcVector.h"

#include <iostream>
#include <vector>
using namespace std;
using namespace DiscreteSimulator;

Profile::Profile()
{

		no_pts = 0;
		type = UNKNOWN;
		coolant1 = false;
		coolant2 = false;
		typeTool = UNKNOWNTOOL;
		//mMaterial = UNKNOWN;
		mAssocitedDBDLIndexes = new QList<GLuint>;
		mAssociated2DDLIndexes = new QList<GLuint>;

		mNoOfDBDL = 0;	//number of deformed body display lists
						//to avoid keeping track of no of DB DL in individual codes
}

STATUS	Profile::normalvector()
{
	double v1[3], v2[3];
	double length;
	int i, k;
	this->unitnormal.resize(MAX+1,std::vector<NcVector>(this->no_pts));
	for(int j = 0; j < MAX; j++)
	{
		for(k = 0; k < this->no_pts - 1; k++)
		{
			for(i = 0; i < 3; i++)
			{
				v1[i] = this->S[j][k+1][i] - this->S[j][k][i];

				if(j+1 == MAX)
				{
					v2[i] = this->S[0][k][i] - this->S[j][k][i];
				}
				else
				{
					v2[i] = this->S[j+1][k][i] - this->S[j][k][i];
				}
			}

			this->unitnormal[j][k][0] = v1[1] * v2[2] - v1[2] * v2[1];
			this->unitnormal[j][k][1] = v1[2] * v2[0] - v1[0] * v2[2];
			this->unitnormal[j][k][2] = v1[0] * v2[1] - v1[1] * v2[0];

			length = sqrt(this->unitnormal[j][k][0] * this->unitnormal[j][k][0]
							+ this->unitnormal[j][k][1] * this->unitnormal[j][k][1]
							+ this->unitnormal[j][k][2] * this->unitnormal[j][k][2]);
			if(length==0.0)
				length=1.0;

			this->unitnormal[j][k][0] = this->unitnormal[j][k][0] / length;
			this->unitnormal[j][k][1] = this->unitnormal[j][k][1] / length;
			this->unitnormal[j][k][2] = this->unitnormal[j][k][2] / length;
		}
	}
	return OK;
}


//void Profile::allocate()
//{ 
//	P.resize(no_pts);
//	S.resize(MAX+1,P);
//	unitnormal.resize(MAX+1,P);
//	/*if(P)
//	{
//		#ifdef debug
//       	std::cout << "Already allocated\n";
//		#endif
//	}
//
//    try{
//            P = new double* [no_pts];
//
//            for(int i=0;i<no_pts;i++)
//			{
//            	P[i]= new double [3];
//            }
//        }
//	catch(std::bad_alloc xa)
//	{
//		#ifdef debug
//       	std::cout<<"Memory allocation failed for P\n";
//		#endif
//    }
//    
//	try{
//           unitnormal = new double** [MAX + 1];
//
//			for(int i = 0; i < MAX + 1; i++)
//			{
//                unitnormal[i] = new double* [no_pts];
//            	for(int j = 0; j < no_pts; j++)
//				{
//            		unitnormal[i][j] = new double [3];
//            	}
//		}
//
//	}
//	catch(std::bad_alloc xa)
//	{
//		#ifdef debug
//       	std::cout<<"Memory allocation failed for normal\n";
//		#endif
//    }
//
//	try
//	{	
//		S = new double ** [MAX+1];
//        for(int i = 0; i < MAX+1; i++)
//		{
//            S[i] = new double* [no_pts];
//            for(int j=0;j<no_pts;j++)
//			{
//                    S[i][j]=new double [3];
//			}
//         }
//     }
//	catch(std::bad_alloc xa)
//	{
//		#ifdef debug
//       	std::cout<<"Memory allocation failed for S\n";
//		#endif
//    }*/
//}


//void Profile::free_allocate()
//{ 
//	//if(!P.empty())
//	//{	
// //   	for(int i=0;i<no_pts;i++)       //deleting P
// //           delete [] P[i];
// //   	delete [] P;
//	//}
//
//	//if(!unitnormal.empty())
//	//{
//	//	for(int i=0;i<MAX+1;i++)
//	//	{       //deleting unitnormal
//	//		for(int j=0;j<no_pts;j++)
//	//			delete [] unitnormal[i][j];
// //                  	delete [] unitnormal[i];
//	//	}
//	//	delete [] unitnormal;
//	//}
//	//
//	//if(!S.empty())
//	//{
// //      	for(int i=0;i<(MAX+1);i++)
//	//	{         //deleting S
//	//		for(int j=0;j<no_pts;j++)
//	//			delete [] S[i][j];
// //                  	delete [] S[i];
// //        }
//	//     delete [] S;
//	//}
//
//	//*P = NULL;
//	//S = NULL;
//	//unitnormal = NULL;
//	//no_pts = 0;*/
//}



