# 自定义摄像头捕获异常
class CameraError(Exception):
    def __init__(self, message):
        super().__init__(message)


# 自定义人脸捕获异常
class FaceDetectionError(Exception):
    def __init__(self, message):
        super().__init__(message)
