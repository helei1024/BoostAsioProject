// BoostAsioServer.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"

// 日志类实例化宏 必须定义在   #include "stdafx.h" 下面位置，  #include "LogUtil.h" 上面位置
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
	// 序列化
	std::ostringstream os;
	boost::archive::binary_oarchive oa(os);
	//序列化到一个ostringstream里面
	oa << mes;
	//content保存了序列化后的数据
	std::string content = os.str();
	std::cout << content<<endl;



	// 反序列化
	message mes1;
	std::istringstream is(content);
	boost::archive::binary_iarchive ia(is);
	//从一个保存序列化数据的string里面反序列化，从而得到原来的对象
	ia >> mes1;
	cout << mes1.m_tag << "----" << mes1.m_length<<endl;
	logError(mes1.m_tag);
	*/

	

	boost::asio::io_service ios;	// 所以的asio程序必须要有一个io_service对象

	// 定时器io_service作为构造函数参数，两秒钟之后定时器终止
	boost::asio::deadline_timer t(ios, boost::posix_time::seconds(1));

	t.async_wait(Print);						// 调用wait异步等待

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

