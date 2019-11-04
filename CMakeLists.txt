cmake_minimum_required(VERSION 3.0)
project(SeRoNetSDK)

set(CMAKE_CXX_STANDARD 14)
enable_testing()

# Ensures that all DLLs and executeables are in the same directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(ENABLE_CODECOVERAGE OFF CACHE BOOL "Enables the codecoverage")

option(CMAKE_POSITION_INDEPENDENT_CODE "Compile with position independent code" ON )
mark_as_advanced(CMAKE_POSITION_INDEPENDENT_CODE)

option(BUILD_SHARED_LIBS "Build shared libraryies" ON)

find_package(open62541 REQUIRED)
find_package(Open62541Cpp REQUIRED)
find_package(SmartSoft_CD_API REQUIRED)

set(PRJ_FILES
        SeRoNetSDK/SeRoNet/CommunicationObjects/Description/ComplexType.cpp
        SeRoNetSDK/SeRoNet/DefaultCommObjects/ParameterResponse.cpp
        SeRoNetSDK/SeRoNet/DefaultCommObjects/ParameterRequest.cpp
        SeRoNetSDK/SeRoNet/DefaultCommObjects/WiringCommObject.cpp
        SeRoNetSDK/SeRoNet/DefaultCommObjects/Description/ParameterResponseDescription.cpp
        SeRoNetSDK/SeRoNet/DefaultCommObjects/Description/ParameterRequestDescription.cpp
        SeRoNetSDK/SeRoNet/DefaultCommObjects/Description/ParameterRequestIdlDescription.cpp
        SeRoNetSDK/SeRoNet/DefaultCommObjects/Description/ParameterResponseIdlDescription.cpp
        SeRoNetSDK/SeRoNet/DefaultCommObjects/Description/WiringCommObjectDescription.cpp

		SeRoNetSDK/SeRoNet/OPCUA/Client/IBlocking.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Converter/CommObjectToUaVariantArray.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Converter/UaVariantArrayToCommObject.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Converter/CommObjectToListOfPrimitivDescriptions.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Converter/CommObjectBrowseToNodeIds.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Client/NamingServiceOpcUa.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Client/ParameterMaster.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Client/AsyncAnswerMethodCommObjectRequest.cpp

        SeRoNetSDK/SeRoNet/OPCUA/Converter/CommObjectToUaArgument.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Converter/CommObjectToPushModel.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/PushServerUpdater.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Server/SendServerHandler.hpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/ParameterSlave.hpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/ParameterUpdateHandler.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Server/PushServerDisabler.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Server/PushServerEnabler.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Server/ParameterSlave.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Server/WiringSlave.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/OpcuaServer.cpp

        SeRoNetSDK/SeRoNet/Utils/Component.cpp
        SeRoNetSDK/SeRoNet/Utils/HsUlm/smartProcessingPattern.hpp
        SeRoNetSDK/SeRoNet/Utils/smartMessageQueue.hpp
        SeRoNetSDK/SeRoNet/Utils/Semaphore.hpp

        SeRoNetSDK/SeRoNet/State/CommObjsSelfDescription.cpp
        SeRoNetSDK/SeRoNet/State/StateChangeHandler.cpp
        SeRoNetSDK/SeRoNet/State/StateMaster.cpp
        SeRoNetSDK/SeRoNet/State/StateSlave.cpp
        SeRoNetSDK/SeRoNet/State/StateSlaveHandler.cpp
        SeRoNetSDK/SeRoNet/State/StateUpdateThread.cpp

        SeRoNetSDK/SeRoNet/OPCUA/Converter/CommObjArrayToTypeIndex.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Converter/CommObjArrayToValue.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Converter/UaVariantToCommObjArray.cpp
        )


add_library(SeRoNetSDK ${PRJ_FILES})
set_target_properties(SeRoNetSDK PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(SeRoNetSDK PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(SeRoNetSDK PUBLIC Open62541Cpp::Open62541Cpp SmartSoft_CD_API)
target_include_directories(SeRoNetSDK INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SeRoNetSDK>
        $<INSTALL_INTERFACE:include/SeRoNetSDK>)

if(BUILD_SHARED_LIBS)
	# Ensure dllImport
	target_compile_definitions(SeRoNetSDK PUBLIC SERONET_SDK_AS_SHARED_LIBRARY)

	# Ensure dllexport
	target_compile_definitions(SeRoNetSDK PRIVATE SERONETSDK_BUILD_SHARED)

	add_custom_command(TARGET SeRoNetSDK POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:open62541::open62541>
			$<TARGET_FILE_DIR:SeRoNetSDK>
			COMMENT "Copy open62541 shared library to bin"
			)
endif()


# Tests
option(SERONETSDK_ENABLE_TESTS "Enable Tests" OFF)

if(${SERONETSDK_ENABLE_TESTS})
	add_subdirectory(tests)
endif()

# Install
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

set(cmake_configfile_install lib/cmake/SeRoNetSDK)
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
		DESTINATION "${cmake_configfile_install}"
		NAMESPACE SeRoNetSDK::
		EXPORT_LINK_INTERFACE_LIBRARIES)

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
