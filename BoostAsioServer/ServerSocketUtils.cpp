#include "stdafx.h"
#include "ServerSocketUtils.h"
#include "ioservice/IoServicePool.h"



namespace pingcuo{
	ServerSocketUtils::ServerSocketUtils(int port)
		:m_work(IoServicePool::singleton::GetInstance()->GetNextIoService_()),
		m_acceptor(IoServicePool::singleton::GetInstance()->GetNextIoService_(), tcp::endpoint(tcp::v4(), port)),
		m_manager(IoServicePool::singleton::GetInstance()->GetNextIoService_(), SERVER, 3)
	{
		//m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));  
		//// �ر�����ǰ��0����ͻ���������  
		//m_acceptor.set_option(boost::asio::ip::tcp::acceptor::linger(true, 0));  
		//m_acceptor.set_option(boost::asio::ip::tcp::no_delay(true));  
		//m_acceptor.set_option(boost::asio::socket_base::keep_alive(true));  
		//m_acceptor.set_option(boost::asio::socket_base::receive_buffer_size(16384));  
	}

	ServerSocketUtils::~ServerSocketUtils(void)
	{
	}

	void ServerSocketUtils::start()
	{
		try{
			socket_session_ptr new_session(new SocketSession(IoServicePool::singleton::GetInstance()->GetNextIoService_()));
			m_acceptor.async_accept(new_session->socket(),
				boost::bind(&ServerSocketUtils::handle_accept, this, new_session,
				boost::asio::placeholders::error));
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "socket�쳣:[" << e.what() << "]");
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "socket�쳣:[δ֪�쳣]");
		}
	}

	void ServerSocketUtils::handle_accept(socket_session_ptr session, const boost::system::error_code& error)
	{
		if (!error)
		{
			try{
				// ������δ������ �滻�ɵ���start();
				socket_session_ptr new_session(new SocketSession(IoServicePool::singleton::GetInstance()->GetNextIoService_()));
				m_acceptor.async_accept(new_session->socket(),
					boost::bind(&ServerSocketUtils::handle_accept, this, new_session,
					boost::asio::placeholders::error));
				
				if (session != NULL)
				{
					//ע��رջص�����  
					session->installCloseCallBack(boost::bind(&ServerSocketUtils::close_callback, this, _1));
					//ע��������ݻص�����  
					session->installReadDataCallBack(boost::bind(&ServerSocketUtils::read_data_callback, this, _1, _2, _3));

					boost::format fmt("%1%:%2%");
					fmt % session->socket().remote_endpoint().address().to_string();
					fmt % session->socket().remote_endpoint().port();
					session->set_remote_addr(fmt.str());

					session->start();
					m_manager.add_session(session);
				}
			}
			catch (std::exception& e)
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "socket�쳣:[" << e.what() << "]");
			}
			catch (...)
			{
				//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "socket�쳣:[δ֪�쳣]");
			}

		}
	}

	void ServerSocketUtils::close_callback(socket_session_ptr session)
	{
		//LOG4CXX_DEBUG(firebird_log, "close_callback");
		try{
			m_manager.del_session<sid>(session->id());
		}
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "socket�쳣:[" << e.what() << "]");
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "socket�쳣:[δ֪�쳣]");
		}

	}

	void ServerSocketUtils::read_data_callback(const boost::system::error_code& e,
		socket_session_ptr session, Message& msg)
	{
		try{
			//LOG4CXX_DEBUG(firebird_log, "command =[" << msg.command << "],["
				//<< msg.business_type << "],[" << msg.data() << "]");
			//���� 
			if (msg.m_command == heartbeat)
			{
				string strLog = "�ͻ��ˣ�" +session->get_remote_addr() + "����������Ϣ.";
				logInfo(strLog);
				session->async_write(msg);
			}
			//ע��
			else if (msg.m_command == regist)
			{  
				session->set_business_type(msg.m_business_type);
				session->set_app_id(msg.m_app_id);
				m_manager.update_session(session);

				session->async_write(msg);
				//LOG4CXX_FATAL(firebird_log, "Զ�̵�ַ:[" << session->get_remote_addr() << "],����������:[" <<
					//session->get_business_type() << "],������ID:[" << session->get_app_id() << "]ע��ɹ�!");

				string strLog = "Զ�̵�ַ:[" + session->get_remote_addr() + "],����������:[" + to_string(session->get_business_type()) + \
					"],������ID:[" + to_string(session->get_app_id()) + "]ע��ɹ�!";
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
		catch (std::exception& e)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "socket�쳣:[" << e.what() << "]");
		}
		catch (...)
		{
			//LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "socket�쳣:[δ֪�쳣]");
		}
	}
}