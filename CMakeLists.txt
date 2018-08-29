cmake_minimum_required(VERSION 3.0)
project(SeRoNetSDK)

set(CMAKE_CXX_STANDARD 14)
enable_testing()
set(ENABLE_CODECOVERAGE OFF CACHE BOOL "Enables the codecoverage")

find_package(open62541 REQUIRED)
find_package(Open62541Cpp REQUIRED)
find_package(SmartSoft_CD_API REQUIRED)

if ( ENABLE_CODECOVERAGE )

	if ( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
		message( WARNING "Code coverage results with an optimised (non-Debug) build may be misleading" )
	endif ( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )

	if ( NOT DEFINED CODECOV_OUTPUTFILE )
		set( CODECOV_OUTPUTFILE cmake_coverage.output )
	endif ( NOT DEFINED CODECOV_OUTPUTFILE )

	if ( NOT DEFINED CODECOV_HTMLOUTPUTDIR )
		set( CODECOV_HTMLOUTPUTDIR coverage_results )
	endif ( NOT DEFINED CODECOV_HTMLOUTPUTDIR )

	if ( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCXX )
		find_program( CODECOV_GCOV gcov )
		if(NOT CODECOV_GCOV)
			message(FATAL_ERROR "CODECOV_GCOV not found!")
		endif()
		find_program( CODECOV_LCOV lcov )
		if(NOT CODECOV_LCOV)
			message(FATAL_ERROR "CODECOV_LCOV not found!")
		endif()
		find_program( CODECOV_GENHTML genhtml )
		add_definitions( -fprofile-arcs -ftest-coverage )
		link_libraries( gcov )
		set( CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} --coverage )
		add_custom_target( coverage_init ALL ${CODECOV_LCOV} --base-directory .  --directory ${CMAKE_BINARY_DIR} --output-file ${CODECOV_OUTPUTFILE} --capture --initial )
		add_custom_target( coverage ${CODECOV_LCOV} --base-directory .  --directory ${CMAKE_BINARY_DIR} --output-file ${CODECOV_OUTPUTFILE} --capture COMMAND genhtml -o ${CODECOV_HTMLOUTPUTDIR} ${CODECOV_OUTPUTFILE} )
	endif ( CMAKE_COMPILER_IS_GNUCXX )

endif (ENABLE_CODECOVERAGE )

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
