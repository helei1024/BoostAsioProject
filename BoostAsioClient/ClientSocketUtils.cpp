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
			//ע��رջص�����  
			pSession->installCloseCallBack(boost::bind(&ClientSocketUtils::close_callback, this, _1));
			//ע��������ݻص�����  
			pSession->installReadDataCallBack(boost::bind(&ClientSocketUtils::read_data_callback, this, _1, _2, _3));

			std::vector<std::string> ip_port;
			boost::split(ip_port, addr, boost::is_any_of(":"));

			if (ip_port.size() < 2)
			{
				//throw std::runtime_error("ip ��ʽ����ȷ��");  
				//LOG4CXX_ERROR(firebird_log, "[" << addr << "] ip ��ʽ����ȷ!");
				return;
			}

			tcp::resolver resolver(pSession->socket().get_io_service());
			tcp::resolver::query query(ip_port[0], ip_port[1]);
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			//pSession->set_begin_endpoint(endpoint_iterator);//������ʼ��ַ���Ա�����  

			//���ڿͻ����ǲ��������ģ���ʹ��δ����ҲҪ�����session  
			m_manager.add_session(pSession);

			tcp::endpoint endpoint = *endpoint_iterator;
			pSession->socket().async_connect(endpoint,
				boost::bind(&ClientSocketUtils::handle_connect, this,
				boost::asio::placeholders::error, ++endpoint_iterator, pSession));
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << addr << "],socket�쳣:[" << e.what() << "]");
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << addr << "],socket�쳣:[δ֪�쳣]");
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
				//LOG4CXX_FATAL(firebird_log, "������:[" << pSession->get_business_type() << "],����Զ�̵�ַ:[" << pSession->get_remote_addr().c_str() << "]�ɹ�!");
				pSession->start();

				//�������ע���������  
				Message msg;
				msg.m_command = regist;
				msg.m_business_type = pSession->get_business_type();
				msg.m_app_id = 9666;//pSession->get_app_id();
				msg.m_data = "R";

				pSession->async_write(msg);
			}
			else if (endpoint_iterator != tcp::resolver::iterator())
			{
				//LOG4CXX_ERROR(firebird_log, "����Զ�̵�ַ:[" << pSession->get_remote_addr().c_str() << "]ʧ�ܣ���ͼ������һ����ַ��");
				pSession->socket().close();//�˴���socket��close����Ӧ��session��close�������ӣ���Ȼ�ᵼ��һֱ����  
				tcp::endpoint endpoint = *endpoint_iterator;
				pSession->socket().async_connect(endpoint,
					boost::bind(&ClientSocketUtils::handle_connect, this,
					boost::asio::placeholders::error, ++endpoint_iterator, pSession));
			}
			else
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << pSession->get_remote_addr().c_str() << "]ʧ�ܣ�");
				pSession->socket().close();//�˴���socket��close����Ӧ��session��close�������ӣ���Ȼ�ᵼ��һֱ����  
			}
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << pSession->get_remote_addr().c_str() << "],socket�쳣:[" << e.what() << "]");
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << pSession->get_remote_addr().c_str() << "],socket�쳣:[δ֪�쳣]");
		}
	}

	void ClientSocketUtils::read_data_callback(const boost::system::error_code& e,
		socket_session_ptr session, Message& msg)
	{
		//LOG4CXX_DEBUG(firebird_log, "command =[" << msg.command << "],["
			//<< msg.business_type << "],[" << msg.data() << "]");

		if (msg.m_command == heartbeat)
		{//����  
		}
		else if (msg.m_command == regist)
		{//ע��  
			//LOG4CXX_FATAL(firebird_log, "������:[" << session->get_business_type() << "]ע��ɹ���");
			string strLog = "������:[" + to_string(session->get_business_type()) + "]ע��ɹ���";
			logInfo(strLog);
		}
		else if (msg.m_command == normal)
		{//ҵ������  
			handle_read_data(msg, session);
		}
		else
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "�յ��Ƿ���Ϣ��!");
		}
	}

	//�ر�session�ͻ�����  
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
				//LOG4CXX_ERROR(firebird_log, "[" << addr << "] ip ��ʽ����ȷ!");
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
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << session->get_remote_addr().c_str() << "],socket�쳣:[" << e.what() << "]");
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "����Զ�̵�ַ:[" << session->get_remote_addr().c_str() << "],socket�쳣:[δ֪�쳣]");
		}
	}
}
