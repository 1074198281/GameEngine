# GameEngine

[TOC]

GameEngine代码主要参考了https://zhuanlan.zhihu.com/p/510064704专栏内容。

该项目将主要用于学习渲染理论和光照相关理论。其中的物理引擎和GUI部分暂时不作深入考虑。

该项目主体由**dx12**支持，且暂时**没有**考虑跨平台和vulkan支持，所以主要在windows上使用，IDE使用VS2022。

## 0 项目构建

项目中目前使用的三方库包括bullet3,ImGUI,OGEX。所有的三方库文件都存储在External/Windows/include下。

编译得到的.lib文件存储在External/Windows/lib下。

### 0.1 bullet3

在bullet3文件夹下创建build文件夹，并在其中执行cmake。在项目目录下打开cmd/powershell。

`cd External/Windows/include/bullet3`
`mkdir build`
`cd build`
`cmake ..`
`./MY_BULLET_PHYSICS.sln`

编译即可。

### 0.2 ImGUI

在ImGUI下创建build文件夹，并在其中执行cmake。

`cd External/Windows/include/ImGUI`
`mkdir build`
`cd build`
`cmake ..`
`./ImGUI.sln`

编译即可。

### 0.3 OGEX

OGEX的lib文件直接存放在lib文件夹下，可以直接使用。由于对应的cpp文件丢失，编译方式需要参考知乎文档中的内容。

### 0.4 项目构建

在项目根目录下执行：

`mkdir build`
`cd build`
`cmake ..`
`./GameEngine.sln`

即可打开项目工程。

## 1 文件结构

### 1.1 Asset

用于存放所有的资源文件，包括模型、贴图以及Shader文件。

#### 1.1.1 Scene

场景资源文件。是主要的资源文件存放处，目前使用的模型包括gltf和ogex。

#### 1.1.2 Shaders

shader文件。是主要的shader文件和编译输出的二进制文件存放处。主要保存hlsl文件，glsl文件暂时没有支持。

### 1.2 build

build文件夹用于整体构建工程，在build文件夹中使用指令  **cmake ../**  即可进行构建。由于开发环境主要在windows，所以主要使用VS2022。

### 1.3 build_shader

build_shader文件夹主要用于构建shader的编译工程。在该目录下使用指令  **cmake ../Asset/Shaders/hlsl**  即可将hlsl文件夹下的hlsl文件添加进入用于编译shader的工程。在hlsl文件夹下编写的CMakeLists.txt文件中已经为所有收集到的hlsl文件设置了文件属性，可以将其编译为CompiledShaders文件夹下的.h文件并直接在GameEngine中包含头文件就可以使用。
注意，对应的顶点着色器文件后缀应为  **_VS.hlsl**  ，像素着色器文件应为  **_PS.hlsl**  ，这样可以在cmake文件中识别。

### 1.4 External

External文件夹主要存放了三方库的头文件和编译的lib与bin文件。

### 1.5 Framework

Framework主要存放了引擎框架上的一些文件。

#### 1.5.1 Algorithm

Algorithm主要保存了算法相关的一些文件。

#### 1.5.2 Common

Common文件夹主要存放了一些通用结构、类和一些常用的工具函数的声明、实现。

#### 1.5.3 GeomMath

GeomMath主要存放了数学库相关的文件。

#### 1.5.4 Interface

Interface主要存放了父类接口的声明。通常这些为纯虚类。

#### 1.5.5 Manager

Manager主要存放了管理器父类的一些接口。

#### 1.5.6 Parser

Parser主要是存放了一些解释器相关的文件。

#### 1.5.7 SceneGraph

SceneGraph主要保存了场景相关的一些文件。

### 1.6 Platform

 Platform主要存放了平台相关的接口文件。但是由于本学习用例只打算运行在windows上，所以未考虑其他平台的代码结构。

### 1.7 RHI

用于存放了RHI相关的接口。在原参考文中，给出了d3d,d2d,opengl等图形接口，这里只着重考虑d3d的使用。

