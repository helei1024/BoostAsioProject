#include "stdafx.h"
#include "BusinessServerSocketUtil.h"

namespace pingcuo
{
	BusinessServerSocketUtil::BusinessServerSocketUtil(int port):ServerSocketUtils(port)
	{

	}


	BusinessServerSocketUtil::~BusinessServerSocketUtil()
	{
	}

	void BusinessServerSocketUtil::handle_read_data(Message& msg, socket_session_ptr pSession)
	{
		
		// todo

		logInfo("Ω” ’µΩ£∫message£°");
	}

}
