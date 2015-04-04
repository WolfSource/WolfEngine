#pragma once

#include <maya/MArgList.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>

class WLoadSceneCmd : public MPxCommand
{
public:
	WLoadSceneCmd() {};

	//virtual methods of MPxCommand
	virtual MStatus doIt(const MArgList&);

	//The Command caller
	static void* Command();
};
