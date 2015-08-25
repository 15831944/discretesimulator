#ifndef NC_SIMULATIONCONTROLLER
#define NC_SIMULATIONCONTROLLER

#include "NcSimulationController\NcSimulationControllerGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"
#include <QtOpenGl>
#include <QObject>

namespace DiscreteSimulator
{
	class NcStlImport;
	class NcCode;
	
	enum SimulationMode {BYBLOCK, BYCYCLE};

	class NCSIMULATIONCONTROLLEREXPORT NcSimulationController : public QObject
	{
		Q_OBJECT

	public:
		~NcSimulationController();
		static			NcSimulationController* getSimControllerInstance();
		void			setSimulationMode(SimulationMode mode);
		SimulationMode	getSimulationMode();
		bool			runSimulation();

		// !!!!  Dead code !! Functions not defined anywhere for this class  !!!! //

		//void			callToolPathDL();
		//void			callDeformedBodyDL(GLuint id);s
		
		inline void		setSimulationState(SimulationState simulationstate);
		void			setOpIndexForNEXTState();

		void			rewindSimulation();
		
		friend class	cleanupSimulationController;
	
	signals:
		void			codeEditorRequiresUpdate(int);
		void			enableRewindButton();

	protected:
		NcSimulationController();									
		NcSimulationController(const NcSimulationController &){}									
		NcSimulationController&	operator=(const NcSimulationController&);

	private:
		bool			runSimulationByBlock();
		bool			runSimulationByCycle();

	private:
		SimulationMode					mNcSimulationMode;
		static NcSimulationController	*mSimControllerInstance;

		SimulationState					mSimulationState;

		int								indexOfLastGCodeLine;

		//////variables for controlling simulation
		static	int		executingCodeLineNumber;
		static	int		executingBlockOfCurrLine;

		NcCode			*currentcode;
		NcCode			*mLastMachiningCode;
	};

}


#endif