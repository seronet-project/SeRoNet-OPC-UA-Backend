cmake_minimum_required(VERSION 3.0)
project(SeRoNetSDK)

set(CMAKE_CXX_STANDARD 14)


find_package(open62541 REQUIRED)
find_package(Open62541Cpp REQUIRED)
find_package(SmartSoft_CD_API REQUIRED)

set(PRJ_FILES
        SeRoNetSDK/SeRoNet/CommunicationObjects/Description/ComplexType.cpp
        SeRoNetSDK/SeRoNet/CommunicationObjects/DefaultObjects/ParameterResponse.cpp
        SeRoNetSDK/SeRoNet/CommunicationObjects/DefaultObjects/ParameterRequest.cpp
        SeRoNetSDK/SeRoNet/CommunicationObjects/Description/DefaultObjects/ParameterRequestDescription.cpp
        SeRoNetSDK/SeRoNet/CommunicationObjects/Description/DefaultObjects/ParameterResponseDescription.cpp
        SeRoNetSDK/SeRoNet/CommunicationObjects/Description/DefaultObjects/ParameterRequestIdlDescription.cpp
        SeRoNetSDK/SeRoNet/CommunicationObjects/Description/DefaultObjects/ParameterResponseIdlDescription.cpp

		SeRoNetSDK/SeRoNet/OPCUA/Client/IBlocking.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Client/Converter/CommObjectToUaVariantArray.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Client/Converter/UaVariantArrayToCommObject.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Client/Converter/CommObjectToListOfPrimitivDescriptions.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Client/Converter/CommObjectBrowseToNodeIds.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Client/NamingServiceOpcUa.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Client/ParameterMaster.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Client/AsyncAnswerMethodCommObjectRequest.cpp

        SeRoNetSDK/SeRoNet/OPCUA/Server/CommObjectToUaArgument.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/CommObjectToPushModell.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/PushServerUpdater.cpp
		SeRoNetSDK/SeRoNet/OPCUA/Server/SendServerHandler.hpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/ParameterSlave.hpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/ParameterUpdateHandler.cpp
        SeRoNetSDK/SeRoNet/OPCUA/Server/ParameterSlave.cpp

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
        SeRoNetSDK/SeRoNet/OPCUA/Converter/VariantToCommObjArray.cpp
        )


add_library(SeRoNetSDK ${PRJ_FILES})
set_target_properties(SeRoNetSDK PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(SeRoNetSDK PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(SeRoNetSDK PUBLIC Open62541Cpp::Open62541Cpp SmartSoft_CD_API)
target_include_directories(SeRoNetSDK INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SeRoNetSDK>
        $<INSTALL_INTERFACE:include/SeRoNetSDK>)

# Options for GTest
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "")
set(INSTALL_GMOCK OFF CACHE BOOL "")
set(gtest_force_shared_crt ON CACHE BOOL "")
set(gtest_hide_internal_symbols ON CACHE BOOL "")
set(build_static_lib ON CACHE BOOL "")
add_subdirectory(googletest)

add_subdirectory(tests)

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
