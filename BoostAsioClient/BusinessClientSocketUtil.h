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
		// 处理业务数据
		void handle_read_data(Message& msg, socket_session_ptr pSession);
	};

}

