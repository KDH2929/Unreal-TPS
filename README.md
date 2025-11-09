# Unreal-TPS

- 제작인원 : 3인
- IK, AimOffset을 통한 Bone 회전의 경우 유튜브 영상을 참고하여 제작
- 툰 쉐이더의 경우, 아는 지인이 같이 스터디해보자고 하여 공부하게 되었습니다.

<br>

- [유튜브 영상 및 다운로드 링크](https://youtu.be/Q7WIUYH8JqI?si=F-wGc_7AaTSI5MV9)

<br>

---

## 참고자료
- [Unreal Fest 카툰렌더링](https://youtu.be/sXIAIOuGee4?si=txHLi6Ylrb8GgqSz)

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
- 배열 기반 인벤토리 및 UI 연동

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


### 1. **Raycast 기반 총알이 도달할 위치**

```latex
Camera Position + Forward Vector * WeaponRange  + R cosθ ⋅ Right Vector + R sinθ ⋅ Up Vector
```

<p align="left">
  <img src="https://github.com/user-attachments/assets/bc1c0261-4e10-4cec-a724-6bbc9153c22b" width="50%">
</p>

<br>

### 2. **절차적 지형메시 생성**

#### 2.1 **Vertices 생성**

- 정점 배열은 1차원 배열로 저장되며, Loop를 통해 생성
- 정점의 X좌표와 Y좌표는 아래 그림의 예시와 같이 설정 
- Z좌표는 PerlinNoise를 사용하여 랜덤하게 결정하여 지형의 높이 설정

<br>

<p align="left">
  <img src="https://github.com/user-attachments/assets/2a3c490a-7fe2-47d6-b050-cf702a187fad" width="40%">
</p>

<br>

#### 2.2 **Indices 생성**

- 언리얼엔진에서는 Mesh 생성 시 삼각형 인덱스정보를 반시계방향으로 입력 시 `FrontFace`로 판단하고, 시계방향으로 입력 시 `BackFace`로 판단
- 직사각형 메시를 만들기 위해 각 삼각형을 아래와 같이 구성
  
  - **아래쪽 삼각형**: `Vertex`, `Vertex + 1`, `Vertex + Ysize + 1`
  - **위쪽 삼각형**: `Vertex + 1`, `Vertex + Ysize + 2`, `Vertex + Ysize + 1`

<br>

#### 2.3 **인덱스 계산 과정**

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



