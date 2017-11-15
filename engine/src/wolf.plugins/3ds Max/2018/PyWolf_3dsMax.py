import sys
import os
import MaxPlus
import math
import ctypes
import thread
import datetime

#import tempfile
#import shutil

#append search path for PyWolf
#PyWolfPath = "E:\\SourceCode\\github\\WolfSource\\Wolf.Engine\\bin\\x64\\Debug\\Win32"
PyWolfPath = "F:\\github\\WolfSource\\Wolf.Engine\\bin\\x64\\Debug\\Win32"

if PyWolfPath != "" and (not PyWolfPath in sys.path):
	sys.path.append(PyWolfPath)

wolf_version = ""

try:
	import PyWolf
	wolf_version = "PyWolf (v.1.44.7.15)"
except:
	print "Error, could not find PyWolf"
	
from collections import defaultdict
from sys import maxint

#create temp folder
#tmp_dir = tempfile.mkdtemp()

#check pyside version
pyside_version = -1
try:
	from PySide2 import QtWidgets, QtCore
	from PySide2.QtWidgets import *
	from PySide2.QtCore import *
	pyside_version = 2
except:
	try:
		from PySide import QtGui, QtCore
		from PySide.QtGui import *
		from PySide.QtCore import *
		pyside_version = 1
	except:
		print "Error, could not find PySide2 or PySide"

if 	pyside_version == -1:
	sys.exit(1)

#_pywolf_lock = 0
#class WolfLock:
#	global pywolf_lock
#	def pywolf_lock():
#		_pywolf_lock = 1
		
#	global pywolf_unlock
#	def pywolf_unlock():
#		_pywolf_lock = 0
    
#	global pywolf_is_lock
#	def pywolf_is_lock():
#		return _pywolf_lock 

class _GCProtector(object):
    widgets = []

app = QApplication.instance()
if not app:
	app = QApplication([])

class Logger(QPlainTextEdit):
    def __init__(self, parent = None):
		super(Logger, self).__init__(parent)
		self.setReadOnly(1)
  
    def log(self, pMsg):
        self.appendPlainText(pMsg)

class WolfWidget(QWidget):
    def __init__(self, parent = None):
		super(WolfWidget, self).__init__(parent)
  
    def closeEvent(self, event):
		# just release all models
        event.accept()

class CallBacks():
    def __init__(self):
		self.OnSystemShutDownHandle = None
		
    def register_all(self):
		self.OnSystemShutDownHandle = MaxPlus.NotificationManager.Register(MaxPlus.NotificationCodes.SystemShutdown, OnSystemShutDown)
	
    def unregister_all(self):
		MaxPlus.NotificationManager.Unregister(self.OnSystemShutDownHandle)
		self.OnSystemShutDownHandle = None

    OnSystemShutDownHandle = None

#callbacks
callbacks = CallBacks()
#wolf widget
wolfWidget = WolfWidget()
WIDTH = 640
HEIGHT = 480

class Dialog(QDialog):
    def __init__(self, parent = None):
		super(Dialog, self).__init__(parent)
  
    def closeEvent(self, event):
		#clear temp
        #shutil.rmtree(tmp_dir)
        pywolf_release() 
        event.accept()
		
#Global variables
preNameTextBox = QTextEdit("CG")
preNameTextBox.setMaximumHeight(23)

refModelTextBox = QTextEdit("")
refModelTextBox.setMaximumHeight(23)

logger = Logger()
cmd = Logger()
#old_instances = defaultdict(list)

def quaternion_to_euler_angle(w, x, y, z):
	ysqr = y * y
	
	t0 = +2.0 * (w * x + y * z)
	t1 = +1.0 - 2.0 * (x * x + ysqr)
	X = math.degrees(math.atan2(t0, t1))
	
	t2 = +2.0 * (w * y - z * x)
	t2 = +1.0 if t2 > +1.0 else t2
	t2 = -1.0 if t2 < -1.0 else t2
	Y = math.degrees(math.asin(t2))
	
	t3 = +2.0 * (w * z + x * y)
	t4 = +1.0 - 2.0 * (ysqr + z * z)
	Z = math.degrees(math.atan2(t3, t4))
	
	return X, Y, Z

def to_quaternion(pPitch, pRoll, pYaw):
	#Abbreviations for the various angular functions
	cy = math.cos(pYaw * 0.5)
	sy = math.sin(pYaw * 0.5)
	cr = math.cos(pRoll * 0.5)
	sr = math.sin(pRoll * 0.5)
	cp = math.cos(pPitch * 0.5)
	sp = math.sin(pPitch * 0.5)

	w = cy * cr * cp + sy * sr * sp
	x = cy * sr * cp - sy * cr * sp
	y = cy * cr * sp + sy * sr * cp
	z = sy * cr * cp - cy * sr * sp
	return w, x, y, z 

def in_range_of(pNum, pMin, pMax):
	return pNum >= pMin and pNum <= pMax
	
def apply_editpoly_reset_xform_selected_model(pName):
	if pName == "":
		return
	
	_max_sxript_cmd = "select $" + pName + "									\r\n\
	macros.run \"Modifier Stack\" \"Convert_to_Poly\"					\r\n\
	ResetXForm $" +  pName + "														\r\n\
	macros.run \"Modifier Stack\" \"Convert_to_Poly\"					\r\n\
	"
	MaxPlus.Core.EvalMAXScript(_max_sxript_cmd)
	
def apply_editpoly_reset_xform_selected_models(pNames):
	_l = len(pNames)
	if _l == 0:
		return

	_str = "#("
	for i in range(0, _l):
		_str = _str + "$" + pNames[i]
		if i != len(pNames) - 1 :
			_str = _str + ","	

	_str = _str +  ")"
	
	_max_sxript_cmd = "select " + _str + "										\r\n\
	macros.run \"Modifier Stack\" \"Convert_to_Poly\"					\r\n\
	ResetXForm " +  _str + "																\r\n\
	macros.run \"Modifier Stack\" \"Convert_to_Poly\"					\r\n\
	"
	MaxPlus.Core.EvalMAXScript(_max_sxript_cmd)
	
def get_standard_index_name(pIndex):
	_index_str = ""
	if pIndex < 10 :
		_index_str = "00" + str(pIndex)
	elif pIndex < 100 :
		_index_str = "0" + str(pIndex)
	else:
		_index_str = str(pIndex)
	return _index_str

def get_unique_name():
	return datetime.datetime.now().strftime('%Y_%m_%d_%H_%M_%S')

def select_ref_node():
	_selected_count = MaxPlus.SelectionManager.GetCount()
	if _selected_count == 0:
		logger.log("Nothing selected as a Ref node from current active layer")
	elif _selected_count == 1:
		for c in MaxPlus.SelectionManager.Nodes:
			print c.Name
			refModelTextBox.setText(c.Name)
	else:
		logger.log("Please select one node as a Ref node from current active layer")
	
def reinstance_current_layer():
	pre_name = preNameTextBox.toPlainText()
	
	_layer = MaxPlus.LayerManager_GetCurrentLayer()
	_n = _layer.GetName()
	
	if _n == "0" or _n == "_Problems_" or _n == "_Boundaries_" or _n == "_Inner_Layers_" or _n == "_Middle_Layers_" or _n == "_Outer_Layers_":
		logger.log("The following layers not allowed: 0(default), _Inner_Layers_, _Middle_Layers_, _Outer_Layers_, _Problems_ and _Boundaries_ . Move your nodes to new layer.")
		refModelTextBox.setText("")
		return
	
	y = _n.startswith('Layer')
	if not y:
		logger.log("Layer name \"" + _n + "\" must have start with \"Layer\"")
		refModelTextBox.setText("")
		return
	
	#get layer index as str
	_layer_index_str = str(_n .replace('Layer', ''))
	#check is it valid ?
	_layer_index = 0
	
	try:
		_layer_index = int(_layer_index_str)
	except:
		_layer_index = -maxint - 1
		
	if _layer_index == (-maxint - 1):
		logger.log("Layer \"" + _n + "\" must have in following format: Layer + Number; for example Layer001, Layer1, Layer1000")
		refModelTextBox.setText("")
		return
	
	_layer_index_str = get_standard_index_name(_layer_index)
		
	#find ref node
	No_Ref_Founded = 0
	_ref_index = -1
	_lod_index = -1
	nodes = _layer.GetNodes()
	instances_index = []
	
	_len_nodes = len(nodes)
	if _len_nodes == 0:
		logger.log("Layer \"" + _n + "\" does not have any node")
		refModelTextBox.setText("")
		return
	
	_preferred_ref_name = refModelTextBox.toPlainText()
	#check for ref
	for j in range(0, _len_nodes):
		_node = nodes[j]
		_rotate = _node.GetWorldRotation()
		_rx, _ry, _rz = quaternion_to_euler_angle(_rotate.GetW(), _rotate.GetX(), _rotate.GetY(), _rotate.GetZ())	
		_node_name = _node.Name
			
		if _node_name.find("-ch") != -1:
			continue
		
		if _node_name.find("-lod") != -1:
			_lod_index = j
			continue
		
		if _preferred_ref_name == "":
			if in_range_of(_rx, -0.1, 0.1) and in_range_of(_ry, -0.1, 0.1) and in_range_of(_rz, -0.1, 0.1):
				_ref_index = j
				break
		else:
			if _preferred_ref_name == _node_name and in_range_of(_rx, -0.1, 0.1) and in_range_of(_ry, -0.1, 0.1) and in_range_of(_rz, -0.1, 0.1):
				_ref_index = j
				break
	
	if _ref_index == -1:
		logger.log("Layer \"" + _n + "\" does not have ref object, At least one node should contain zero rotation or may be the preferred Ref node is not child of current active layer")
		refModelTextBox.setText("")
		return
		
	_ref =  nodes[_ref_index]
	logger.log("using " + _ref.Name + " as ref node")
	
	for j in range(0, _len_nodes):
		_node = nodes[j]
		_rotate = _node.GetWorldRotation()
		_rx, _ry, _rz = quaternion_to_euler_angle(_rotate.GetW(), _rotate.GetX(), _rotate.GetY(), _rotate.GetZ())	
		_node_name = _node.Name
			
		#convex hull
		if _node_name.find("-ch") != -1:
			continue
		
		#lod
		if _node_name.find("-lod") != -1:
			_lod_index = j
			continue
		
		#ref
		if j == _ref_index:
			continue
			
		_node.Name = "(OLD-INS)" + _node.Name
		instances_index.append(j)
	
	#get ref node Equipment Name
	_equipment_name = ""
	_splits = _ref.Name.split("_", 1)
	if len(_splits) > 1:
		_equipment_name = _splits[1]	
		
	_ref.Name = pre_name + _layer_index_str + "_" + _equipment_name
		
	#move lod to ref location
	if _lod_index != -1:
		_lod =  nodes[_lod_index]
		_ref_pos = _ref.Position
		_lod.SetPositionX(_ref_pos.GetX())
		_lod.SetPositionY(_ref_pos.GetY())
		_lod.SetPositionZ(_ref_pos.GetZ())
		
	#apply convert to editpoly, reset xform 
	apply_editpoly_reset_xform_selected_model(_ref.Name)
	
	default_mat = MaxPlus.Factory.CreateDefaultStdMat() 
	default_mat.Ambient = MaxPlus.Color(0.0, 1.0, 0.0) 
	default_mat.Diffuse = MaxPlus.Color(0.0, 1.0, 0.0) 
	default_mat.Specular = MaxPlus.Color(1.0, 1.0, 1.0) 
	default_mat.Shininess = 0.5 
	default_mat.ShinyStrength = 0.7 

	num_instances = len(instances_index)
	for k in xrange(0, num_instances):
		old_ins = nodes[instances_index[k]]
		pos = old_ins.Position
		rot = old_ins.GetWorldRotation()
		
		_equipment_name = ""
		#get ref node Equipment Name
		_splits = old_ins.Name.split("_", 1)
		if len(_splits) > 1:
			_equipment_name = _splits[1]
		
		#create instance
		instance = _ref.CreateTreeInstance()
		instance.Name = pre_name + _layer_index_str + "-ins" +  get_standard_index_name(k + 1) + "_" + _equipment_name
		instance.SetPositionX(pos.GetX())
		instance.SetPositionY(pos.GetY())
		instance.SetPositionZ(pos.GetZ())
		instance.SetWorldRotation(rot)
			
		#change color of old instance
		old_ins.Material = default_mat
		#old_ins.WireColor = MaxPlus.Color(1.0, 0.0, 0.0)
		#store to old instances
		##################################################old_instances[0].append(old_ins)
	
	refModelTextBox.setText("")
	
#def delete_old_nodes():
	#delete all nodes
	#do_not_add_for_first = 1
	#_max_sxript_cmd = "select #("
	#for k in old_instances:
		#for v in old_instances[k]:
			#if do_not_add_for_first:
				#do_not_add_for_first = 0
				#_max_sxript_cmd = _max_sxript_cmd + "$\'" + v.Name + "\'"
			#else:
				#_max_sxript_cmd = _max_sxript_cmd + ",$\'" + v.Name + "\'"
	
	#_max_sxript_cmd =  _max_sxript_cmd + ")											\r\n\
	#actionMan.executeAction 0 \"40020\"  -- Edit: Delete Objects	\r\n\"
	#MaxPlus.Core.EvalMAXScript(_max_sxript_cmd)
	#old_instances.clear()

def reset_nodes():
	_selected_count = MaxPlus.SelectionManager.GetCount()
	if _selected_count == 0:
		logger.log("Nothing selected for reseting")
	
	names = []
	for c in MaxPlus.SelectionManager.Nodes:
		names.append(c.Name)
	
	if len(names) == 1:
		apply_editpoly_reset_xform_selected_model(names[0])
	else:
		apply_editpoly_reset_xform_selected_models(names)

def clear_log():
	logger.clear()
	logger.appendPlainText("Wolf Engine Plugin for Autodesk 3ds Max "  + wolf_version)
	#logger.appendPlainText("Temp Path: \""  + tmp_dir + "\"")
	
def pywolf_show():
    if wolf_version == "" :
		logger.log("PyWolf not available")
		return
	#show widget
    wolfWidget.show()

def pywolf_sync_camera():
	if wolf_version == "" or (not wolfWidget.isVisible()):
		return
		
	MaxPlus.SelectionManager.ClearNodeSelection()
	#create camera from viewport, there is a bug which camera target not updated in Max 2018 so we need to do a trick an update the sliderTime a bit
	_max_sxript_cmd = "macros.run \"Lights and Cameras\" \"PhysicalCamera_CreateFromView\" \r\n\
	v = sliderTime \r\n\
	sliderTime = v + 1 \r\n\
	sliderTime = v"
	MaxPlus.Core.EvalMAXScript(_max_sxript_cmd)
	_camera_node = MaxPlus.SelectionManager.GetNode(0)
	_camera_name =_camera_node.GetName()

	_max_sxript_cmd = "select #($'" + _camera_name + "', $'" + _camera_name + ".Target')"
	MaxPlus.Core.EvalMAXScript(_max_sxript_cmd)

	if MaxPlus.SelectionManager.GetCount() == 2:
		_camera_node = MaxPlus.SelectionManager.GetNode(1)
		_camera_name =_camera_node.GetName()
		
		look_atX = 0.0
		look_atY = 0.0
		look_atZ = 0.0
		
		posX = 0.0
		posY = 0.0
		posZ = 0.0
		
		if _camera_name.find("Target"):
			look_atX = _camera_node.GetPositionX()
			look_atY = _camera_node.GetPositionY()
			look_atZ = _camera_node.GetPositionZ()
			_camera_node = MaxPlus.SelectionManager.GetNode(0)
			posX = _camera_node.GetPositionX()
			posY = _camera_node.GetPositionZ()
			posZ = _camera_node.GetPositionY()
		else:
			posX = _camera_node.GetPositionX()
			posY = _camera_node.GetPositionY()
			posZ = _camera_node.GetPositionZ()
			_camera_node = MaxPlus.SelectionManager.GetNode(0)
			look_atX = _camera_node.GetPositionX()
			look_atY = _camera_node.GetPositionY()
			look_atZ = _camera_node.GetPositionZ()
		
		#delete objects
		_max_sxript_cmd = "actionMan.executeAction 0 \"40020\"  -- Edit: Delete Objects"
		MaxPlus.Core.EvalMAXScript(_max_sxript_cmd)
		
		_hr = PyWolf.set_camera_position(posX, posY, posZ)
		if _hr != 0:
			logger.log("Error on syncing camera position")	
				
		_hr = PyWolf.set_camera_lookat(look_atX, look_atZ, look_atY)
		if _hr != 0:
			logger.log("Error on syncing camera look at")
	

def pywolf_sync_active_layer_models():
	if wolf_version == "":
		logger.log("PyWolf not available")
		return
			
	#first select nodes of active layer
	MaxPlus.SelectionManager.ClearNodeSelection()
	_layer = MaxPlus.LayerManager.GetCurrentLayer()
	_layer_name = _layer.GetName()
	if _layer_name == "_Problems_" or _layer_name == "_Boundaries_" or _layer_name == "_Inner_Layers_" or _layer_name == "_Middle_Layers_" or _layer_name == "_Outer_Layers_":
		logger.log("Could not add following Layers: _Problems_, _Boundaries_, _Inner_Layers_, _Middle_Layers_, _Outer_Layers_")
		return
	
	_selecting_nodes = MaxPlus.INodeTab()
	for _node in _layer.GetNodes():
		_selecting_nodes.Append(_node)
	if len(_selecting_nodes) != 0:
		#select
		MaxPlus.SelectionManager.SelectNodes(_selecting_nodes)
		#_collada_exp_path = tmp_dir + "\W" + get_unique_name() + ".DAE"
		#export the scene in the format of OpenCollada
		_max_sxript_cmd = "exp_classes = exporterPlugin.classes																\r\n\
		_idx = findItem exp_classes OpenCOLLADAExporter																			\r\n\
		if _idx != 0 do 																															\r\n\
		(																																					\r\n\
		   file_url = \"c:\\Wolf\\models.DAE\"																				   	\r\n\
		   exportFile (file_url) #noprompt selectedOnly:on using:exp_classes[_idx]								\r\n\
     )"
		MaxPlus.Core.EvalMAXScript(_max_sxript_cmd)
		
		#now call PyWolf to load the scene
		_hr = PyWolf.load_scene("c:\\Wolf\\models.DAE")
		if	_hr != 0:
			logger.log("Could not sync active layer with Wolf. Make sure run 3Ds Max as administrator and OpenCollada plugin is available")
		pywolf_sync_camera()


def pywolf_remove_all_models():
	if wolf_version == "":
		logger.log("PyWolf not available")
		return
	_hr = PyWolf.remove_all_models()
	if	_hr != 0:
		logger.log("Could not remove all models")
		
def pywolf_release():
    wolfWidget.close()
    PyWolf.release()
    callbacks.unregister_all()
    print "PyWolf shut down successfully"
	
def execute_cmd():
	_cmd = cmd.toPlainText()
	exec(_cmd)
	logger.log(_cmd)
	
def init_reinstance_layout():
	widget = QWidget()
	
	label = QLabel("PreName")
	
	button_1 = QPushButton("Select Ref node ")
	button_1.clicked.connect(select_ref_node)
	button_1.setToolTip("Select preferred node as a parent of other instnaces. First select 3D object and then press me")
	
	button_2 = QPushButton("ReInstance ")
	button_2.clicked.connect(reinstance_current_layer)
	button_2.setToolTip("I'm responisble to re-instnace all nodes of active layer except LOD and Convex Hull objects")
	
	#button_3 = QPushButton("Delete old node(s)  ")
	#button_3.setEnabled(0)#Bug in deleting
	#button_3.clicked.connect(delete_old_nodes)
	
	h_layout = QHBoxLayout(widget)
	h_layout.setAlignment(Qt.AlignTop)
	h_layout.addWidget(label)
	h_layout.addWidget(preNameTextBox)
	h_layout.addWidget(button_1)
	h_layout.addWidget(refModelTextBox)
	h_layout.addWidget(button_2)
	
	return widget

def init_reset_layout():
	widget = QWidget()
		
	button_1 = QPushButton("Reset selected model(s)")
	button_1.clicked.connect(reset_nodes)
	button_1.setToolTip("I'm responisble to reset xform selected models and covert them to editable polygons")
	
	h_layout = QHBoxLayout(widget)
	h_layout.setAlignment(Qt.AlignTop)
	h_layout.addWidget(button_1)
	
	return widget
	
def init_helper():
	widget = QWidget()
	
	#command input
	cmd.setReadOnly(0)
	
	#wolf buttons
	wolf_buttons = QWidget()
	
	#run wolf.engine
	run_wolf_button = QPushButton("Show Wolf")
	run_wolf_button.clicked.connect(pywolf_show)
	run_wolf_button.setToolTip("Show Wolf Engine Viewer")
	
	#sync current models
	sync_models_to_wolf_button = QPushButton("Export active layer to Wolf")
	sync_models_to_wolf_button.clicked.connect(pywolf_sync_active_layer_models)
	sync_models_to_wolf_button.setToolTip("Export nodes of active layer to Wolf Engine")
	
	#sync camera
	sync_camera_wolf_button = QPushButton("Sync camera")
	sync_camera_wolf_button.clicked.connect(pywolf_sync_camera)
	sync_camera_wolf_button.setToolTip("Sync camera of 3ds Max with camera of Wolf Engine")

	#shutdown wolf.engine
	remove_all_models_button = QPushButton("Remove All Models")
	remove_all_models_button.clicked.connect(pywolf_remove_all_models)
	remove_all_models_button.setToolTip("Remove all models in Wolf Engine")

	#execute python codes
	exe_button = QPushButton("execute")
	exe_button.clicked.connect(execute_cmd)
	exe_button.setToolTip("Run Python code")
		
	h_layout = QHBoxLayout(wolf_buttons)
	h_layout.addWidget(run_wolf_button)
	h_layout.addWidget(sync_models_to_wolf_button)
	h_layout.addWidget(sync_camera_wolf_button)
	h_layout.addWidget(remove_all_models_button)
	
	v_layout = QVBoxLayout(widget)
	v_layout.addWidget(wolf_buttons)
	v_layout.addWidget(QLabel("PyWolf (Python wrapper for Wolf.Engine)"))
	v_layout.addWidget(cmd)
	v_layout.addWidget(exe_button)
		
	return widget

def init_pywolf():
	widget = QWidget()
	
	v_layout = QVBoxLayout(widget)	
	v_layout.addWidget(QLabel("ReInstance active layer"))
	v_layout.addWidget(init_reinstance_layout())
	v_layout.addWidget(QLabel("Model Utilities"))
	v_layout.addWidget(init_reset_layout())

	return widget

def run_wolf():
    # get window handle
    pycobject_hwnd = wolfWidget.winId()
    #convert window handle as HWND to unsigned integer pointer for c++
    ctypes.pythonapi.PyCObject_AsVoidPtr.restype  = ctypes.c_void_p
    ctypes.pythonapi.PyCObject_AsVoidPtr.argtypes = [ctypes.py_object]
    int_hwnd = ctypes.pythonapi.PyCObject_AsVoidPtr(pycobject_hwnd)
    
    #get the current path
    _current_script_path = os.path.dirname(os.path.abspath(__file__))
    _hr = PyWolf.initialize(int_hwnd, "PyWolf", str(_current_script_path), "F:\\github\\WolfSource\\Wolf.Engine\\content\\")
    if _hr == 1 :
        print "Error on initializing PyWolf"
    else:
        logger.log( wolf_version + " launched")

	
def OnSystemShutDown(e):
	#shut down wolf
	if wolf_version != "":
		pywolf_release()
		PyWolf.release_shared_data_over_all_instances()
		
def main():
    dialog = Dialog(MaxPlus.GetQMaxMainWindow())
    dialog.resize(WIDTH, HEIGHT)
    _GCProtector.widgets.append(dialog)
	
    wolfWidget.setWindowTitle("PyWolf Viewer")
    wolfWidget.resize(WIDTH, HEIGHT)
    _GCProtector.widgets.append(wolfWidget)

    title = "Wolf Engine Plugin for Autodesk 3ds Max "  + wolf_version
    dialog.setWindowTitle(title)
	
	#create first tab	
    main_tab = QTabWidget()
    main_tab.addTab(init_pywolf(), "3Ds Max Helper")
    main_tab.addTab(init_helper(), "Wolf Viewer")
	
    clear_log()
    if wolf_version == "":
	    logger.log("Error, could not find PyWolf")
    else:
		#run main thread of wolf engine
        run_wolf()

    #clear log
	clear_log_btn = QPushButton("Clear Log")
	clear_log_btn.clicked.connect(clear_log)

	#create vertex box layout
	main_layout = QVBoxLayout()
	main_layout.addWidget(main_tab)
	main_layout.addWidget(logger)
	main_layout.addWidget(clear_log_btn)
	
    dialog.setLayout(main_layout)
    dialog.setWindowModality(Qt.NonModal)
    dialog.show()
	
    #register callbacks
    callbacks.register_all()
    
    print title + " just launched"
	
if __name__ == '__main__':
	main()
