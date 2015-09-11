#ifndef NC_PROFILE
#define NC_PROFILE


#include "NcGeomKernel\NcGeomKernelGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"
//#include "NcUtility\NcVector.h"



#include <vector>
#include <QList>
#include <QtOpenGL>

namespace DiscreteSimulator
{
	
	class NCGEOMKERNELEXPORT Profile
	{
	public:
		Profile();
	     
		int				no_pts;
		CODE_Type		type;
		CODE_Type		typeTool;
		bool			coolant1;
		bool			coolant2;
		SpindleStatus	spindle;
		double			feed;
		
		STATUS			normalvector();	
		/*double			**P;
		double			***S;
		double			***unitnormal;*/
		std::vector <NcVector> P;
		std::vector < std::vector<NcVector > > S;
		std::vector < std::vector<NcVector> > unitnormal;
		QList<GLuint>	*mAssocitedDBDLIndexes;
		QList<GLuint>	*mAssociated2DDLIndexes;
		int				mNoOfDBDL;
		/*void allocate();*/

	private:
		
		
		
	};
}

#endif