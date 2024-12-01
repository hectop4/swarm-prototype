from pynput import keyboard
import serial
# Initialize serial connection (adjust 'COM3' and baudrate as needed)
ser = serial.Serial('/dev/ttyUSB0', 115200)

def send_to_serial(data):   
    ser.write(data.encode())
def on_press(key):
    try:
        if key == keyboard.Key.up:
            print("1")
            send_to_serial('1')
        elif key == keyboard.Key.down:
            print("2")
            send_to_serial("2")
        elif key == keyboard.Key.left:
            print("3")
            send_to_serial("3")
        elif key == keyboard.Key.right:
            print("4")
            send_to_serial("4")
    except AttributeError:
        pass

def on_release(key):
    try:
        if key == keyboard.Key.up:
            print("5")
            send_to_serial("5")
        elif key == keyboard.Key.down:
            print("6")
            send_to_serial("6")
        elif key == keyboard.Key.left:
            print("7")
            send_to_serial("7")
        elif key == keyboard.Key.right:
            print("8")
            send_to_serial("8")
    except AttributeError:
        pass

# Collect events until released
with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()