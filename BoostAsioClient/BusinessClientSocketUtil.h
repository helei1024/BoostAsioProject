#pragma once
#include "ClientSocketUtils.h"


namespace pingcuo
{
	class BusinessClientSocketUtil :public pingcuo::ClientSocketUtils
	{
	public:
		BusinessClientSocketUtil();
		~BusinessClientSocketUtil();

	protected:
		// ����ҵ������
		void handle_read_data(Message& msg, socket_session_ptr pSession);
	};

}

