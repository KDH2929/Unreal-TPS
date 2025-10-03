# Unreal-TPS



**개발 기간** : 2달<br>
**팀 구성** : 3인팀

---

## 프로젝트 목표
- [Unreal Fest 카툰렌더링](https://youtu.be/sXIAIOuGee4?si=txHLi6Ylrb8GgqSz) 영상을 따라서<br>
  팀원들과 함께 **Toon Shader**를 공부하는 것을 목표로 하였습니다.

<br>

---
[유튜브 영상 및 다운로드 링크](https://youtu.be/Q7WIUYH8JqI?si=F-wGc_7AaTSI5MV9)


---

<br>

## 추가 구현 요소
- 팀 프로젝트에서 만든 Toon Shader를 기반으로, 개인적으로 유튜브 강의들을 참고하여 아래 기능들을 구현해 보았습니다.
- 유튜브 강의는 블루프린트로 보통 되어있기에 C++로 옮기는 작업을 하였었습니다.

  <br>

- **TPS 구현**  
  - 참고 영상: [How To Create A Weapons System (2.0) - Unreal Engine 5 Tutorial](https://www.youtube.com/watch?v=H_Q57Yso9mM&list=PLY2663dNRL_iCP0xsW6xw0Ph94ApT7JrR)

- **지형 렌더링**  
  - 참고 영상: [Terrain generation for Beginners - Unreal Engine tutorial](https://www.youtube.com/watch?v=9ASgfwzNvL8&list=PLyL5ZNukfVqskz_OkMdrLamiYg1sITyic)




<br>


<br>

---

## 구현 목록

### 1. **캐릭터**  
- **FABRIK을 통한 총기 잡기**  
  - IK(Inverse Kinematics)를 사용하여 캐릭터가 자연스럽게 총기를 잡도록 처리  

- **AimOffset을 통한 상체 회전**  
  - 마우스 Pitch 값에 따른 Spine Bone 회전  

- **카메라 제어**  
  - 1인칭 / 3인칭 카메라 전환  
  - 카메라의 확대/축소 기능  

- **캐릭터 애니메이션**  
  - IK Retargeter를 통한 애니메이션 리타겟팅  
  - 애니메이션 FSM(Finite State Machine)  

<br>

---

### 2. **총기류**  
- **Raycast 총기 구현**  
  - Pistol, Shotgun, Rifle, SniperRifle 구현  

- **총기 반동**  
  - 지정된 범위의 랜덤한 반동 수치 생성  
  - Bone의 Location, Rotation 조절 및 보간 함수 적용  

- **동적 조준원**  
  - 극좌표계 수식을 기반으로 하여 총알이 정해진 조준원의 범위에 랜덤하게 도달하게 함  
  - 캐릭터의 움직임에 따라 조준원의 크기를 동적으로 조절  

<br>

---

### 3. **인벤토리**  
- **배열과 HashMap 활용**
  
  - Key: 아이템 이름  
  - Value: 수량 및 UI 텍스처 정보 저장  

<br>

---

### 4. **지형**  
- **절차적 지형 생성**  
  - Perlin Noise 기반의 Vertices & Indices 정의  
  - Loop를 활용하여 메시 생성  

- **무한맵 구현**  
  - 캐릭터 주위의 공간을 Chunk 단위로 구분  
  - 캐릭터 주위에는 항상 지형 메시가 생성되도록 함  
  - HashSet으로 중복 생성 방지  

<br>

---

### 5. **몬스터**  
- **이동 및 행동 제어**  
  - `NavMesh`와 `Behavior Tree`를 사용해 경로 탐색 및 AI 동작 설정  
  - `Blackboard`를 통한 몬스터 상태 관리  

- **플레이어 감지**  
  - `PawnSensing Component`를 이용해 일정 거리 내 플레이어 감지  

- **공격 및 충돌 판정**  
  - `Capsule Collider`를 사용해 플레이어와 충돌 판정  
  - 공격 범위 내 플레이어가 있을 경우 애니메이션 실행 및 데미지 적용  



<br>


## 구현 상세
### 1. **총기류 클래스 구조도**
<p align="left">
  <img src="https://github.com/user-attachments/assets/ba820803-9eac-4ced-993c-9fefe5ecd33f" width="50%">
</p>

- 핵심 기능은 C++로 구현  
- Skeletal Mesh 경로 설정, 총기류 분류, 총기별 메타데이터 설정은 Blueprint에서 수행하여 구현 시간을 단축시키고자 함
- 몬스터, 아이템 클래스 역시 위와 비슷한 구조로 설계됨

<br>

### 2. **Raycast 기반 총알이 도달할 위치 계산**

```latex
Camera Position + Forward Vector * WeaponRange  + R cosθ ⋅ Right Vector + R sinθ ⋅ Up Vector
```

<p align="left">
  <img src="https://github.com/user-attachments/assets/bc1c0261-4e10-4cec-a724-6bbc9153c22b" width="50%">
</p>

<br>

### 3. **절차적 지형메시 생성**

#### 3.1 **Vertices 생성**

- 정점 배열은 1차원 배열로 저장되며, Loop를 통해 생성
- 정점의 X좌표와 Y좌표는 아래 그림의 예시와 같이 설정 
- Z좌표는 PerlinNoise를 사용하여 랜덤하게 결정하여 지형의 높이 설정

<br>

<p align="left">
  <img src="https://github.com/user-attachments/assets/2a3c490a-7fe2-47d6-b050-cf702a187fad" width="40%">
</p>

<br>

#### 3.2 **Indices 생성**

- 언리얼엔진에서는 Mesh 생성 시 삼각형 인덱스정보를 반시계방향으로 입력 시 `FrontFace`로 판단하고, 시계방향으로 입력 시 `BackFace`로 판단
- 직사각형 메시를 만들기 위해 각 삼각형을 아래와 같이 구성
  
  - **아래쪽 삼각형**: `Vertex`, `Vertex + 1`, `Vertex + Ysize + 1`
  - **위쪽 삼각형**: `Vertex + 1`, `Vertex + Ysize + 2`, `Vertex + Ysize + 1`

<br>

#### 3.3 **인덱스 계산 과정**

<p align="left">
  <img src="https://github.com/user-attachments/assets/875a4e18-5b03-401a-b384-219a296130a0" width="38%">
</p>

- **Ysize** = 가로 길이 = `4` (언리얼 로컬 좌표계 Y축)  
- **Xsize** = 세로 길이 = `3` (언리얼 로컬 좌표계 X축)  


1. 정점 배열에서  
   - `1번`, `2번`, `5번` → **아래쪽 삼각형**  
   - `2번`, `6번`, `5번` → **위쪽 삼각형**  

2. 결과적으로 `1번, 2번, 5번, 6번` 정점으로 직사각형 메시 생성  

3. 일반화된 수식:
   - **아래쪽 삼각형**: `Vertex`, `Vertex + 1`, `Vertex + Ysize + 1`  
   - **위쪽 삼각형**: `Vertex + 1`, `Vertex + Ysize + 2`, `Vertex + Ysize + 1`  

4. `Loop`를 통해 위 수식을 반복 적용하여 전체 지형 메시 생성



