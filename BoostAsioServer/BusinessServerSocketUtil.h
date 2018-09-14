#pragma once

#include "ServerSocketUtils.h"

namespace pingcuo
{
	class BusinessServerSocketUtil : public pingcuo::ServerSocketUtils
	{
	public:
		BusinessServerSocketUtil(int port);
		~BusinessServerSocketUtil();

	protected:
		virtual void handle_read_data(Message& msg, socket_session_ptr pSession);
	};
}

