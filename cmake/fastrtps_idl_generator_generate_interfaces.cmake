# Copyright 2014-2015 Open Source Robotics Foundation, Inc.
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


# avoid generating any fastrtps specific stuff for builtin_interfaces
if(NOT PROJECT_NAME STREQUAL "builtin_interfaces")

set(_output_subfolder "fastrtps_idl")

rosidl_generate_dds_interfaces(
  ${rosidl_generate_interfaces_TARGET}__dds_fastrtps_idl
  IDL_FILES ${rosidl_generate_interfaces_IDL_FILES}
  DEPENDENCY_PACKAGE_NAMES ${rosidl_generate_interfaces_DEPENDENCY_PACKAGE_NAMES}
  OUTPUT_SUBFOLDERS ${_output_subfolder}
)

set(_dds_idl_files "")
set(_dds_idl_base_path "${CMAKE_CURRENT_BINARY_DIR}/rosidl_generator_dds_idl")

foreach(_idl_file ${rosidl_generate_interfaces_IDL_FILES})
  get_filename_component(_extension "${_idl_file}" EXT)
  get_filename_component(_name "${_idl_file}" NAME_WE)

  if(_extension STREQUAL ".msg")
    get_filename_component(_parent_folder "${_idl_file}" DIRECTORY)
    get_filename_component(_parent_folder "${_parent_folder}" NAME)
    list(APPEND _dds_idl_files
      "${_dds_idl_base_path}/${PROJECT_NAME}/${_parent_folder}/${_output_subfolder}/${_name}_.idl")
  elseif(_extension STREQUAL ".srv")
    list(APPEND _dds_idl_files
      "${_dds_idl_base_path}/${PROJECT_NAME}/srv/${_output_subfolder}/Sample_${_name}_Request_.idl")
    list(APPEND _dds_idl_files
      "${_dds_idl_base_path}/${PROJECT_NAME}/srv/${_output_subfolder}/Sample_${_name}_Response_.idl")
  endif()
endforeach()

add_custom_target(${rosidl_generate_interfaces_TARGET}__dds_fastrtps_idl_launcher ALL
    DEPENDS ${target_dependencies} ${_dds_idl_files}
)

add_dependencies(
  ${rosidl_generate_interfaces_TARGET}__dds_fastrtps_idl
  ${rosidl_generate_interfaces_TARGET}__dds_fastrtps_idl_launcher
)

else()

  # generate specific type support code for the builtin_interfaces package
  set(_generated_msg_files "")
  set(_generated_external_msg_files)
  set(_generated_srv_files)
  set(_generated_external_srv_files)

endif()
