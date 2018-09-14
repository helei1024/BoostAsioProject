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
			std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[" + e.what() + "]";
			logError(strLog);
			
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[δ֪�쳣]");
		}
	}

	void SocketSession::close()
	{
		//���ڻص����м���������������ύ������һ���߳�ȥ������Ȼ���������  
		m_io_service.post(boost::bind(&SocketSession::handle_close, shared_from_this()));
	}

	static int connection_timeout = 60;

	bool SocketSession::is_timeout()
	{
		std::time_t now;
		std::time(&now);
		return now - m_last_op_time > connection_timeout;
	}

	//����Ϣͷ  
	void SocketSession::handle_read_header(const boost::system::error_code& error)
	{
		//LOG4CXX_DEBUG(firebird_log, KDS_CODE_INFO << "enter.");
		try{
			if (error)
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[" << error.message().c_str() << "]");
				std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[" + error.message().c_str() +"]";
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
			{//�������ݻص�ע���READ_DATA����  
				Message msg;
				// �����л� todo
				//message_iarchive(msg, data);



				//*********************************** �����л�*************************************
				std::istringstream is(data);
				boost::archive::binary_iarchive ia(is);
				//��һ���������л����ݵ�string���淴���л����Ӷ��õ�ԭ���Ķ���
				ia >> msg;
				//*********************************** �����л�*************************************



				read_data_cb(error, shared_from_this(), msg);
			}
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[" << e.what() << "]");
			std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[" + e.what() + "]";
			logError(strLog);

			close();
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[δ֪�쳣]");
			std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[δ֪�쳣]";
			logError(strLog);
			close();
		}
	}

	//����Ϣ��  
	void SocketSession::handle_read_body(const boost::system::error_code& error)
	{
		//LOG4CXX_DEBUG(firebird_log, KDS_CODE_INFO << "enter.");
		try{
			if (error)
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[" << error.message().c_str() << "]");
				std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[" + error.message().c_str() + "]";
				logError(strLog);
				close();
				return;
			}

			if (tag.compare(0, tag.length(), sHeader.data(), 0, tag.length()))
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[���Ǹ��Ƿ�����!]");
				std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[���Ǹ��Ƿ�����!]";
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
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[" << e.what() << "]");
			std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[" + e.what() +"]";
			logError(strLog);
			close();
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[δ֪�쳣]");
			std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[δ֪�쳣]";
			logError(strLog);
			close();
		}
	}

	void SocketSession::handle_write(const boost::system::error_code& error,
		std::size_t bytes_transferred, std::string* pmsg)
	{
		//���ݷ��ͳɹ�������  
		if (pmsg != NULL)
		{
			delete pmsg;
		}

		if (error)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[" << error.message().c_str() << "]");
			std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[" + error.message().c_str() + "]";
			logError(strLog);
			close();
			return;
		}
	}

	void SocketSession::async_write(const std::string& sMsg)
	{
		//LOG4CXX_DEBUG(firebird_log, KDS_CODE_INFO << "enter.")
		string strLog_ = "��������,��ǰ�߳�ID:" + to_string(boost::this_thread::get_id());
		logInfo(strLog_);

		try
		{
			DWORD dwLength = sMsg.size();
			char* pLen = (char*)&dwLength;

			//�������첽���ͣ�Ҫ��֤���ݷ�������ʱ���Ű���������  
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
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[" << e.what() << "]");
			std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[" + e.what() + "]";
			logError(strLog);
			close();
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << get_remote_addr() << "],socket�쳣:[δ֪�쳣]");
			std::string strLog = "����Զ�̵�ַ:[" + get_remote_addr() + "],socket�쳣:[δ֪�쳣]";
			logError(strLog);
			close();
		}
	}

	void SocketSession::async_write(Message& msg)
	{
		std::string data;

		// ���л� todo
		//message_oarchive(data, msg);

		/***************************���л�*******************************************/
		std::ostringstream os;
		boost::archive::binary_oarchive oa(os);
		//���л���һ��ostringstream����
		oa << msg;
		//content���������л��������
		data = os.str();
		/**********************************************************************/

		async_write(data);
	}
}