# 실시간 특정 객체 모자이크 시스템
   
<p align="center">
<img src="./image/model.png" alt="model"/>
</p>

#### Paper
> https://www.koreascience.or.kr/article/CFKO201924664108405.page

#### 설명
방송에서는 당사자의 동의 없이 얼굴을 노출 시키거나, 유해물질로 판단되는 물체의 노출을 금지하고 있다. 기존의 처리방식으로 편집자가 촬영된 영상을 직접 편집하거나, 촬영 시 가리개를 이용하는 방법을 사용한다. 이러한 방법은 번거롭고, 실수로 인해 얼굴이나 유해물질이 방송에 그대로 노출될 수 있다. 본 논문에서는 딥러닝 기반의 객체탐지 모델과 동일인 판단 모델을 사용하여 편집 과정을 자동으로 처리하고 후처리 뿐만 아니라 실시간 방송에서의 적용을 위해 추가적으로 객체추적 알고리즘을 도입하여 처리속도를 높이는 방안을 제시한다.

#### 연구방법
유해물질 모자이크 처리방식은 카메라로 입력받은 영상을 객체탐지 딥러닝 모델 중 하나인 YOLO를 이용해 객체를 찾아내고 모자이크 처리를 하는 방식을 이용한다. 사람의 경우 객체탐지는 유해물질과 동일하게 수행되지만, 특정 인물을 모자이크에서 제외시키기 위해 동일인 판단 딥러닝 모델인 FaceNet을 이용하여 제외시킬 인물을 판단한다. 그러나 매 프레임마다 FaceNet 처리과정의 반복으로 인해 속도저하가 발생했고 이를 해결하기 위해 객체추적 알고리즘인 MeanShift를 사용하여 FaceNet이 발생시키는 병목현상을 완화시켰다. 이러한 방법을 활용한다면 실시간 방송뿐만 아니라, 촬영 후 편집 과정에서 사람이 놓칠 수 있는 부분을 잡아낼 수 있으며, 영상편집 작업에 소요되는 시간을 단축할 수 있다.

#### 정리
* 실시간으로 특정 객체에 이미지 처리를 수행하는 시스템 입니다.   
* 촬영 후 편집으로 수행했던 작업들을 실시간으로 처리할 수 있으므로 실시간 미디어 매체에 활용이 가능합니다.   
* 임베디드 보드(Jetson tx1 board)에서 동작할 수 있습니다.   
* YOLO와 FaceNet을 활용하여 구현하였습니다.

***

### 특정 인물을 제외한 모든 인물 모자이크

<p align="center">
<img src="./image/option_1_test.gif" alt="option_1"/>
</p>

***

### 사물 모자이크

<p align="center">
<img src="./image/option_2_test.gif" alt="option_2"/>
</p>

***

### 임베디드 보드 테스트 (Jetson TX1 board)

<p align="center">
  <img src="./image/JTX1_devkit.png" alt="JTX1_devkit" style="width:500px;"/>
<img src="./image/board_test_image.png" alt="board_test_image" style="width:500px;"/>
<img src="./image/board_test.gif" alt="board_test"/>
</p>

## 실행

#### 사물 모자이크
```
# step 1
./cap.bash object
```

#### 특정 인물 제외 모든 인물 모자이크
```
# step 1. 특정 인물 얼굴 캡쳐
./cap.bash crop

# step 2. FaceNet 학습
./cap.bash train

# step 3. 실행
./cap.bash face
```


## 세부 구현 내용
#### YOLO-FaceNet 통신

<p align="center">
<img src="./image/shared_memory.png" alt="model_connection" style="width:500px;"/>
</p>

YOLO는 `C`, FaceNet은 `Python`으로 구현되어 있으므로 입출력값을 공유하기 위한 공유 메모리를 사용합니다.

#### MeanShift

병목 현상 방지를 위해 30fps 중 1fps는 FaceNet, 29fps는 MeanShift 알고리즘을 사용하였습니다.

#### 임베디드 환경

YOLO(Tiny Yolo)는 임베디드 보드로 수행, 30fps 마다 PC와 Socket 통신하여 연산을 요청, FaceNet의 결과를 제공받습니다.


## 개발 환경
#### OS   
* Linux   
#### 개발 언어
* C   
* Python   
#### GPU   
|PC|TX1|
|---|---|
|NVIDIA CUDA® Cores-4352|256-core NVIDIA Maxwell™|
   
## Reference
#### Site
* [YOLOv3](https://pjreddie.com/darknet/yolo/)
#### Paper
* [YOLOv3](https://arxiv.org/pdf/1804.02767.pdf)
* [FaceNet](https://www.cv-foundation.org/openaccess/content_cvpr_2015/papers/Schroff_FaceNet_A_Unified_2015_CVPR_paper.pdf)
#### Code
* https://github.com/davidsandberg/facenet
* https://github.com/shanren7/real_time_face_recognition

