import os
import thread
import PyWolf

def RunWolf():
    #get the current path
    _current_path = os.path.dirname(os.path.abspath(__file__))
    _hr = PyWolf.initialize("PyWolf", _current_path, "content")
    if _hr == 1 :
        print "Error on initializing PyWolf"
        return

    PyWolf.release()
    print "\r\nPyWolf successfully released"
    return

def main():
    try:
        thread.start_new_thread( RunWolf, () )
    except:
        print "Error: unable to start thread for PyWolf"

if __name__ == '__main__':
	main()