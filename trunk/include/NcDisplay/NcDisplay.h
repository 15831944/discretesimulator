#ifndef NC_DISPLAY
#define NC_DISPLAY

#include "NcDisplay\NcDisplayGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"
#include <QtOpenGl>

namespace DiscreteSimulator
{
	class NcCode;
	class Profile;



	class NCDISPLAYEXPORT box
	{
	public:
		double xmin, zmin;
		double xmax, zmax;
	};


	class NCDISPLAYEXPORT NcDisplay
	{
	public:
		~NcDisplay();
		static			NcDisplay* getNcDisplayInstance();
		void			CreateDisplayListsForGCodes();		
		void			setStockBBInitialValues();

		STATUS			displayStockProfile();	

		bool			setStockBBFinalValues();
		void			generateDisplayLists();	
		void			callToolPathDL();
		void			callDeformedBodyDL(GLuint id);	

		GLuint			getStockDisplayListIndex();	

		void			setIndex(int index);

		Profile*		getStockProfile();		
		void			generateStartupDLForStock();

	private:
		STATUS			createPart();
		STATUS			createSurfaceOfRotation(int t);
		STATUS			createDeformedBody(Profile* target, double **tool, CUT cut = NO_VERTICAL_CUT);
		STATUS			createDeformedBody(Profile* target);
		STATUS			compressArray(double target[][2], int &n);
		
		STATUS			load_Cutting_Tool(double **tool, double *P, int i, int j);
		STATUS			load_Drilling_Tool(double **tool, double *P1, double *P2, double *P);
		STATUS			load_Parting_Tool(double **tool, double *P, int i, int j);
		STATUS			load_CG00_Tool(double **tool, double *P, int i, int j);
		STATUS			load_Facing_Tool(double **tool, double *P, int i, int j);

		STATUS			normalvector(int t);	

		int				spinDisplay(bool gstepmode, int goNextOperation, int& deformed_ds_count, int& DISPLAY_count);
		void			material_stock();
		void			surfaceDisplay();
		void			addDLIndexesToList(NcCode *code);
		void			addProfilesToList(NcCode *code);

	private:
		void			updateStockBoundingBox();
		void			displayProfile(Profile *target);
		static NcDisplay		*mNcDisplayInstance;
		static int				dllistcount;
		box						mStockBoundingBox;
		Profile					*stock;
		QList<Profile *>		mPartProfileList;
		QList<GLuint>			mDisplayListIndexes;
		QList<GLuint>			mDeformedBodyDLIndexes;

		int						mTotalPartDisplayLists;
		GLuint					mPartDisplayListIndexes;
		SimulationState			mSimulationState;
		int						mLastMachiningOpIndex;


	protected:
		NcDisplay();									
		NcDisplay(const NcDisplay &){}									
		NcDisplay&	operator=(const NcDisplay&);

	};
}

#endif
