import sys, ctypes

#append search path for pyWolf in windows
#pyWolfPath = "/Users/pooyaeimandar/Documents/github/WolfSource/Wolf.Engine/bin/Build/macOS/Products/Debug/"
pyWolfPath = "D:\\github\\WolfSource\\Wolf.Engine\\bin\\x64\\Debug\\Win32\\"

if pyWolfPath != "" and (not pyWolfPath in sys.path):
    sys.path.append(pyWolfPath)

import pyWolf

def update():
    print "updating"

if __name__ == '__main__':
    bbox = pyWolf.system.w_bounding_box()
    sphere = pyWolf.system.w_bounding_sphere()
    
    bbox_from_sphere = pyWolf.bounding_box_from_bounding_sphere(sphere);

    _max = bbox_from_sphere.get_max()
    bbox.set_max(_max)

    v0 = (0.0, 0.0, 0.0)
    c = bbox.contains_point(v0)
    print type(c)

    color = pyWolf.system.w_color()
    c0 = (0.0, 1.0, 0.0, 0.0)
    color.to_color(c0);
    color.WHITE();

    game_time = pyWolf.system.w_game_time()
    game_time.reset()
    fps = game_time.get_frames_per_second()
    game_time.tick(update)# for sending null use "lambda: None" instead "update"

    _mouse = pyWolf.system.w_mouse()
    _keyboard = pyWolf.system.w_keyboard()
    _l0 = _keyboard.get_keys_pressed()
    _l1 = _keyboard.get_keys_released()
    _l2 = _keyboard.get_inputed_chars()

    _timer0 = pyWolf.system.w_timer()
    _timer0.start()
    print "pyWolf"
    print "pyWolf"
    print _timer0.get_seconds()
    _timer0.reset()

    _ts = pyWolf.system.w_time_span()
    _ts.now()
    print _ts.to_string()


    _enum_screens = pyWolf.system.w_enumerate_screens()
    _scrs = _enum_screens.get_screens()


    _window = pyWolf.system.w_window_info()
