#include "pch.h"
#include "WLoadSceneCmd.h"

MStatus WLoadSceneCmd::doIt(const MArgList&)
{
#ifdef _DEBUG
	Log("LoadScene command called");
#endif
	
	Wolf::Commands = Wolf::WCommnads::LoadScene;

	return MS::kSuccess;
}

void* WLoadSceneCmd::Command()
{ 
	return new WLoadSceneCmd();
}