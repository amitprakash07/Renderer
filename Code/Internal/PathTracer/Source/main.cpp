#include "xmlload.h"
#include "viewport.h"
#include "../Threading/Mutex.h"
#include <tchar.h>

//#define LOAD_FILE "CornellBox.xml"
//#define LOAD_FILE "Box.xml"
//#define LOAD_FILE "DepthOfField.xml"
//#define LOAD_FILE "SoftShadowsGlossySurface.xml"
//#define LOAD_FILE "SoftShadowsGlossySurfacewithoutPlane.xml"
//#define LOAD_FILE "SoftShadowsGlossySurface_simple.xml"
//#define LOAD_FILE "Shading.xml"
//#define LOAD_FILE "white_teapot_GI.xml"
#define LOAD_FILE "CornellBoxForGI.xml"

int main(void)
{

	int temp = LoadScene(LOAD_FILE);
	TCHAR* mutexName = __T("WritingMutex");
	HANDLE mutexHandle = CreateMutex(nullptr, FALSE, mutexName);
	ShowViewport();
	return 0;
	
}
		












