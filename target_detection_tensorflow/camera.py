import cv2
import keras

import Exceptions.exception_define as def_
import Exceptions.exception_handle as hdl


class Camera:
    def __init__(self):
        # 初始化摄像头和模型
        self.cap = cv2.VideoCapture(0)
        self.model = keras.models.load_model()

    def capture_video(self):
        while True:
            ret, frame = self.cap.read()  # 读取视频流中的一帧
            try:
                if not ret:
                    self.cap.release()
                    raise def_.CameraError("无法检测到摄像头，请确保摄像头已打开...")
                # 处理每帧的画面，将每帧(frame)传入模型进行预测，然后绘制框图

                cv2.imshow('Face Recognition', frame)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break

            except def_.CameraError as e:
                hdl.handle_camera_error(e)
                self.cap = cv2.VideoCapture(0)
                continue

        self.cap.release()
        cv2.destroyAllWindows()


if __name__ == "__main__":
    cam = Camera()
    cam.capture_video()
