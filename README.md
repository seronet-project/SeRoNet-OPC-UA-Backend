# SeRoNetSDK 

The OPC UA SDK is used in the SeRoNet Projct as communication backend in the service robotic. More Information about the project can be found here:
[SeRoNet Website] (https://www.seronet-projekt.de/). The SeRoNet tooling offers the best user experience.

## Examples

There is a seperate Repos for the examples:[here](https://github.com/seronet-project/SeRoNetSDK_Tests)


## Dependencies

The current implementation is based on ~~a fork of the  [open62541](https://github.com/seronet-project/open62541)
 project~~ the open62541 project (0.4 or higher) . The [SmartSoft Component-Developer API](https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp) is another dependency, which however is a git submodule.
 For building the library, CMake (at least version 3.8) is required.

## Installation
For building the examples the following packages need to be installed on your system:

- **CMake** (at least version 3.8)
- **open62541** the (https://github.com/open62541/open62541) (clone, build and install the library)
- **open62541AbstractionLayer** the [master](https://github.com/seronet-project/open62541AbstractionLayer) (clone, 
build and 
install the 
library
- **SmartSoft Component-Developer API**  [SmartSoft Component-Developer API](https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp) (build and install the library)


### Installation open62541
It is important to install and build the Mulicast-Discovery (clone recursive and cmake flag DUA_ENABLE_DISCOVERY_MULTICAST:BOOL="1"). Dependencies for open62641 is the python module six (````pip install six````)

An example for the installation can be found here. Some Commands and Parameter can be depend on your system. 
````
> git clone https://github.com/open62541/open62541.git --recursive
> pushd open62541
> mkdir cmake-build-debug
> cd cmake-build-debug
> cmake -DUA_ENABLE_AMALGAMATION:BOOL="1" -DUA_ENABLE_DISCOVERY_MULTICAST:BOOL="1" ..
> cmake --build . --target install -- -j 2
> popd
````

### Installation open62541AbstractionLayer
see [Readme.md](https://github.com/seronet-project/open62541AbstractionLayer/blob/master/README.md) of the Project. Please note that the op62541 projcet may already have been installed.

### Installation SmartSoft Component-Developer API
 see [SmartSoft Component-Developer API](https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp) 


### Build and Installation SeRoNetSDK
This is an example. Some of the instuctions depends on your system. Please note that you have to set the SmartSoft_CD_API_DIR. Its the path to the install directory of the SmartSoft API installed one step before.

````
> git clone --recursive https://github.com/seronet-project/SeRoNetSDK.git
> pushd SeRoNetSDK
> mkdir cmake-build-debug
> cd cmake-build-debug
> cmake -DSmartSoft_CD_API_DIR:PATH="PATH/TO/YOUR/INSTALLATION/modules"  ..
> cmake --build . --target install -- -j 2
> popd
````

## Status and Current Shortcuts

The library is in a first draft version and includes some hardcoded shortcuts as explained below.

- The current implementation is in no way optimized towards performance. The focus is on clean structures rather than efficiency. However, this implementation already minimizes the number of required threads. Basically, at the moment, each component requires exactly one internal thread that manages all events from all potential server ports that need to provide (internal management) feedback to client ports. Yet, execution times and the amount of required memory can still be considerably improved.

