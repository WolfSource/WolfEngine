import sys, os

#get path of script
_script_path = os.path.realpath(__file__)
pyWolfPath = os.path.dirname(_script_path)

if sys.platform == "linux" or sys.platform == "linux2":
    print "Linux not tested yet"
elif sys.platform == "darwin":
    print "OS X not tested yet"
elif sys.platform == "win32":
    pyWolfPath =  pyWolfPath + "\\..\\..\\..\\..\\bin\\x64\\Debug\\Win32\\"

if pyWolfPath != "" and (not pyWolfPath in sys.path):
    sys.path.append(pyWolfPath)

import ctypes, threading, pyWolf
from PySide import QtGui, QtCore
from PySide.QtGui import *
from PySide.QtCore import *

screen_width = 800
screen_height = 600

#++++++++++++++++++++++++++++++++++++++++++++++++++++
#The following codes have been added for this project
#++++++++++++++++++++++++++++++++++++++++++++++++++++
class gui(QWidget):
    def __init__(self, parent=None):
        super(gui, self).__init__(parent)

        self._debug_text = ""
        self._label = QLabel()
        self._label.setAlignment(Qt.AlignLeft)

        vbox = QVBoxLayout()
        vbox.addWidget(self._label)

        timer = QTimer(self)
        timer.timeout.connect(self.updateTime)
        timer.start(50)
        
        self.setLayout(vbox)

    @QtCore.Slot()
    def slot(self, str):
        self._debug_text = str
        
    def updateTime(self):
        self._label.setText(self._debug_text)
#++++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++++++++++++++++++

class scene(QWidget):
    #++++++++++++++++++++++++++++++++++++++++++++++++++++
    #The following codes have been added for this project
    #++++++++++++++++++++++++++++++++++++++++++++++++++++
    signal = Signal(str)
    #++++++++++++++++++++++++++++++++++++++++++++++++++++
    #++++++++++++++++++++++++++++++++++++++++++++++++++++
        
    def __init__(self, pContentPath, pLogPath, pAppName, parent = None):
        super(scene, self).__init__(parent)
        self.__exiting = False
        self._game = pyWolf.framework.w_game(pContentPath, pLogPath, pAppName)
        self._game.set_pre_init_callback(self.pre_init)
        self._game.set_post_init_callback(self.post_init)
        self._game.set_load_callback(self.load)
        self._game.set_update_callback(self.update)
        self._game.set_pre_render_callback(self.pre_render)
        self._game.set_post_render_callback(self.post_render)
        self._gDevice = None
        self._viewport = pyWolf.graphics.w_viewport()
        self._viewport_scissor = pyWolf.graphics.w_viewport_scissor()
        self._draw_command_buffers = pyWolf.graphics.w_command_buffers()
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
        self._viewport.y = 0
        self._viewport.width = screen_width
        self._viewport.height = screen_height
        self._viewport.minDepth = 0
        self._viewport.maxDepth = 1
        
        #initialize scissor of viewport
        self._viewport_scissor.offset.x = 0
        self._viewport_scissor.offset.y = 0
        self._viewport_scissor.extent.width = screen_width
        self._viewport_scissor.extent.height = screen_height

        #load render pass which contains frame buffers
        _render_pass_attachments = []
        _output_window = self._gDevice.output_presentation_window
        for _iter in _output_window.swap_chain_image_views:
            # COLOR                                 #DEPTH
            _render_pass_attachments.append([_iter, _output_window.depth_buffer_image_view])

        _hr = self._draw_render_pass.load(self._gDevice, self._viewport, self._viewport_scissor, _render_pass_attachments)
        if _hr:
            print "Error on loading render pass"
            return
        
        #create one semaphore for drawing
        _hr = self._draw_semaphore.initialize(self._gDevice)
        if _hr:
            print "Error on initializing semaphore"
            return

        #create one fence for drawing
        _hr = self._draw_fence.initialize(self._gDevice)
        if _hr:
            print "Error on initializing fence(s)"
            return

        #create one fence for drawing
        number_of_swap_chains = self._gDevice.get_number_of_swap_chains()
        _hr = self._draw_command_buffers.load(self._gDevice, number_of_swap_chains, pyWolf.graphics.w_command_buffer_level.PRIMARY)
        if _hr:
            print "Error on initializing draw command buffer(s)"
            return

        _hr = self.build_command_buffers()
        if _hr:
            print "Error on building draw command buffer(s)"
            return
        
        print "scene loaded successfully"

    def build_command_buffers(self):
        _hr = pyWolf.W_PASSED
        _size = self._draw_command_buffers.get_commands_size()
        for i in xrange(_size):
            _cmd = self._draw_command_buffers.get_command_at(i)
            _hr = self._draw_command_buffers.begin(i, pyWolf.graphics.w_command_buffer_usage_flag_bits.SIMULTANEOUS_USE_BIT)
            if _hr:
                print "Error on begining command buffer: " + str(i)
                break
            
            self._draw_render_pass.begin(i, _cmd, pyWolf.system.w_color.CORNFLOWER_BLUE(), 1.0, 0)
            #place your draw code
            self._draw_render_pass.end(_cmd)
            
            _hr = self._draw_command_buffers.end(i)
            if _hr:
                print "Error on ending command buffer: " + str(i)
                break

        return _hr

    def update(self, pGameTime):
        #++++++++++++++++++++++++++++++++++++++++++++++++++++
        #The following codes have been added for this project
        #++++++++++++++++++++++++++++++++++++++++++++++++++++
        #Update label of gui widget
        _debug_text = "FPS: " + str(pGameTime.get_frames_per_second()) + "\r\n\r\nFrameTime: " + str(pGameTime.get_elapsed_seconds()) + "\r\n\r\nTotalTime: " + str(pGameTime.get_total_seconds())
        self.signal.emit(_debug_text)
        #++++++++++++++++++++++++++++++++++++++++++++++++++++
        #++++++++++++++++++++++++++++++++++++++++++++++++++++

    def pre_render(self, pGameTime):
        _output_window = self._gDevice.output_presentation_window
        _frame_index = _output_window.swap_chain_image_index

        _wait_dst_stage_mask = [ pyWolf.graphics.w_pipeline_stage_flag_bits.COLOR_ATTACHMENT_OUTPUT_BIT ]
        _wait_semaphores = [ _output_window.swap_chain_image_is_available_semaphore ]
        _signal_semaphores = [ _output_window.rendering_done_semaphore ]
        _cmd = self._draw_command_buffers.get_command_at(_frame_index)
        _cmd_buffers = [_cmd ]       

        #reset draw fence
        self._draw_fence.reset()
        _hr = self._gDevice.submit(_cmd_buffers, self._gDevice.graphics_queue, _wait_dst_stage_mask, _wait_semaphores, _signal_semaphores, self._draw_fence)
        if _hr:
            print "Error on submit to graphics device"
            return 

        _hr = self._draw_fence.wait()
        if _hr:
            print "Error on waiting for draw fence"
            return 

    
    def post_render(self, pSuccessfullyRendered):
        if pSuccessfullyRendered == False:
            print "Rendered Unsuccessfully"
    
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
                self.release()
                break
            self._game.run(_map_info)

        print "Game exited"

    def showEvent(self, event):
        #run in another thread
        threading.Thread(target=self.run).start()
        event.accept()

    def closeEvent(self, event):
        self.__exiting = True
        event.accept()

    def release(self):
        self._draw_fence.release()
        self._draw_fence = None
        
        self._draw_semaphore.release()
        self._draw_semaphore = None
        
        self._draw_command_buffers.release()
        self._draw_command_buffers = None

        self._draw_render_pass.release()
        self._draw_render_pass = None

        self._game.exit()
        self._game = None
        self._gDevice = None
        self._viewport = None
        self._viewport_scissor = None
        
        
if __name__ == '__main__':
    # Create a Qt application
    _app = QApplication(sys.argv)
    
    #Show gui
    _gui = gui()
    _gui.resize(screen_width /2, screen_height /2)
    _gui.setWindowTitle('Wolf.Engine Debug')

    #Init scene
    _scene = scene(pyWolfPath + "..\\..\\..\\..\\content\\",
                  pyWolfPath,
                  "py_10_gui")
    _scene.resize(screen_width, screen_height)
    _scene.setWindowTitle('Wolf.Engine')

    #++++++++++++++++++++++++++++++++++++++++++++++++++++
    #The following codes have been added for this project
    #++++++++++++++++++++++++++++++++++++++++++++++++++++
    _scene.signal.connect(_gui.slot)
    #++++++++++++++++++++++++++++++++++++++++++++++++++++
    #++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    #Show all widgets
    _scene.show()
    _gui.show()
    
    sys.exit(_app.exec_())
