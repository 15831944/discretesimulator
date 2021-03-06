#ifndef NC_DISPLAY
#define NC_DISPLAY

#include "NcDisplay\NcDisplayGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"
#include "NcGeomKernel\NcPolygonBoolean.h"
#include <QtOpenGl>

namespace DiscreteSimulator
{//sda

	typedef enum _CUT{	NO_VERTICAL_CUT,
								ALLOW_VERTICAL_CUT
							 }CUT;
 //*****//
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
		void			updateStockBoundingBox();	
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
		STATUS			createSurfaceOfRotation();
		STATUS			createDeformedBody(Profile* target, std::vector<vector2d> tool, CUT cut = NO_VERTICAL_CUT);
		STATUS			createDeformedBody(Profile* target);
		STATUS			compressArray(double target[][2], int &n);
		
		STATUS			load_Cutting_Tool(std::vector<vector2d> tool, double *P, int i, int j);
		STATUS			load_Drilling_Tool(std::vector<vector2d> tool, const NcVector& P1, const NcVector&  P2);
		STATUS			load_Parting_Tool(std::vector<vector2d> tool, int i, int j);
		STATUS			load_CG00_Tool(std::vector<vector2d> tool);
		STATUS			load_Facing_Tool(std::vector<vector2d> tool, int i, int j);

		int				spinDisplay(bool gstepmode, int goNextOperation, int& deformed_ds_count, int& DISPLAY_count);
		void			material_stock();
		void			surfaceDisplay();
		void			addDLIndexesToList(NcCode *code);
		void			addProfilesToList(NcCode *code);

  public:
		/****Pranit***/
		bool			setUserDefinedStockValues(double stockRadius,double stockLength );
		bool			getUserDefinedStockFlag(){return mUserDefinedStock; }
		void			setUserDefinedStockFlag(bool status ){ mUserDefinedStock = status; } // this function is added to check if user has defined stock dimentions
		
	private:
		
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
		GLuint					mStartupDLForStock;

	protected:
		NcDisplay();									
		NcDisplay(const NcDisplay &){}									
		NcDisplay&	operator=(const NcDisplay&);

		/****Pranit***/
	public:
			bool			mUserDefinedStock;
			
	};
	
	
	
}

#endif
