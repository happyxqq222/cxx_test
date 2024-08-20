cmake_minimum_required(VERSION 3.28)
project(NodeEditor)


set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH D:\\tools\\OMDev\\tools\\msys\\ucrt64)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(NodeEditor main.cpp
        MainWidget.cpp
        MainWidget.h
        FlowCharView.cpp
        FlowCharView.h
        FlowChartScene.cpp
        FlowChartScene.h
        NodeScene.cpp
        NodeScene.h)

target_link_libraries(NodeEditor PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui)
