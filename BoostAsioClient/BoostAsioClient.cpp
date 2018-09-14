// ConsoleApplication1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
// ��־��ʵ������ ���붨����   #include "stdafx.h" ����λ�ã�  #include "LogUtil.h" ����λ��
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

