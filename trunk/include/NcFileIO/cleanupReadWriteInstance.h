#ifndef NC_READ_WRITE_INSTANCE
#define NC_READ_WRITE_INSTANCE
#include "NcFileIO\NcFileIOGlobal.h"

namespace DiscreteSimulator
{
	class NCEXPORT cleanupReadWriteInstance
	{
	public:
		~cleanupReadWriteInstance();
	};
}

#endif