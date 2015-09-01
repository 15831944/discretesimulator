
#include <QtOpenGl>
#include "NcUtility\NcGlobalDefinitions.h"

namespace DiscreteSimulator
{
class NcToolImporter

{
public:
	
	GLuint				getDLIdForTurningTool() {return tool_list;}
	GLuint				getDLIdForDrillingTool() {return drill_tool_list;}
	GLuint				getDLIdForPartingTool() {return parting_tool_list;}
	GLuint				getDLIdForThreadingTool() {return threading_tool_list;}
	   int			    importLatheTools();
	STATUS				stl_display_triangulation (triangulation *patch);

private:
		GLuint			n_list;
		GLuint			n_list_count;
		GLuint			tool_list;
		GLuint			drill_tool_list;
		GLuint			parting_tool_list;
		GLuint			threading_tool_list;
		/*GLuint			lathe_list;
		GLuint			chuck_list;*/
};

}