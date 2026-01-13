# 包含交叉编译工具链
include(cross.cmake)

# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread -Wall -fopenmp")  # 对于 C++ 编译器
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -fopenmp")      # 对于 C 编译器

# 获取当前 CMakeLists.txt 文件所在目录的完整路径
set(CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})

# 获取上上级目录的完整路径
get_filename_component(PARENT_PARENT_DIR ${CURRENT_DIR}/.. ABSOLUTE)

# 从完整路径中提取文件夹名称
get_filename_component(PARENT_PARENT_FOLDER_NAME ${PARENT_PARENT_DIR} NAME)

# 打印上上级文件夹名称
message("Parent parent folder name: ${PARENT_PARENT_FOLDER_NAME}")

# 定义项目名称和版本，并指定使用C和C++语言
project(${PARENT_PARENT_FOLDER_NAME} VERSION 0.1.0 LANGUAGES C CXX)

# 设置OpenCV的安装路径
set(CMAKE_PREFIX_PATH "../../cross_lib/opencv")

# 查找OpenCV库，确保安装了所需的依赖
find_package(OpenCV REQUIRED)

# 包含OpenCV的头文件路径
include_directories(${OpenCV_INCLUDE_DIRS})
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")

# 设置NCNN的安装路径
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "../../cross_lib/ncnn")

# 设置NCNN的头文件路径和库路径
set(NCNN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../cross_lib/ncnn/include/ncnn")
set(NCNN_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/../../cross_lib/ncnn/lib/libncnn.a")

# 包含NCNN的头文件路径
include_directories(${NCNN_INCLUDE_DIR})
message(STATUS "NCNN_INCLUDE_DIR: ${NCNN_INCLUDE_DIR}")
message(STATUS "NCNN_LIBRARY: ${NCNN_LIBRARY}")

# 设置JsonCpp的头文件路径和库路径
set(JSONCPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../cross_lib/jsoncpp/include")
set(JSONCPP_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/../../cross_lib/jsoncpp/lib/libjsoncpp.so")

# 包含JsonCpp的头文件路径
include_directories(${JSONCPP_INCLUDE_DIR})
message(STATUS "JSONCPP_INCLUDE_DIR: ${JSONCPP_INCLUDE_DIR}")
message(STATUS "JSONCPP_LIBRARY: ${JSONCPP_LIBRARY}")

# 包含项目的自定义库路径
include_directories(../user)
include_directories(../code)
#================= 可以添加自己文件夹的路径便于归类整理 =================
# 示例
# include_directories(../code/CAM)
# include_directories(../code/INFERENCE)
#================= 可以添加自己文件夹的路径便于归类整理 =================
include_directories(../../libraries/zf_common)
include_directories(../../libraries/zf_device)
include_directories(../../libraries/zf_driver)
include_directories(../../libraries/zf_components)
include_directories(../../cross_lib/wuwu)

# 从目录下查找C/C++文件
aux_source_directory(../user DIR_SRCS)
aux_source_directory(../code DIR_SRCS)
#================= 可以添加自己文件夹的路径便于归类整理 =================
# 示例
# aux_source_directory(../code/CAM DIR_SRCS)
# aux_source_directory(../code/INFERENCE DIR_SRCS)
#================= 可以添加自己文件夹的路径便于归类整理 =================
aux_source_directory(../../libraries/zf_common DIR_SRCS)
aux_source_directory(../../libraries/zf_device DIR_SRCS)
aux_source_directory(../../libraries/zf_driver DIR_SRCS)
aux_source_directory(../../libraries/zf_components DIR_SRCS)
aux_source_directory(../../cross_lib/wuwu DIR_SRCS)

# 创建可执行文件，使用项目名称作为目标名称
add_executable(${PROJECT_NAME} ${DIR_SRCS})

# 预编译头文件 避免重复编译
target_precompile_headers(${PROJECT_NAME} PRIVATE
    ../../libraries/zf_common/zf_common_headfile.hpp
)

# 链接OpenCV、NCNN和JsonCpp库到可执行文件
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}          # OpenCV_LIBS
    ${NCNN_LIBRARY}         # NCNN_LIBRARY
    "${JSONCPP_LIBRARY}"    # JSONCPP_LIBRARY
    gomp  # OpenMP库
)