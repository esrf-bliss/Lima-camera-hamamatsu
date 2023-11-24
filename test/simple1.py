from Lima import Core, Hamamatsu

Core.DebParams.getModule
def new_func():
    cam = Hamamatsu.Camera("", 1 ,10)
    return cam

cam = new_func()


cam.getStatus()
