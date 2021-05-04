from pynput.keyboard import Listener

def on_press(key):
    print('des')

def on_release(key):
    pass


with Listener(on_press=on_press, on_release=on_release)as listener:
    listener.join()