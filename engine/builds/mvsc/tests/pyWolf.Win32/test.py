import sys, ctypes

#append search path for pyWolf in windows
pyWolfPath = "/Users/pooyaeimandar/Documents/github/WolfSource/Wolf.Engine/bin/Build/macOS/Products/Debug/"
if pyWolfPath != "" and (not pyWolfPath in sys.path):
    sys.path.append(pyWolfPath)

import pyWolf
from pyWolf import *

if __name__ == '__main__':
    bbox_0 = pyWolf.w_bounding_box()
    bbox_1 = pyWolf.w_bounding_box()

    bbox_0.max[0] = 10
    bbox_0.max[1] = 10
    bbox_0.max[2] = 10

    y = bbox_0.intersects(bbox_1)
    bbox_0.generate_vertices_indices()
    bbox_0.merge(bbox_1)

    v0 = (0.0, 0.0, 0.0)
    c = bbox_0.contains_point(v0)
    print type(c)

    print "pyWolf"
