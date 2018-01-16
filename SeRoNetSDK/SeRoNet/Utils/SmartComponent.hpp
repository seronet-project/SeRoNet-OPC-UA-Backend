#pragma once
#include <open62541.h>

namespace SeRoNet {
	namespace Utils {

		enum StatusCode
		{
			SMART_OK,
			SMART_WRONGID,
			SMART_DISCONNECTED,
			SMART_ERROR_COMMUNICATION,
			SMART_ERROR,
		};

		
		class SmartComponent {

		private:
			UA_ServerConfig* config;
			UA_Server * server;
			UA_Boolean running = true;

		public:
			SmartComponent() {
				config = UA_ServerConfig_new_default();
				server = UA_Server_new(config);
			};

			void run() {
				UA_Server_run(server, &running);
			}

			UA_Server* getServer() {
				return server;
			}
		};

	}
}