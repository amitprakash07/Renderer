#ifndef __MAIN_H
#define __MAIN_H

#pragma optimize("/Og", off)
#include "objects.h"


//#define LOAD_FILE "Texture.xml"
//#define LOAD_FILE "XMLFile.xml"
#define LOAD_FILE  "DepthOfField.xml"
//#define LOAD_FILE "SoftShadowsGlossySurface.xml"
#define MIN_SAMPLE_COUNT 16
#define MAX_SAMPLE_COUNT 256
#define MIN_VARIANCE 0.0001
#define MAX_VARIANCE 0.001

#ifdef WIN32
#define COMPARE(a,b) (_stricmp(a,b)==0)
#else
#define COMPARE(a,b) (strcasecmp(a,b)==0)
#endif





#endif