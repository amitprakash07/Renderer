#ifndef __RENDERING_H
#define __RENDERING_H

#include <windows.h>
#include "objects.h"

void BeginRender();
inline void StopRender(){}

struct __threadHandles
{
	HANDLE *thread;
	int threadCount;

	__threadHandles(int i_threadCount)
	{
		threadCount = i_threadCount;
		thread = new HANDLE[i_threadCount];
		//threadID = new unsigned long[threadCount];
	}

	__threadHandles()
	{
		threadCount = 0;
		thread = nullptr;
		//threadVal = nullptr;
	}

	void destroyThread()
	{
		for (int i = 0; i < threadCount; ++i)
		{
			CloseHandle(thread[i]);
			thread[i] = nullptr;
		}
		//delete threadID;
	}
};
typedef __threadHandles ThreadHandle;


struct __pixel
{
	int offsetAlongHeight;
	int offsetAlongWidth;
};

typedef __pixel Pixel;

class Renderer
{
public:
	Renderer();
	static void startRendering(size_t);
private:
	static DWORD WINAPI renderPixel(LPVOID threadData);
	static void calculatePixelColor(Node&, LightList&, int, int);
	static size_t threadCount;
	static ThreadHandle mThreadHandle;
	//static Pixel * mPixelOffset;
	static Color24* renderingImage;
	static float* zBufferImage;
	static uchar* sampleCountImage;
	static Color24* operationCountImage;
	static size_t noOfRowsToRenderPerThread;
	static int imageWidth;
	static int imageHeight;
};



#endif