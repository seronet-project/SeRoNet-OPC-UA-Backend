cmake_minimum_required(VERSION 3.0)
project(SeRoNetSDK)

set(CMAKE_CXX_STANDARD 17)


find_package(open62541 REQUIRED)
find_package(Open62541Cpp REQUIRED)


set(PRJ_FILES
		SeRoNetSDK/SeRoNet/OPCUA/Client/IBlocking.cpp
		SeRoNetSDK/SeRoNet/CommunicationObjects/Description/ComplexType.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Client/Converter/CommObjectToUaVariantArray.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Client/Converter/UaVariantArrayToCommObject.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/CommObjectToUaArgument.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Server/CommObjectToPushModell.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Server/PushServerUpdater.cpp
		SeRoNetSDK/SeRoNet/Utils/Component.cpp
		)

add_library(SeRoNetSDK ${PRJ_FILES})
set_target_properties(SeRoNetSDK PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(SeRoNetSDK PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(SeRoNetSDK Open62541Cpp open62541)


include(GenerateExportHeader)
set(SERONET_SDK_VERSION 0.1.0)

set_property(TARGET SeRoNetSDK PROPERTY VERSION ${OPEN62541_CPP_VERSION})
set_property(TARGET SeRoNetSDK PROPERTY SOVERSION 3)

# export library (either static or shared depending on BUILD_SHARED_LIBS)
install(TARGETS SeRoNetSDK
		EXPORT SeRoNetSDKTargets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include/SeRoNetSDK
		)

install(DIRECTORY SeRoNetSDK/ DESTINATION include/SeRoNetSDK
		FILES_MATCHING PATTERN *.hpp)

install(DIRECTORY SmartSoftComponentDeveloperAPIcpp/ DESTINATION include/SmartSoftComponentDeveloperAPIcpp/
		FILES_MATCHING PATTERN *.h)


set(cmake_configfile_install lib/cmake)
set(target_install_dest_name "${cmake_configfile_install}/SeRoNetSDKTargets.cmake")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/SeRoNetSDKConfigVersion.cmake"
		VERSION ${SERONET_SDK_VERSION}
		COMPATIBILITY AnyNewerVersion
)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/SeRoNetSDKConfig.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/SeRoNetSDKConfig.cmake"
		INSTALL_DESTINATION "${cmake_configfile_install}"
		PATH_VARS target_install_dest_name)

install(EXPORT SeRoNetSDKTargets
		FILE SeRoNetSDKTargets.cmake
		DESTINATION "${cmake_configfile_install}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/SeRoNetSDKConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/SeRoNetSDKConfigVersion.cmake"
		DESTINATION "${cmake_configfile_install}")


# Modify default MSVC options of CMake
if (MSVC)
	set(CompilerFlags
			CMAKE_CXX_FLAGS
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_RELEASE
			CMAKE_C_FLAGS
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_RELEASE
			)
	foreach (CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach ()
endif (MSVC)
