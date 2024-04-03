# graphics_tutorial


# 1
이 패키지는 Visual Studio Code에서 C++ 언어를 기반으로 하였으므로

1. Visual Studio Code 설치
2. Visual Studio Code 확장 - 에서 C/C++, C/C++ Extension Pack의 설치가 필요함
   
![확장팩 설치](https://github.com/crator99/graphics_tutorial/blob/main/screenshots/extension.png)

# 2

C++ 파일의 수행을 위해 G++, GCC, GDB 등의 설치

<https://code.visualstudio.com/docs/cpp/config-mingw#_create-hello-world> 에서 참고 후 환경 변수에서 PATH 설정 필수

# 3
패키지의 실행을 위해 ".vscode/tasks.json"의 수정 필요

![tasks.json](https://github.com/crator99/graphics_tutorial/blob/main/screenshots/task_modify.png)

위 항목에서

ray_intersection.cpp을 실행하고 싶다면 16번째 줄 주석 제거

shading.cpp을 실행하고 싶다면 17번째 줄 주석 제거

antialiasing.cpp을 실행하고 싶다면 18번째 줄 주석 제거 필수

# 4
![ray_intersection](https://github.com/crator99/graphics_tutorial/blob/main/screenshots/ray_intersection.png)

ray_intersection의 결과 화면

![shading](https://github.com/crator99/graphics_tutorial/blob/main/screenshots/shading.png)

shading의 결과 화면

![anti_aliasing](https://github.com/crator99/graphics_tutorial/blob/main/screenshots/anti_aliasing.png)

antialiasing의 결과 화면
