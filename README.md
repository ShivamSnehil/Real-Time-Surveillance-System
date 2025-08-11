# Real-Time-Surveillance-System (C++/OpenCV)

A simple multi-camera surveillance that detects people using an OpenCV Haar cascade and triggers an audible alarm. 

## ✨ Features
- Monitor **N** cameras concurrently (USB index like `0`, `1`, or IP/RTSP URLs).
- Person detection via **Haar cascade** (`haarcascade_fullbody.xml`).
- Draws bounding boxes on detections.
- **Audible alarm** (Windows `PlaySound`) when a person is detected.
- Per-camera preview windows; **ESC** to close a camera.
- Console **Enter** to stop the alarm.

## 🧭 Architecture & Flow
- **Main**: loads cascade → reads camera sources → spawns one worker thread per camera.
- **Camera worker** (`monitorCamera`): open stream → read frames → grayscale → `detectMultiScale` → draw boxes → show window.
- **Alarm worker** (`playAlarm`): plays `alert.wav` asynchronously while a global flag is set.

```
+----------------+          +----------------------------+
|   main()       |          |  playAlarm() (detached)    |
| - load cascade | <------> | - while alarmActive:       |
| - spawn N cams |          |   PlaySound("alert.wav")   |
+-------+--------+          +--------------+-------------+
        |                                   ^
        v                                   |
+-------+------------------------------+    |
| monitorCamera(source, cascade)       |----+ (set alarmActive=true on first detection)
| - VideoCapture open (index or URL)   |
| - loop: read → gray → detect → draw  |
| - imshow / waitKey(30)               |
+--------------------------------------+
```

## 🧩 Requirements
- **OS**: Windows 10/11 (uses `windows.h` / `PlaySound`).
- **Compiler**: MSVC 2019+ (Visual Studio) or MinGW (MSVC recommended).
- **OpenCV**: 4.5+ (modules: core, imgproc, highgui, objdetect, videoio).
- **Files in working directory**:
  - `haarcascade_fullbody.xml`
  - `alert.wav` 
