import tkinter as tk
import requests

ESP_IP = "192.168.4.1"  # ESP32 in AP mode

def send_start(direction):
    try:
        requests.post(f"http://{ESP_IP}/move/start?dir={direction}")
        print(f"Sent start: {direction}")
    except Exception as e:
        print("Error:", e)

def send_stop():
    try:
        requests.post(f"http://{ESP_IP}/move/stop")
        print("Sent stop")
    except Exception as e:
        print("Error:", e)

def set_speed(val):
    try:
        speed = int(float(val))
        requests.post(f"http://{ESP_IP}/speed?val={speed}")
        print(f"Set speed: {speed}")
    except Exception as e:
        print("Error:", e)

def set_servo(val):
    try:
        pos = int(float(val))
        requests.post(f"http://{ESP_IP}/servo?val={pos}")
        print(f"Set servo: {pos}")
    except Exception as e:
        print("Error:", e)

def update_sensor_data():
    try:
        response = requests.get("http://192.168.4.1/sensor")  # Replace with ESP32 IP
        data = response.json()  # Assume ESP32 sends JSON like 

        sensor_label.config(text=f"Dist: {data['distance']}cm")
    except Exception as e:
        print("Sensor update failed:", e)
    root.after(100, update_sensor_data)  # poll every 1s


root = tk.Tk()
root.title("ESP32 RC Car Control")

# Control Buttons
controls = {
    "Forward": "forward",
    "Backward": "backward",
    "Left": "left",
    "Right": "right"
}

for i, (label, direction) in enumerate(controls.items()):
    btn = tk.Button(root, text=label, width=12)
    btn.grid(row=i, column=0, padx=10, pady=5)
    btn.bind('<ButtonPress>', lambda e, d=direction: send_start(d))
    btn.bind('<ButtonRelease>', lambda e: send_stop())

# Speed Control Slider
tk.Label(root, text="Speed").grid(row=0, column=1)
speed_slider = tk.Scale(root, from_=0, to=100, orient='vertical', command=set_speed)
speed_slider.set(50)
speed_slider.grid(row=1, column=1, rowspan=3)

# Speed Control Slider
tk.Label(root, text="Servo").grid(row=0, column=1)
servo_slider = tk.Scale(root, from_=0, to=180, orient='horizontal', command=set_servo)
servo_slider.set(90)
servo_slider.grid(row=1, column=2, columnspan=3)

# Sensor Data Label
sensor_label = tk.Label(root, text="Dist: -- cm", font=("Arial", 12), fg="blue")
sensor_label.grid(row=3, column=2, rowspan=3, padx=20)

# Start polling for sensor data
update_sensor_data()

root.mainloop()

