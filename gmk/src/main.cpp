/*
 *	main.cpp
 *	GMK API Library Test
 *	(AKA Preprocessor madness)
 */

#ifdef STANDALONE

//#define USE_THREADS
//#define PROFILE
#define DEFRAGMENT

#if defined USE_THREADS || defined _DEBUG || defined PROFILE
#include <Windows.h>
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <iostream>
#include <gmk.hpp>

#define FILENAME_IN				"Vivant.gm81"
#define FILENAME_OUT			"out.gm81"

#ifdef USE_THREADS
DWORD WINAPI SaveThread(LPVOID lpParam)
{
	Gmk::GmkFile* gmk = reinterpret_cast<Gmk::GmkFile*>(lpParam);
	gmk->Save(FILENAME_OUT);

	return 0;
}

DWORD WINAPI LoadThread(LPVOID lpParam)
{
	Gmk::GmkFile* gmk = reinterpret_cast<Gmk::GmkFile*>(lpParam);
	gmk->Load(FILENAME_IN);

	return 0;
}
#endif

int main(int argc, char* argv[])
{
#ifdef PROFILE
	LARGE_INTEGER frequency, t1, t2;

	QueryPerformanceFrequency(&frequency);
#endif

	std::cout << "GMK API Library Test" << std::endl;

	Gmk::GmkFile* gmk = new Gmk::GmkFile();

#ifdef USE_THREADS
	DWORD loadThreadId;
	HANDLE loadThread = CreateThread(NULL, 0, LoadThread, gmk, 0, &loadThreadId);

	while(WaitForSingleObject(loadThread, 1) != WAIT_OBJECT_0)
		std::cout << "Loading... " << std::floor(gmk->GetProgress()) << "%      \r";

	CloseHandle(loadThread);
#else
	std::cout << "Loading... ";
#ifdef PROFILE
	QueryPerformanceCounter(&t1);
#endif
	gmk->Load(FILENAME_IN);
#ifdef PROFILE
	QueryPerformanceCounter(&t2);
	std::cout << "Done!" << std::endl;
	std::cout << "Elapsed " << (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart << "ms" << std::endl;
#else
	std::cout << "Done!" << std::endl;
#endif
#endif

#ifdef DEFRAGMENT
	std::cout << "Defragmenting... ";
#ifdef PROFILE
	QueryPerformanceCounter(&t1);
#endif
	gmk->DefragmentResources();
#ifdef PROFILE
	QueryPerformanceCounter(&t2);
	std::cout << "Done!" << std::endl;
	std::cout << "Elapsed " << (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart << "ms" << std::endl;
#else
	std::cout << "Done!" << std::endl;
#endif
#endif
	
#ifdef USE_THREADS
	DWORD saveThreadId;
	HANDLE saveThread = CreateThread(NULL, 0, SaveThread, gmk, 0, &saveThreadId);

	while(WaitForSingleObject(saveThread, 1) != WAIT_OBJECT_0)
		std::cout << "Saving... " << std::floor(gmk->GetProgress()) << "%      \r";

	CloseHandle(saveThread);
#else
	std::cout << "Saving... ";
#ifdef PROFILE
	QueryPerformanceCounter(&t1);
#endif
	gmk->Save(FILENAME_OUT);
#ifdef PROFILE
	QueryPerformanceCounter(&t2);
	std::cout << "Done!" << std::endl;
	std::cout << "Elapsed " << (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart << "ms" << std::endl;
#else
	std::cout << "Done!" << std::endl;
#endif
#endif

	delete gmk;

#if defined _DEBUG && defined PROFILE
	if (_CrtDumpMemoryLeaks() == 0)
		std::cout << "No leaks!" << std::endl;
	else
		std::cout << "Leaks detected!" << std::endl;

	std::getchar();
#endif

 	return 0;
}

#endif
