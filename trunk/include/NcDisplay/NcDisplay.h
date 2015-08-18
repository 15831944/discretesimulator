#ifndef NC_DISPLAY
#define NC_DISPLAY

#include "NcDisplay\NcDisplayGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"
#include <QtOpenGl>

namespace DiscreteSimulator
{
	class NcCode;
	class Profile;

	inline double maximum(double a, double b) 
	{
		return (a >= b) ? a : b;
	}

	inline  double minimum(double a, double b)
	{
		return (a <= b) ? a : b;
	}

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
		void			updateStockBoundingBox();
		void			setStockBBInitialValues();
		STATUS			displayStockProfile();
		STATUS			createPart();
		STATUS			createSurfaceOfRotation(int t);
		void			surfaceDisplay();
		bool			setStockBBFinalValues();
		void			generateDisplayLists();
		STATUS			createDeformedBody(Profile* target, double **tool, CUT cut = NO_VERTICAL_CUT);
		void			displayProfile(Profile *target);
		STATUS			compressArray(double target[][2], int &n);
		void			callToolPathDL();
		void			callDeformedBodyDL(GLuint id);
		STATUS			load_Cutting_Tool(double **tool, double *P, int i, int j);
		STATUS			load_Drilling_Tool(double **tool, double *P1, double *P2, double *P);
		STATUS			load_Parting_Tool(double **tool, double *P, int i, int j);
		STATUS			load_CG00_Tool(double **tool, double *P, int i, int j);
		STATUS			load_Facing_Tool(double **tool, double *P, int i, int j);
		
		void			material_stock();
		
		int				spinDisplay(bool gstepmode, int goNextOperation, int& deformed_ds_count, int& DISPLAY_count);
		GLuint			getStockDisplayListIndex();
		STATUS			normalvector(int t);
		
		STATUS			createDeformedBody(Profile* target);
		void			setIndex(int index);

		Profile*		getStockProfile();		
		void			generateStartupDLForStock();

	private:
		
		void			addDLIndexesToList(NcCode *code);
		void			addProfilesToList(NcCode *code);

	private:
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