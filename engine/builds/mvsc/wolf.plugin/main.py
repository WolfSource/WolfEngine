#import os
import sys

#_running_script_dir = os.path.realpath(__file__)

#sys.path
#sys.path.append(os.path.dirname(_running_script_dir) + '/../../../../bin/x64/Debug/Win32/')#add bin path for loading DLL

import PyWolfContentPipeline

_hr = PyWolfContentPipeline.initialize()
if not _hr:
    print "PyWolfContentPipeline could not initialize"

print PyWolfContentPipeline.collada_to_wolf_scene("C:\\Users\\PooyaEimandar\\Documents\\github\\WolfSource\\Wolf.Engine\\content\\models\\inst_max_oc.DAE", "[]")

print "Done"