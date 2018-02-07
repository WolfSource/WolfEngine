import sys, os

#get path of script
_script_path = os.path.realpath(__file__)
pyWolfPath = os.path.dirname(_script_path)

if sys.platform == "linux" or sys.platform == "linux2":
    print "Linux"
elif sys.platform == "darwin":
    print "OS X"
elif sys.platform == "win32":
    pyWolfPath =  pyWolfPath + "\\..\\..\\..\\..\\bin\\x64\\Debug\\Win32\\"

if pyWolfPath != "" and (not pyWolfPath in sys.path):
    sys.path.append(pyWolfPath)

import ctypes, threading, pyWolf
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
        _len = 512 * 512
        _size = 4 * _len

        _texture_data = _size * [None]
        index = 0
        for i in xrange(_len):
            _texture_data[index] = 255
            _texture_data[index + 1] = 0
            _texture_data[index + 2] = 0
            _texture_data[index + 3] = 255
            index = index + 4

        _t = pyWolf.graphics.w_texture.save_bmp_to_file("D:\github\WolfSource\Wolf.Engine\Logo00.bmp", 512, 512, _texture_data, 4 )
        print type(_t)
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
        while True:
            if self.__exiting:
                self._game.exit()
                break
            self._game.run(_map_info)

        print "Exited"

    def showEvent(self, event):
        #run in another thread
        threading.Thread(target=self.run).start()
        event.accept()

    def closeEvent(self, event):
        self.__exiting = True
        event.accept()

if __name__ == '__main__':

    # Create a Qt application
    app = QApplication(sys.argv)
    scene = scene("E:\\SourceCode\\github\\WolfSource\\Wolf.Engine\\content\\",
                   "E:\\SourceCode\\github\\WolfSource\\Wolf.Engine\\bin\\x64\\Debug\\Win32\\",
                   "py_01_clear")
    scene.resize(800, 600)
    scene.setWindowTitle('pyWolf')
    scene.show()

    sys.exit(app.exec_())

