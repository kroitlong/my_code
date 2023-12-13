import time
import cv2


# 摄像头捕获异常处理
def handle_camera_error(e):
    print(e)
    # 循环检测摄像头是否打开
    while True:
        cap = cv2.VideoCapture(0)
        if cap.isOpened():
            print("摄像头已打开")
            cap.release()
            break
        else:
            print("等待打开中...")
            time.sleep(3)


# 人脸捕获异常处理
def handle_face_detection_error(message):
    print("Face detection error occurred. Check the face detection model or input.")
