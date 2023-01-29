cmake_minimum_required(VERSION 3.5)
project(examples_rclcpp_minimal_publisher)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

## lcq
find_package(sensor_msgs REQUIRED)
find_package(serial REQUIRED)
# find_package(tf2_ros REQUIRED)
# find_package(tf2_msgs REQUIRED)
# find_package(tf2_geometry REQUIRED)
## end

add_executable(publisher_lambda lambda.cpp)
ament_target_dependencies(publisher_lambda rclcpp std_msgs)

add_executable(publisher_member_function member_function.cpp CJY61.cpp)

## lcq 找到之后需要链接上
#ament_target_dependencies(publisher_member_function rclcpp std_msgs sensor_msgs tf2_ros tf2_msgs tf2_geometry)
ament_target_dependencies(publisher_member_function rclcpp std_msgs sensor_msgs serial)

add_executable(publisher_member_function_with_type_adapter member_function_with_type_adapter.cpp)
ament_target_dependencies(publisher_member_function_with_type_adapter rclcpp std_msgs)

add_executable(publisher_member_function_with_unique_network_flow_endpoints member_function_with_unique_network_flow_endpoints.cpp)
ament_target_dependencies(publisher_member_function_with_unique_network_flow_endpoints rclcpp std_msgs)

add_executable(publisher_wait_for_all_acked member_function_with_wait_for_all_acked.cpp)
ament_target_dependencies(publisher_wait_for_all_acked rclcpp std_msgs)

add_executable(publisher_not_composable not_composable.cpp)
ament_target_dependencies(publisher_not_composable rclcpp std_msgs)

install(TARGETS
  publisher_lambda
  publisher_member_function
  publisher_member_function_with_type_adapter
  publisher_member_function_with_unique_network_flow_endpoints
  publisher_wait_for_all_acked
  publisher_not_composable
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
