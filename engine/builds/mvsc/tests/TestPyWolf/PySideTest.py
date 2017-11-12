import os
import sys
import thread
import ctypes
import PyWolf
import datetime

#PySide
from PySide import QtCore, QtGui
from PySide.QtGui import *
from PySide.QtCore import *

WIDTH = 640
HEIGHT = 480

pywolf_version = "PyWolf (Python wrapper for Wolf.Engine)"

#PySide code
app = QtGui.QApplication(sys.argv)
if not app:
    app = QtGui.QApplication([])

class Logger(QPlainTextEdit):
    def __init__(self, parent = None):
        super(Logger, self).__init__(parent)
        self.setReadOnly(1)
  
    def log(self, pMsg):
        self.appendPlainText(pMsg)

w = QWidget()
logger = Logger()
cmd = Logger()

def run_wolf():
    # get window handle
    pycobject_hwnd = w.winId()
    #convert window handle as HWND to unsigned integer pointer for c++
    ctypes.pythonapi.PyCObject_AsVoidPtr.restype  = ctypes.c_void_p
    ctypes.pythonapi.PyCObject_AsVoidPtr.argtypes = [ctypes.py_object]
    int_hwnd = ctypes.pythonapi.PyCObject_AsVoidPtr(pycobject_hwnd)
    
    #get the current path
    _current_path = os.path.dirname(os.path.abspath(__file__))
    _hr = PyWolf.initialize(int_hwnd, "PyWolf", _current_path, "F:\\github\\WolfSource\\Wolf.Engine\\content\\")
    if _hr == 1 :
        print "Error on running PyWolf"
        return

def on_clear_log_clicked():
    logger.clear()
    logger.log(pywolf_version)

def on_execute_clicked():
    exec(cmd.toPlainText())

def init_wolf_utilities(pWidget):
    
    #initialize the widget
    pWidget.setWindowTitle("PyWolf Utilities")
    pWidget.resize(WIDTH / 2, HEIGHT / 2)
    
    #initialize the buttons
    clear_btn = QPushButton("Clear Log")
    clear_btn.clicked.connect(on_clear_log_clicked)

    execute_btn = QPushButton("Execute")
    execute_btn.clicked.connect(on_execute_clicked)

    #clear log
    on_clear_log_clicked()

    #input command and logger
    cmd.setReadOnly(0)
    v = QVBoxLayout(pWidget)
    v.addWidget(logger)
    v.addWidget(cmd)
    v.addWidget(clear_btn)
    v.addWidget(execute_btn)

#create instance of Wolf widget viewer
if __name__ == '__main__':
    w.resize(WIDTH, HEIGHT)
    w.setWindowTitle("PyWolf Viewer")
    w.show()
    
    d = QWidget()
    init_wolf_utilities(d)
    d.show()
     
    #run wolf
    try:
        thread.start_new_thread( run_wolf, () )
    except:
        print "Error: unable to start thread for PyWolf"

    app.exec_()

    PyWolf.release()
    print "\r\nPyWolf shut down successfully"
