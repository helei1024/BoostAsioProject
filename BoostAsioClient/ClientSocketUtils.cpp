#include "stdafx.h"
#include "ClientSocketUtils.h"


namespace pingcuo{
	ClientSocketUtils::ClientSocketUtils()
		:m_work(m_io_srv), m_manager(m_io_srv, CLIENT, 3)
	{
	}

	ClientSocketUtils::~ClientSocketUtils()
	{
	}

	void ClientSocketUtils::session_connect(std::vector<socket_session_ptr>& vSession)
	{
		for (int i = 0; i < vSession.size(); ++i)
		{
			session_connect(vSession[i]);
		}
	}

	void ClientSocketUtils::session_connect(socket_session_ptr pSession)
	{
		std::string& addr = pSession->get_remote_addr();
		try{
			//注册关闭回调函数  
			pSession->installCloseCallBack(boost::bind(&ClientSocketUtils::close_callback, this, _1));
			//注册读到数据回调函数  
			pSession->installReadDataCallBack(boost::bind(&ClientSocketUtils::read_data_callback, this, _1, _2, _3));

			std::vector<std::string> ip_port;
			boost::split(ip_port, addr, boost::is_any_of(":"));

			if (ip_port.size() < 2)
			{
				//throw std::runtime_error("ip 格式不正确！");  
				//LOG4CXX_ERROR(firebird_log, "[" << addr << "] ip 格式不正确!");
				return;
			}

			tcp::resolver resolver(pSession->socket().get_io_service());
			tcp::resolver::query query(ip_port[0], ip_port[1]);
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			//pSession->set_begin_endpoint(endpoint_iterator);//设置起始地址，以便重连  

			//由于客户端是不断重连的，即使还未连接也要保存该session  
			m_manager.add_session(pSession);

			tcp::endpoint endpoint = *endpoint_iterator;
			pSession->socket().async_connect(endpoint,
				boost::bind(&ClientSocketUtils::handle_connect, this,
				boost::asio::placeholders::error, ++endpoint_iterator, pSession));
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << addr << "],socket异常:[" << e.what() << "]");
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << addr << "],socket异常:[未知异常]");
		}
	}

	void ClientSocketUtils::handle_connect(const boost::system::error_code& error,
		tcp::resolver::iterator endpoint_iterator, socket_session_ptr pSession)
	{
		//LOG4CXX_DEBUG(firebird_log, KDS_CODE_INFO << " enter.");
		std::string sLog;
		try{
			if (!error)
			{
				//LOG4CXX_FATAL(firebird_log, "服务器:[" << pSession->get_business_type() << "],连接远程地址:[" << pSession->get_remote_addr().c_str() << "]成功!");
				pSession->start();

				//向服务器注册服务类型  
				Message msg;
				msg.m_command = regist;
				msg.m_business_type = pSession->get_business_type();
				msg.m_app_id = 9666;//pSession->get_app_id();
				msg.m_data = "R";

				pSession->async_write(msg);
			}
			else if (endpoint_iterator != tcp::resolver::iterator())
			{
				//LOG4CXX_ERROR(firebird_log, "连接远程地址:[" << pSession->get_remote_addr().c_str() << "]失败，试图重连下一个地址。");
				pSession->socket().close();//此处用socket的close，不应用session的close触发连接，不然会导致一直重连  
				tcp::endpoint endpoint = *endpoint_iterator;
				pSession->socket().async_connect(endpoint,
					boost::bind(&ClientSocketUtils::handle_connect, this,
					boost::asio::placeholders::error, ++endpoint_iterator, pSession));
			}
			else
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << pSession->get_remote_addr().c_str() << "]失败！");
				pSession->socket().close();//此处用socket的close，不应用session的close触发连接，不然会导致一直重连  
			}
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << pSession->get_remote_addr().c_str() << "],socket异常:[" << e.what() << "]");
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << pSession->get_remote_addr().c_str() << "],socket异常:[未知异常]");
		}
	}

	void ClientSocketUtils::read_data_callback(const boost::system::error_code& e,
		socket_session_ptr session, Message& msg)
	{
		//LOG4CXX_DEBUG(firebird_log, "command =[" << msg.command << "],["
			//<< msg.business_type << "],[" << msg.data() << "]");

		if (msg.m_command == heartbeat)
		{//心跳  
		}
		else if (msg.m_command == regist)
		{//注册  
			//LOG4CXX_FATAL(firebird_log, "服务器:[" << session->get_business_type() << "]注册成功。");
			string strLog = "服务器:[" + to_string(session->get_business_type()) + "]注册成功。";
			logInfo(strLog);
		}
		else if (msg.m_command == normal)
		{//业务数据  
			handle_read_data(msg, session);
		}
		else
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "收到非法消息包!");
		}
	}

	//关闭session就会重连  
	void ClientSocketUtils::close_callback(socket_session_ptr session)
	{
		//LOG4CXX_DEBUG(firebird_log, KDS_CODE_INFO << "enter.");

		try{
			//tcp::resolver::iterator endpoint_iterator = context.session->get_begin_endpoint();  

			std::string& addr = session->get_remote_addr();

			std::vector<std::string> ip_port;
			boost::split(ip_port, addr, boost::is_any_of(":"));

			if (ip_port.size() < 2)
			{
				//LOG4CXX_ERROR(firebird_log, "[" << addr << "] ip 格式不正确!");
				return;
			}

			tcp::resolver resolver(session->socket().get_io_service());
			tcp::resolver::query query(ip_port[0], ip_port[1]);
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

			tcp::endpoint endpoint = *endpoint_iterator;
			session->socket().async_connect(endpoint,
				boost::bind(&ClientSocketUtils::handle_connect, this,
				boost::asio::placeholders::error, ++endpoint_iterator, session));
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << session->get_remote_addr().c_str() << "],socket异常:[" << e.what() << "]");
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << session->get_remote_addr().c_str() << "],socket异常:[未知异常]");
		}
	}
}
