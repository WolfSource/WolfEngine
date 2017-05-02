# Project          : Wolf Engine. Copyright(c) Pooya Eimandar ("http://PooyaEimandar.com") . All rights reserved.
# Source           : Please direct any bug to "https://github.com/WolfSource/Wolf.Engine/issues"
# Website          : "http://WolfSource.io"
# Name             : wolf_syncer_max.py
# Description      : Python script for 3d max which is responsible to sync 3d max data and Wolf Engine
# Comment          :

import os
#import sys
import MaxPlus

#print sys.version

# try load PySide2 for 3DMax 2018 and the later version
try:
    from PySide2 import QtCore, QtGui, QtWidgets
except:
    from PySide import QtCore, QtGui

can_exit = True
class _GCProtector(object):
    widgets = []

def on_selection_changed(code):
    
    return

class PropertyGrids(QtGui.QWidget):
    def __init__(self, parent= None):
     super(PropertyGrids, self).__init__(parent)
     self.setFixedHeight(350)
 
     widget = QtGui.QWidget()
     #Layout of Container Widget
     layout = QtGui.QVBoxLayout(self)
     for _ in range(20):
         hlayout = QtGui.QHBoxLayout()
         hlayout.addWidget(QtGui.QLabel("Item    "))
         hlayout.addWidget(QtGui.QLineEdit("Value"))
         layout.addLayout(hlayout)
     widget.setLayout(layout)
     
     #Scroll Area Properties
     scroll = QtGui.QScrollArea()
     scroll.setWidgetResizable(False)
     scroll.setWidget(widget)

     #Scroll Area Layer add 
     vLayout = QtGui.QVBoxLayout(self)
     vLayout.addWidget(scroll)
     self.setLayout(vLayout)

class MainWindow(QtGui.QWidget):
    
    def __init__(self, parent = MaxPlus.GetQMaxWindow() ) :
        super(MainWindow, self).__init__(parent)
        self.create_layout()
        _GCProtector.widgets.append(self)

    def create_layout(self):

        # +++++++++++++++++++++++++++++++++++++++++++++++
        # +++++++++++++++++++ Left Up +++++++++++++++++++
        # +++++++++++++++++++++++++++++++++++++++++++++++

        # Wolf Directory    |__________________| |browse|
        _engine_path_label = QtGui.QLabel(" Wolf Directory      ")
        _engine_path_text = QtGui.QLineEdit()
        _engine_path_browse_btn = QtGui.QPushButton("browse")

        _engine_path_layout =  QtGui.QHBoxLayout()
        _engine_path_layout.addWidget(_engine_path_label)
        _engine_path_layout.addWidget(_engine_path_text)
        _engine_path_layout.addWidget(_engine_path_browse_btn)

        # Content Directory |__________________| |browse|
        _content_path_label = QtGui.QLabel(" Content Directory")
        _content_path_text = QtGui.QLineEdit()
        _content_path_browse_btn = QtGui.QPushButton("browse")

        _content_path_layout =  QtGui.QHBoxLayout()
        _content_path_layout.addWidget(_content_path_label)
        _content_path_layout.addWidget(_content_path_text)
        _content_path_layout.addWidget(_content_path_browse_btn)      

        # Ip |__________________| Port |    |
        _ip_label = QtGui.QLabel(" Ip")
        _ip_text = QtGui.QLineEdit("127.0.0.1")
        _port_label = QtGui.QLabel("Port")
        _port_text = QtGui.QLineEdit("1717")

        _ip_port_layout =  QtGui.QHBoxLayout()
        _ip_port_layout.addWidget(_ip_label)
        _ip_port_layout.addWidget(_ip_text)
        _ip_port_layout.addWidget(_port_label)
        _ip_port_layout.addWidget(_port_text)

        left_first_left_layout = QtGui.QVBoxLayout()
        
        left_00_layout = QtGui.QVBoxLayout()
        left_00_layout.addLayout(_engine_path_layout)
        left_00_layout.addLayout(_content_path_layout)
        left_00_layout.addLayout(_ip_port_layout)

        _start_wolf_btn = QtGui.QPushButton("Start Wolf")
        _restart_wolf_btn = QtGui.QPushButton("Reset Wolf")
        _stop_wolf_btn = QtGui.QPushButton("Stop Wolf")

        left_01_layout = QtGui.QVBoxLayout()
        left_01_layout.addWidget(_start_wolf_btn)
        left_01_layout.addWidget(_restart_wolf_btn)
        left_01_layout.addWidget(_stop_wolf_btn)

        left_up_layout = QtGui.QHBoxLayout()
        left_up_layout.addLayout(left_00_layout)
        left_up_layout.addLayout(left_01_layout)

        # +++++++++++++++++++++++++++++++++++++++++++++++
        # +++++++++++++++++ Left Middle +++++++++++++++++
        # +++++++++++++++++++++++++++++++++++++++++++++++

        # |Sync|        |Send Selected| |Send First Camera|
        # |Send Lights| |Send Textures| |     Send All    |
        _sync_btn = QtGui.QPushButton("Sync") #Send name of deleted objects
        _send_selected_btn = QtGui.QPushButton("Send Selected") #Send selected objects to '~/content/scenes/' folder
        _send_first_camera_btn = QtGui.QPushButton("Send First Camera") #Send first or default camera (Camera001)
        _send_lights_btn = QtGui.QPushButton("Send Lights") #Send all lights to '~/content/scenes/' folder
        _send_all_textures_btn = QtGui.QPushButton("Send Textures") #Send all textures to '~/content/textures/' folder
        _sync_all_btn = QtGui.QPushButton("Send All") #Send all objects, camera, lights and textures

        left_middle_0_layout = QtGui.QHBoxLayout()
        left_middle_0_layout.addWidget(_sync_btn)
        left_middle_0_layout.addWidget(_send_selected_btn)
        left_middle_0_layout.addWidget(_send_first_camera_btn)

        left_middle_1_layout = QtGui.QHBoxLayout()
        left_middle_1_layout.addWidget(_send_lights_btn)
        left_middle_1_layout.addWidget(_send_all_textures_btn)
        left_middle_1_layout.addWidget(_sync_all_btn)

        left_middle_layout = QtGui.QVBoxLayout()
        left_middle_layout.addLayout(left_middle_0_layout)
        left_middle_layout.addLayout(left_middle_1_layout)

        # +++++++++++++++++++++++++++++++++++++++++++++++
        # ++++++++++++++++++ Left Down ++++++++++++++++++
        # +++++++++++++++++++++++++++++++++++++++++++++++

        # Log Textbox 
        text_log_history = QtGui.QTextEdit()
        text_log_history.setReadOnly(True)

        left_bottom_layout = QtGui.QVBoxLayout()
        left_bottom_layout.addWidget(text_log_history)
        left_bottom_layout.addWidget(QtGui.QPushButton("Clear History Log"))
        
        # +++++++++++++++++++++++++++++++++++++++++++++++
        # ++++++++++++++++++++ Left  ++++++++++++++++++++
        # +++++++++++++++++++++++++++++++++++++++++++++++
        left_layout = QtGui.QVBoxLayout()
        left_layout.addLayout(left_up_layout)
        left_layout.addLayout(left_middle_layout)
        left_layout.addLayout(left_bottom_layout)
        
        # +++++++++++++++++++++++++++++++++++++++++++++++
        # +++++++++++++++++++ Right  ++++++++++++++++++++
        # +++++++++++++++++++++++++++++++++++++++++++++++
        
        right_layout = QtGui.QHBoxLayout()
        right_layout.addWidget(PropertyGrids(self))

        # +++++++++++++++++++++++++++++++++++++++++++++++
        # +++++++++++++++++++ Main  +++++++++++++++++++++
        # +++++++++++++++++++++++++++++++++++++++++++++++

        main_up_layout = QtGui.QHBoxLayout()
        main_up_layout.addLayout(left_layout)
        main_up_layout.addLayout(right_layout)

        main_layout = QtGui.QVBoxLayout()
        main_layout.addLayout(main_up_layout)
        main_layout.addWidget(QtGui.QLabel("Last Error:"))
        main_layout.addWidget(QtGui.QLabel("Wolf Engine. Copyright(c) Pooya Eimandar \"http://WolfSource.io\". All rights reserved."))
        main_layout.addStretch()

        self.setLayout(main_layout)
        
        return

    def closeEvent(self, event):
        print "Exit"
        if can_exit:
            _GCProtector.widgets.remove(self) # Let the window close
            print "Main window closed"
            event.accept()
        else:
            print "closing Main window ignored"
            event.ignore()
        return

def main():
    MaxPlus.NotificationManager.Register(MaxPlus.NotificationCodes.SelectionsetChanged, on_selection_changed)
    
    _window = MainWindow()
    _window.resize(500, 400)
    _window.setWindowTitle('Wolf Engine Syncer for OpenCollada')
    _window.show()
    
    return
    
    
if __name__ == '__main__':
    main()
