#include "stdafx.h"
#include "SocketSession.h"



namespace pingcuo
{ 
	boost::detail::atomic_count SocketSession::m_last_id(0);


	SocketSession::SocketSession(boost::asio::io_service& io_srv)
		:m_io_service(io_srv), m_socket(io_srv),
		m_business_type(0), m_app_id(0)
	{
		m_id = ++SocketSession::m_last_id;
	}

	SocketSession::~SocketSession(void)
	{
		m_socket.close();
	}

	void SocketSession::start()
	{
		m_socket.set_option(boost::asio::ip::tcp::acceptor::linger(true, 0));
		m_socket.set_option(boost::asio::socket_base::keep_alive(true));
		std::time(&m_last_op_time);
		const boost::system::error_code error;
		handle_read_header(error);
	}

	void SocketSession::handle_close()
	{
		try{
			m_socket.close();
			close_cb(shared_from_this());
		}
		catch (std::exception& e)
		{
			std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[" + e.what() + "]";
			logError(strLog);
			
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[未知异常]");
		}
	}

	void SocketSession::close()
	{
		//由于回调中有加锁的情况，必须提交到另外一个线程去做，不然会出现死锁  
		m_io_service.post(boost::bind(&SocketSession::handle_close, shared_from_this()));
	}

	static int connection_timeout = 60;

	bool SocketSession::is_timeout()
	{
		std::time_t now;
		std::time(&now);
		return now - m_last_op_time > connection_timeout;
	}

	//读消息头  
	void SocketSession::handle_read_header(const boost::system::error_code& error)
	{
		//LOG4CXX_DEBUG(firebird_log, KDS_CODE_INFO << "enter.");
		try{
			if (error)
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[" << error.message().c_str() << "]");
				std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[" + error.message().c_str() +"]";
				logError(strLog);
				close();
				return;
			}

			std::string data;
			data.swap(sBody);
			boost::asio::async_read(m_socket,
				boost::asio::buffer(sHeader),
				boost::bind(&SocketSession::handle_read_body, shared_from_this(),
				boost::asio::placeholders::error));

			if (data.length() > 0 && data != "")
			{//读到数据回调注册的READ_DATA函数  
				Message msg;
				// 反序列化 todo
				//message_iarchive(msg, data);



				//*********************************** 反序列化*************************************
				std::istringstream is(data);
				boost::archive::binary_iarchive ia(is);
				//从一个保存序列化数据的string里面反序列化，从而得到原来的对象
				ia >> msg;
				//*********************************** 反序列化*************************************



				read_data_cb(error, shared_from_this(), msg);
			}
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[" << e.what() << "]");
			std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[" + e.what() + "]";
			logError(strLog);

			close();
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[未知异常]");
			std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[未知异常]";
			logError(strLog);
			close();
		}
	}

	//读消息体  
	void SocketSession::handle_read_body(const boost::system::error_code& error)
	{
		//LOG4CXX_DEBUG(firebird_log, KDS_CODE_INFO << "enter.");
		try{
			if (error)
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[" << error.message().c_str() << "]");
				std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[" + error.message().c_str() + "]";
				logError(strLog);
				close();
				return;
			}

			if (tag.compare(0, tag.length(), sHeader.data(), 0, tag.length()))
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[这是个非法连接!]");
				std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[这是个非法连接!]";
				logError(strLog);
				close();
				return;
			}

			DWORD dwLength = 0;

			char* len = (char*)&dwLength;
			memcpy(len, &sHeader[tag.length()], sizeof(dwLength));

			sBody.resize(dwLength);
			char* pBody = &sBody[0];

			boost::asio::async_read(m_socket,
				boost::asio::buffer(pBody, dwLength),
				boost::bind(&SocketSession::handle_read_header, shared_from_this(),
				boost::asio::placeholders::error));
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[" << e.what() << "]");
			std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[" + e.what() +"]";
			logError(strLog);
			close();
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[未知异常]");
			std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[未知异常]";
			logError(strLog);
			close();
		}
	}

	void SocketSession::handle_write(const boost::system::error_code& error,
		std::size_t bytes_transferred, std::string* pmsg)
	{
		//数据发送成功就销毁  
		if (pmsg != NULL)
		{
			delete pmsg;
		}

		if (error)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[" << error.message().c_str() << "]");
			std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[" + error.message().c_str() + "]";
			logError(strLog);
			close();
			return;
		}
	}

	void SocketSession::async_write(const std::string& sMsg)
	{
		//LOG4CXX_DEBUG(firebird_log, KDS_CODE_INFO << "enter.")
		string strLog_ = "发送数据,当前线程ID:" + to_string(boost::this_thread::get_id());
		logInfo(strLog_);

		try
		{
			DWORD dwLength = sMsg.size();
			char* pLen = (char*)&dwLength;

			//由于是异步发送，要保证数据发送完整时，才把数据销毁  
			std::string* msg = new std::string();
			msg->append(tag);
			msg->append(pLen, sizeof(dwLength));
			msg->append(sMsg);

			boost::asio::async_write(m_socket, boost::asio::buffer(*msg, msg->size()),
				boost::bind(&SocketSession::handle_write, shared_from_this(),
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
				msg));

		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[" << e.what() << "]");
			std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[" + e.what() + "]";
			logError(strLog);
			close();
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << get_remote_addr() << "],socket异常:[未知异常]");
			std::string strLog = "连接远程地址:[" + get_remote_addr() + "],socket异常:[未知异常]";
			logError(strLog);
			close();
		}
	}

	void SocketSession::async_write(Message& msg)
	{
		std::string data;

		// 序列化 todo
		//message_oarchive(data, msg);

		/***************************序列化*******************************************/
		std::ostringstream os;
		boost::archive::binary_oarchive oa(os);
		//序列化到一个ostringstream里面
		oa << msg;
		//content保存了序列化后的数据
		data = os.str();
		/**********************************************************************/

		async_write(data);
	}
}