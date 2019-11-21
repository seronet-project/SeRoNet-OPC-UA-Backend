//===================================================================================
//
//  Copyright (C) 2019 Alex Lotz
//
//        lotz@hs-ulm.de
//
//        Servicerobotik Ulm
//        Christian Schlegel
//        Ulm University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This file is part of the SmartSoft Component-Developer C++ API.
//
//  Redistribution and use in source and binary forms, with or without modification,
//  are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGE.
//
//===================================================================================

#include "TestingEnvironmentOPCUA.h"

DEFINE_TESTING_ENVIRONMENT(SmartOPCUA::TestingEnvironment);

// OPCUA Serialization for the CommTrajectory object
#include "CommTestObjectsOPCUA/CommTrajectoryOPCUA.h"

// OPCUA component realization
#include <SeRoNet/Utils/Component.hpp>

// OPCUA pattern headers
#include <SeRoNet/OPCUA/Client/PushClient.hpp>
#include <SeRoNet/OPCUA/Server/PushServer.hpp>
#include <SeRoNet/OPCUA/Client/SendClient.hpp>
#include <SeRoNet/OPCUA/Server/SendServer.hpp>
#include <SeRoNet/OPCUA/Client/QueryClient.hpp>
#include <SeRoNet/OPCUA/Server/QueryServer.hpp>
#include <SeRoNet/OPCUA/Client/EventClient.hpp>
#include <SeRoNet/OPCUA/Server/EventServer.hpp>


namespace SmartOPCUA {

IComponentPtrType TestingEnvironment::createComponent(const std::string &name) {
	int argc = 0;
	char **argv = 0;
	return std::make_shared<SeRoNet::Utils::Component>(name, argc, argv);
}

IPushClientPtrType TestingEnvironment::createPushClient()
{
	auto smart_component = std::dynamic_pointer_cast<SeRoNet::Utils::Component>(component);
	return IPushClientPtrType(new SeRoNet::OPCUA::Client::PushClient<DataType>(smart_component.get()));
}
IPushServerPtrType TestingEnvironment::createPushServer(const std::string &name)
{
	auto smart_component = std::dynamic_pointer_cast<SeRoNet::Utils::Component>(component);
	return IPushServerPtrType(new SeRoNet::OPCUA::Server::PushServer<DataType>(smart_component.get(), name));
}

ISendClientPtrType TestingEnvironment::createSendClient()
{
	auto smart_component = std::dynamic_pointer_cast<SeRoNet::Utils::Component>(component);
	return ISendClientPtrType(new SeRoNet::OPCUA::Client::SendClient<DataType>(smart_component.get()));
}
ISendServerPtrType TestingEnvironment::createSendServer(const std::string &name, ISendServerHandlerPtrType handler)
{
	auto smart_component = std::dynamic_pointer_cast<SeRoNet::Utils::Component>(component);
	return ISendServerPtrType(new SeRoNet::OPCUA::Server::SendServer<DataType>(smart_component.get(), name, handler));
}

IQueryClientPtrType TestingEnvironment::createQueryClient()
{
	auto smart_component = std::dynamic_pointer_cast<SeRoNet::Utils::Component>(component);
	return IQueryClientPtrType(new SeRoNet::OPCUA::Client::QueryClient<RequestType,AnswerType>(smart_component.get()));
}
IQueryServerPtrType TestingEnvironment::createQueryServer(const std::string &name, IQueryServerHandlerPtrType handler)
{
	auto smart_component = std::dynamic_pointer_cast<SeRoNet::Utils::Component>(component);
	return IQueryServerPtrType(new SeRoNet::OPCUA::Server::QueryServer<RequestType,AnswerType>(smart_component.get(), name, handler));
}

IEventClientPtrType TestingEnvironment::createEventClient()
{
	auto smart_component = std::dynamic_pointer_cast<SeRoNet::Utils::Component>(component);
	return IEventClientPtrType(new SeRoNet::OPCUA::Client::EventClient<ActivationType,EventType>(smart_component.get()));
}
IEventServerPtrType TestingEnvironment::createEventServer(const std::string &name, IEventTestHandlerPtrType handler)
{
	auto smart_component = std::dynamic_pointer_cast<SeRoNet::Utils::Component>(component);
	return IEventServerPtrType(new SeRoNet::OPCUA::Server::EventServer<ActivationType,EventType>(smart_component.get(), name, handler));
}

} /* namespace SmartOPCUA */
