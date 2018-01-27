import os

#append search path for pyWolf in windows
pyWolfPath = "/Users/pooyaeimandar/Documents/github/WolfSource/Wolf.Engine/bin/Build/macOS/Products/Debug/"

if not 'LD_LIBRARY_PATH' in os.environ:
  os.environ['LD_LIBRARY_PATH'] = ":" + pyWolfPath
elif not pyWolfPath in os.environ.get('LD_LIBRARY_PATH'):
  os.environ['LD_LIBRARY_PATH'] += ":" + pyWolfPath

