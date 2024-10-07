set(exe_name ${MAIN_PROJECT})

file(GLOB src_files_assets Assets/*)
file(GLOB src_files_vulkan Vulkan/*)
file(GLOB src_files_utils Utils/*)

set(src_files
    main.cpp
    Options.hpp
    Options.cpp
    TaskCoordinator.cpp
    TaskCoordinator.hpp
)

source_group("Asserts" FILES ${src_files_assets})
source_group("Vulkan" FILES ${src_files_vulkan})
source_group("Main" FILES ${src_files})
source_group("Utils" FILES ${src_files_utils})


add_executable(${exe_name}
    ${src_files_assets}
    ${src_files_vulkan}
    ${src_files_utils}
    ${src_files}
)

# add_dependencies()

set_target_properties(${exe_name} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_include_directories(${exe_name} PRIVATE . ${STB_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
target_include_directories(${exe_name} PRIVATE ${TINYGLTF_INCLUDE_DIRS})
target_include_directories(${exe_name} PRIVATE ${CPP_BASE64_INCLUDE_DIRS})

# 编译后命令
# add_custom_command(
#             TARGET ${exe_name} POST_BUILD
# 			COMMAND echo \"copy oidn dlls...\"
#             COMMAND xcopy \"${CMAKE_CURRENT_SOURCE_DIR}/../src/ThirdParty/oidn/bin/*.dll\" \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\" /Y /Q /F
#         )
# endif()

target_link_libraries(${exe_name} PRIVATE fmt::fmt CURL::libcurl Boost::boost Boost::exception Boost::program_options glm::glm imgui::imgui tinyobjloader::tinyobjloader draco::draco ${Vulkan_LIBRARIES} ${extra_libs})
target_link_libraries(${exe_name} PRIVATE glfw)

if(WIN32)
target_compile_definitions(${exe_name} PUBLIC VK_USE_PLATFORM_WIN32_KHR PLATFORM_WINDOWS)
endif()
