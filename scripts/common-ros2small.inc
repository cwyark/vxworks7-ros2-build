# common-ros2small.inc ROS 2 project specific include file for the shell scripts
# contains a minimal set of ROS 2 libraries to build rclcpp examples
#
# Copyright (c) 2019 Wind River Systems, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Modification history
# --------------------
# 19apr19,akh  created


PRJ_GIT_REPO=${PRJ_GIT_REPO:-"http://stash.wrs.com/scm/to/vxworks7-layer-for-ros2.git"}
PRJ_GIT_BRANCH=${PRJ_GIT_BRANCH:-"dashing-release-SR0620"}
PRJ_GIT_DIR="middleware" # a directory within ${WIND_PKGS_DIR_NAME}

# ROS2 dashing release
PRJ_COMMIT_ID=${PRJ_COMMIT_ID:-dashing-release}

# define VSB layers and put them in a dependency order
VSB_LAYERS="BZIP2 USER_MANAGEMENT EXPAT"
PRJ_VSB_LAYERS="ASIO TINYXML2"

# define VIP components
VIP_PROFILE="PROFILE_DEVELOPMENT"

# POCO is not properly ported to vxWorks-7, do not include POCO and poco_vendor layers
PRJ_VIP_COMPONENTS="INCLUDE_POSIX_PIPES INCLUDE_POSIX_PTHREAD_SCHEDULER INCLUDE_POSIX_SEM INCLUDE_RTP INCLUDE_SHL INCLUDE_GETNAMEINFO_SYSCTL"
VIP_ATA_COMPONENTS="DRV_STORAGE_ATA_HOST PCI_DRV_STORAGE_AHCI"
VIP_COMPONENTS_ADD="${VIP_COMPONENTS_ADD} ${PRJ_VIP_COMPONENTS}"

PRJ_PKGS_DIRS=( \
	src/eProsima/Fast-CDR \
	src/eProsima/Fast-RTPS \
	src/ros/class_loader \
	src/ros2/console_bridge_vendor \
	src/ros2/demos \
	src/ros2/examples \
	src/ros2/geometry2 \
	src/ros2/rcl \
	src/ros2/rclcpp \
	src/ros2/rcutils \
	src/ros2/realtime_support \
	src/ros2/rmw_implementation \
	src/ros2/rosidl \
	src/ros2/rosidl_defaults \
	src/ros2/rosidl_typesupport \
	src/ros2/tinydir_vendor \
)

PRJ_PKGS=( \
	Fast-CDR \
	Fast-RTPS \
	class_loader \
	console_bridge_vendor \
	demos \
	examples \
	geometry2 \
	rcl \
	rclcpp \
	rcutils \
	realtime_support \
	rmw_implementation \
	rosidl \
	rosidl_defaults \
	rosidl_typesupport \
	tinydir_vendor \
)

IGNORED_PKGS=" \
	src/ros-visualization \
	src/ros2/rviz \
	src/ros2/poco_vendor \
	src/osrf/osrf_testing_tools_cpp \
	src/ros2/orocos_kinematics_dynamics \
	src/ros-planning \
	src/ros2/rmw_connext \
	src/ros2/rosidl_typesupport_connext \
	src/ros2/rmw_opensplice \
	src/ros2/rosidl_typesupport_opensplice \
	src/ament/ament_lint \
	src/ament/uncrustify_vendor \
	src/ros2/rcl_logging/rcl_logging_log4cxx \
	src/eclipse-cyclonedds \
"

# Ignore Python-specific packages
IGNORED_PKGS=" ${IGNORED_PKGS} \
	src/ament/ament_cmake/ament_cmake_pytest \
	src/osrf/osrf_pycommon \
	src/ros2/demos/demo_nodes_py \
	src/ros2/geometry2/tf2_py \
	src/ros2/kdl_parser/kdl_parser_py \
	src/ros2/rclpy \
	src/ros2/rosidl_python/rosidl_generator_py \
	src/ros-visualization/rqt/rqt_gui_py \
"

ros2small_download()
{
	# retrieve ROS2 repos
	wget --no-check-certificate https://raw.githubusercontent.com/ros2/ros2/${PRJ_COMMIT_ID}/ros2.repos

	mkdir src
	vcs import src < ros2.repos
}

ros2small_patch()
{
	prj_patch
}


ros2small_configure()
{
        # create vxworks_env.sh and autoconf
        ADDED_LIBS="-L${VSB_DIR}/usr/lib/common -lnet -lunix -lregex" \
        PRE_INCLUDE=-isystem${VSB_DIR}/usr/h/published/UTILS_UNIX \
        EXTRA_DEFINE=-Wno-narrowing \
        CPU=${CPU} TOOL_VERSION=llvm_2016_04 TOOL=llvm make -f $WIND_USR_MK/autoconf.mk vxworks_env.sh

        CPU=${CPU} TOOL_VERSION=llvm_2016_04 TOOL=llvm make -f $WIND_USR_MK/autoconf.mk `pwd`/autoconf_vsbConfig_quote.h

        # figure out why we need to patch for the libyaml.so
        # -L${PRJ_WS}/install/libyaml_vendor/lib -lyaml" \

	prj_configure
}


ros2small_build()
{
	ROS2EXAMPLES="examples_rclcpp_minimal_action_client \
			examples_rclcpp_minimal_client \
			examples_rclcpp_minimal_publisher \
			examples_rclcpp_minimal_subscriber \
			examples_rclcpp_minimal_action_server \
			examples_rclcpp_minimal_composition \
			examples_rclcpp_minimal_service \
			examples_rclcpp_minimal_timer"

	. ./vxworks_env.sh && colcon build --symlink-install --cmake-force-configure --packages-up-to $ROS2EXAMPLES pendulum_control --cmake-args -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_TOOLCHAIN_FILE=$VSB_DIR/buildspecs/cmake/rtp.cmake -DCMAKE_PREFIX_PATH=$PRJ_WS/install\;$VSB_DIR/usr/root -DBUILD_TESTING=OFF
}
