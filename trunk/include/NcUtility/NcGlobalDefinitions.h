#ifndef NCGLOBALDEFINITIONS
#define NCGLOBALDEFINITIONS

#include "NcUtility\NcVector.h"

namespace DiscreteSimulator
{
	typedef enum  _CODE_Type {	UNKNOWN=10,
										CG00,
										CG01,
										CG02,
										CG03,
										CG04,
										CG06,
										CG07,
										CG08,
										CG20,
										CG21,
										CG22,
										CG28,
										CG29,
										CG30,
										CG32,
										CG74,
										CG75,
										CG76,										
										CG90,
										CG92,
										CG94,
										CG96,
										CG97,
										CGAUX,
										CM01,
										CM02,
										CM03,
										CM04,
										CM05,
										CM07,
										CM08,
										CM09,
										PRGSTOP,
										CM98,
										CM99,
										CT00,							//created for CG00
										CT01,							//spoint tool
										CT02,							//drilling tool
										CT03,							//threading tool
										CT04,
										CT05,
										CT06,
										CT07,
										CT08,
										UNKNOWNTOOL
									}CODE_Type;

			typedef enum  _SpindleStatus{	SM03=200,
											SM04,
											SM05,
										}SpindleStatus;

			typedef  enum _STATUS{ FAIL,
								   OK,
								 }STATUS;

			typedef enum _CUT{	NO_VERTICAL_CUT,
								ALLOW_VERTICAL_CUT
							 }CUT;

			typedef enum  _INT_STATUS{	I_FAIL,
								I_OK,
								BOTH_ON_TOOL,
								BOTH_ON_TARGET

							}INT_STATUS;

					enum	{	INSIDE = 1,
								OUTSIDE,
								BOUNDARY,
								ADD,
								DELET
							};

			const int	MAX_STL_TRI 	=	15000;

			typedef struct _triangle
			{
					int id;
					NcVector normal;
					NcVector v[3];
			}triangle;

			typedef struct _triangulation
			{
					int num_triangle;
					triangle t[MAX_STL_TRI];
			}triangulation;

                                                           


		enum SimulationState {RUNNING, PAUSED, NEXT, END};	//set simulation mode as running/paused for sending proper display list index
														//to NcSimulationWindow(paintGL func) 

			const double	TOL					=	0.000001; 
			const int		MAX_LINEAR_SUBDIV  	=	11;
			const int		ARRAY_MAX			=	10000;
			const double	PARA_TOL			=	0.000001;
			const double	M_PI				=	3.14159265358979323846;
			const int		MAX					=	360;
			const int		MAX_ANG_SUBDIV		=	15;
			const int		MAX_ID				=	100;
			
}


#endif