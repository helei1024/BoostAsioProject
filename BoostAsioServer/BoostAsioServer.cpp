// BoostAsioServer.cpp : �������̨Ӧ�ó������ڵ㡣
//


#include "stdafx.h"

// ��־��ʵ������ ���붨����   #include "stdafx.h" ����λ�ã�  #include "LogUtil.h" ����λ��
#define _LOG4CPP_



#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <iostream>
#include <sstream>
#include "Message.h"

#include<boost/thread/thread.hpp>

#include "BusinessServerSocketUtil.h"


#include "boost/asio.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "ioservice/IoServicePool.h"

void Print(const boost::system::error_code& error)
{
	
	cout<<boost::this_thread::get_id()<<endl;
	cout << "hello asio" << endl;
}



int _tmain(int argc, _TCHAR* argv[])
{
	Message mes("fd", 10);

	/*
	// ���л�
	std::ostringstream os;
	boost::archive::binary_oarchive oa(os);
	//���л���һ��ostringstream����
	oa << mes;
	//content���������л��������
	std::string content = os.str();
	std::cout << content<<endl;



	// �����л�
	message mes1;
	std::istringstream is(content);
	boost::archive::binary_iarchive ia(is);
	//��һ���������л����ݵ�string���淴���л����Ӷ��õ�ԭ���Ķ���
	ia >> mes1;
	cout << mes1.m_tag << "----" << mes1.m_length<<endl;
	logError(mes1.m_tag);
	*/

	

	boost::asio::io_service ios;	// ���Ե�asio�������Ҫ��һ��io_service����

	// ��ʱ��io_service��Ϊ���캯��������������֮��ʱ����ֹ
	boost::asio::deadline_timer t(ios, boost::posix_time::seconds(1));

	t.async_wait(Print);						// ����wait�첽�ȴ�

	//ios.run();

	boost::thread  th(boost::bind(&boost::asio::io_service::run, &ios));
	cout << "thread_id:" << th.get_id() << endl;
	th.join();

	cout << "it show before t expired." << endl;

	cout <<"main:threadid" <<boost::this_thread::get_id() << endl;


	logInfo("test");

	pingcuo::BusinessServerSocketUtil bs(9999);
	bs.start();
	//bs.get_io_service().run();
	IoServicePool::singleton::GetInstance()->Run();


	//while (true)
	//{ }
	return 0;
}

