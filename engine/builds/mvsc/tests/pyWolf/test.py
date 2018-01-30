#append search path for pyWolf in windows
#pyWolfPath = "/Users/pooyaeimandar/Documents/github/WolfSource/Wolf.Engine/bin/Build/macOS/Products/Debug/"
#pyWolfPath = "D:\\github\\WolfSource\\Wolf.Engine\\bin\\x64\\Debug\\Win32\\"
#pyWolfPath = "E:\\SourceCode\\github\\WolfSource\\Wolf.Engine\\bin\\x64\\Debug\\Win32"

#if pyWolfPath != "" and (not pyWolfPath in sys.path):
    #sys.path.append(pyWolfPath)

import sys, ctypes, threading, pyWolf 
from PySide import QtGui, QtCore
from PySide.QtGui import *
from PySide.QtCore import *

class scene(QWidget):
    def __init__(self, ContentPath, LogPath, AppName, parent = None):
        super(scene, self).__init__(parent)
        self.__exiting = False
        self._game = pyWolf.framework.w_game(ContentPath, LogPath, AppName)
        self._game.set_pre_init_callback(self.pre_init)
        self._game.set_post_init_callback(self.post_init)
        self._game.set_load_callback(self.load)
        self._game.set_update_callback(self.update)
        self._game.set_pre_render_callback(self.pre_render)
        self._game.set_post_render_callback(self.post_render)
        
    def pre_init(self):
        print "pre_init"

    def post_init(self):
        print "post_init"

    def load(self):
        print "load"

    def update(self, GameTime):
        print GameTime.get_total_seconds()
    
    def pre_render(self, GameTime):
        print "pre_render"
    
    def post_render(self, SuccessfullyRendered):
        print "post_render"
    
    def run(self):
        #run game
        _window_info = pyWolf.system.w_window_info()
        _window_info.width = self.width()
        _window_info.height = self.height()
        _window_info.v_sync_enable = False
        _window_info.is_full_screen = False
        _window_info.swap_chain_format = 44 # BGRA8Unorm in VULKAN 
        _window_info.cpu_access_swap_chain_buffer = False

        # get window handle
        pycobject_hwnd = self.winId()
        #convert window handle as HWND to unsigned integer pointer for c++
        ctypes.pythonapi.PyCObject_AsVoidPtr.restype  = ctypes.c_void_p
        ctypes.pythonapi.PyCObject_AsVoidPtr.argtypes = [ctypes.py_object]
        int_hwnd = ctypes.pythonapi.PyCObject_AsVoidPtr(pycobject_hwnd)
        _window_info.set_win_id(int_hwnd)
  
        #initialize game
        _map_info = (0, _window_info)
        while not  self.__exiting:
            self._game.run(_map_info)

    def showEvent(self, event):
        #run in another thread
        threading.Thread(target=self.run).start()
        event.accept()

    def closeEvent(self, event):
        self.__exiting = True
        self._game.exit()
        event.accept()

if __name__ == '__main__':

    # Create a Qt application
    app = QApplication(sys.argv)
    scene = scene("E:\\SourceCode\\github\\WolfSource\\Wolf.Engine\\content\\",
                   "E:\\SourceCode\\github\\WolfSource\\Wolf.Engine\\bin\\x64\\Debug\\Win32",
                   "pyWolf.test")
    scene.resize(800, 600)
    scene.setWindowTitle('Wolf.Engine')
    scene.show()

    sys.exit(app.exec_())

