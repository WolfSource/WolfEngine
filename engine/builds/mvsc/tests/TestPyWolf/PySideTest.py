import os
import sys
import thread
import PyWolf

#PySide
from PySide import QtCore, QtGui
from PySide.QtGui import *
from PySide.QtCore import *

WIDTH = 640
HEIGHT = 480

#PySide code
app = QtGui.QApplication(sys.argv)
if not app:
    app = QtGui.QApplication([])

class WolfWidget(QtGui.QWidget):
    def __init__(self, parent = None):
        QtGui.QWidget.__init__(self, parent)

    def paintEvent(self, event):
        #p = QPainter(self)
        #po = QPoint(0,0);

        #img = QImage(WIDTH, HEIGHT, QImage.Format_RGB888)       
        #for i in range(0, WIDTH):
        #    for j in range(0, HEIGHT):
        #        img.setPixel(i, j, qRgb(255, 0, 0))

        #p.drawImage(po, img)
        PyWolf.render()
        self.update()

w = WolfWidget()

def main_wolf():
    # get HWND for windows
    _wid = w.winId()

    #get the current path
    _current_path = os.path.dirname(os.path.abspath(__file__))
    _hr = PyWolf.initialize(_wid, "PyWolf", _current_path, "content")
    if _hr == 1 :
        print "Error on initializing PyWolf"
        return

#create instance of Wolf widget viewer
if __name__ == '__main__':
    w.resize(WIDTH, HEIGHT)
    w.setWindowTitle("PyWolf")
    w.show()

    #run wolf
    try:
        thread.start_new_thread( main_wolf, () )
    except:
        print "Error: unable to start thread for PyWolf"

    app.exec_()

    PyWolf.release()
    print "\r\nPyWolf successfully released"