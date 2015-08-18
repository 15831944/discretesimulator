#ifndef NC_PROFILE
#define NC_PROFILE


#include "NcGeomKernel\NcGeomKernelGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"

#include <QList>
#include <QtOpenGL>
namespace DiscreteSimulator
{
	
	class NCGEOMKERNELEXPORT Profile
	{
	public:
		Profile();
		~Profile();
		void allocate();
		void free_allocate();
	     
		int				no_pts;
		CODE_Type		type;
		CODE_Type		typeTool;
		bool			coolant1;
		bool			coolant2;
		SpindleStatus	spindle;
		double			feed;
		double			**P;
		double			***S;
		double			***unitnormal;

		QList<GLuint>	*mAssocitedDBDLIndexes;
		QList<GLuint>	*mAssociated2DDLIndexes;
		int				mNoOfDBDL;
	};
}

#endif