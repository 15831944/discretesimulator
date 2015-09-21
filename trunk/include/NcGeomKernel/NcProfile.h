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
		std::vector <NcVector> P;
		std::vector < std::vector<NcVector > > S;
		std::vector < std::vector<NcVector> > unitnormal;
		CODE_Type		type;
		CODE_Type		typeTool;
		int				no_pts;
		SpindleStatus	spindle;
		double			feed;
		STATUS			normalvector();
		int				mNoOfDBDL;
		void addProfileDisplayListIndex(int newlistindex);
		void addProfileDBDLIndex(int newlistindex);
		void setProfile(std::vector<NcVector> profile);
		GLuint getAssocitedDBDLIndexes(int number);
		GLuint getAssociated2DDLIndexes(int number);
		int getAssocitedDBDLsize();
		int getAssociated2DDLsize();
	
	private:
		
		bool			coolant1;
		bool			coolant2;
		QList<GLuint>	*mAssociatedDBDLIndexes;
		QList<GLuint>	*mAssociated2DDLIndexes;
		
	};
}

#endif