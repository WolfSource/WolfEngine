import sys, ctypes, threading, pyWolf
from PySide import QtGui, QtCore
from PySide.QtGui import *
from PySide.QtCore import *

screen_width = 800
screen_height = 600

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
        self._gDevice = None
        self._viewport = pyWolf.graphics.w_viewport()
        self._viewport_scissor = pyWolf.graphics.w_viewport_scissor()
        self._draw_command_buffers = pyWolf.graphics.w_command_buffer()
        self._draw_render_pass = pyWolf.graphics.w_render_pass()
        self._draw_fence = pyWolf.graphics.w_fences()
        self._draw_semaphore = pyWolf.graphics.w_semaphore()
        

    def pre_init(self):
        print "pre_init"

    def post_init(self):
        #get main graphics device
        self._gDevice = self._game.get_graphics_device(0)
        print self._gDevice.get_info()
        print "post_init"

    def load(self):
        #initialize viewport
        self._viewport.y = 0;
        self._viewport.width = screen_width;
        self._viewport.height = screen_height;
        self._viewport.minDepth = 0;
        self._viewport.maxDepth = 1;
        
        #initialize scissor of viewport
        self._viewport_scissor.offset.x = 0;
        self._viewport_scissor.offset.y = 0;
        self._viewport_scissor.extent.width = screen_width;
        self._viewport_scissor.extent.height = screen_height;

        #load render pass which contains frame buffers
        _hr = self._draw_render_pass.load(self._gDevice, self._viewport, self._viewport_scissor)
        if _hr == False:
            print "Error on loading render pass"

        #create one semaphore for drawing
        _hr = self._draw_semaphore.initialize(self._gDevice)
        if _hr == False:
            print "Error on initializing semaphore"

        ##create one fence for drawing
        #_hr = self.__draw_fence.initialize(0, 1)
        #if _hr == False:
        #    print "Error on initializing fence(s)"

        print "scene loaded"

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
        
        self.release()
        print "Exited"

    def showEvent(self, event):
        #run in another thread
        threading.Thread(target=self.run).start()
        event.accept()

    def closeEvent(self, event):
        self.__exiting = True
        event.accept()

    def release(self):
        self._draw_render_pass.release()
        self._draw_render_pass = None
        self._draw_semaphore = None
        self._game = None
        self._gDevice = None
        self._viewport = None
        self._viewport_scissor = None
        self._draw_command_buffers = None
        self._draw_fence = None
        
if __name__ == '__main__':

    # Create a Qt application
    app = QApplication(sys.argv)
    scene = scene("E:\\SourceCode\\github\\WolfSource\\Wolf.Engine\\content\\",
                   "E:\\SourceCode\\github\\WolfSource\\Wolf.Engine\\bin\\x64\\Debug\\Win32\\",
                   "py_01_clear")
    scene.resize(screen_width, screen_height)
    scene.setWindowTitle('Wolf.Engine')
    scene.show()

    sys.exit(app.exec_())

