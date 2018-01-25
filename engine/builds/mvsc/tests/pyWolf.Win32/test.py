import pyWolf
from pyWolf import *

if __name__ == '__main__':
    bbox_0 = pyWolf.w_bounding_box()
    bbox_1 = pyWolf.w_bounding_box()

    y = bbox_0.intersects(bbox_1)
    bbox_0.generate_vertices_indices()
    bbox_0.merge(bbox_1)

    v0 = (0.0, 0.0, 0.0)
    c = bbox_0.contains_point(v0)
    print type(c)

    print "pyWolf"
