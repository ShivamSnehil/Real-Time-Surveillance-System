#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <windows.h> // For PlaySound

using namespace std;
using namespace cv;

bool alarmActive = false;

void playAlarm() {
    while (alarmActive) {
        PlaySound(TEXT("alert.wav"), NULL, SND_FILENAME | SND_ASYNC);
        this_thread::sleep_for(chrono::seconds(1));
    }
    PlaySound(NULL, 0, 0); // Stop sound
}

void monitorCamera(string source, CascadeClassifier& bodyCascade) {
    VideoCapture cap;
    if (isdigit(source[0])) {
        cap.open(stoi(source)); // Use device index
    }
    else {
        cap.open(source); // Use IP camera
    }

    if (!cap.isOpened()) {
        cerr << "❌ Cannot open camera: " << source << endl;
        return;
    }

    Mat frame, gray;
    vector<Rect> bodies;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        bodyCascade.detectMultiScale(gray, bodies);

        for (Rect body : bodies) {
            rectangle(frame, body, Scalar(0, 255, 0), 2);
        }

        if (!bodies.empty() && !alarmActive) {
            alarmActive = true;
            thread(playAlarm).detach();
            cout << "🚨 Person detected on camera: " << source << endl;
        }

        imshow("Camera " + source, frame);
        if (waitKey(30) == 27) break; // ESC to quit
    }

    cap.release();
    destroyWindow("Camera " + source);
}

int main() {
    CascadeClassifier bodyCascade;
    if (!bodyCascade.load("haarcascade_fullbody.xml")) {
        cerr << "❌ Failed to load cascade file!" << endl;
        return -1;
    }

    int numCams;
    cout << "Enter number of cameras: ";
    cin >> numCams;

    vector<string> sources(numCams);
    cin.ignore(); // Flush newline
    for (int i = 0; i < numCams; ++i) {
        cout << "Enter camera " << i + 1 << " IP or index (0, 1, rtsp://, etc.): ";
        getline(cin, sources[i]);
    }

    vector<thread> threads;
    for (const string& source : sources) {
        threads.emplace_back(monitorCamera, source, ref(bodyCascade));
    }

    cout << "Press Enter to stop the alarm..." << endl;
    cin.get();
    alarmActive = false;

    for (auto& t : threads) t.join();
    return 0;
}
