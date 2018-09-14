// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
// 日志类实例化宏 必须定义在   #include "stdafx.h" 下面位置，  #include "LogUtil.h" 上面位置
#define _LOG4CPP_

#include "log/LogUtil.h"

#include "BusinessClientSocketUtil.h"

using namespace pingcuo;

int _tmain(int argc, _TCHAR* argv[])
{
	BusinessClientSocketUtil bc;
	
	socket_session_ptr pSession = boost::make_shared<SocketSession>(bc.get_io_service());
	string str = "127.0.0.1:9999";
	pSession->set_remote_addr(str);
	bc.session_connect(pSession);

	bc.get_io_service().run();

	return 0;
}

