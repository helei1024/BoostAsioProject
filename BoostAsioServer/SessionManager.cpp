#include "stdafx.h"
#include "SessionManager.h"


namespace pingcuo{
	SessionManager::SessionManager(boost::asio::io_service& io_srv, int type, int expires_time)
		:m_io_srv(io_srv), m_check_tick(io_srv), m_type(type), m_expires_time(expires_time), m_next_session(0)
	{
		check_connection();
	}

	SessionManager::~SessionManager()
	{

	}

	//������������session������״̬  
	void SessionManager::check_connection()
	{
		try{
			writeLock lock(m_mutex);
			string strLog_ = "��ʼ������������session������״̬,��ǰ�߳�ID:" + to_string(boost::this_thread::get_id());
			logInfo(strLog_);
			//������������
			int normalCount = m_sessions.size();

			session_set::iterator iter = m_sessions.begin();
			while (iter != m_sessions.end())
			{
				
				if (CLIENT == m_type)//�ͻ��˵ķ�ʽ  
				{
					if (!iter->session->socket().is_open())//�ѶϿ���ɾ���ѶϿ�������  
					{
						string strLog = "��������[" + iter->address + "]";
						logInfo(strLog);

						iter->session->close(); //ͨ���رմ����ͻ�������  
					}
					else{//�����У���������  
						Message msg;
						msg.m_command = heartbeat;
						msg.m_business_type = iter->session->get_business_type();
						msg.m_app_id = iter->session->get_app_id();
						msg.m_data = "H";

						iter->session->async_write(msg);
						iter->session->set_op_time();

					}
				}
				else if (SERVER == m_type)//�������ķ�ʽ  
				{
					if (!iter->session->socket().is_open())//�ѶϿ���ɾ���ѶϿ�������  
					{
						normalCount--;
						string strLog = "ɾ���ѹرյ�session:[" + iter->session->get_remote_addr() + "]";
						logInfo(strLog);
						iter = m_sessions.erase(iter);
						continue;
					}
					else{//�����У��趨ÿ30����һ��  
						if (iter->session->is_timeout()) //���session�ѳ�ʱ��û��������ر�  
						{
							normalCount--;
							string strLog = "ɾ���ѳ�ʱ��session:[" + iter->session->get_remote_addr() + "]";
							logInfo(strLog);
							iter->session->close();//ͨ���رմ���ɾ��session  
						}
					}

					iter->session->set_op_time();
				}
				else{
					string strLog = "unknown manager_type";
					logInfo(strLog);
				}
				++iter;
			}

			string strLog = "��ɶ�ʱ��飬��������������" + to_string(normalCount);
			logInfo(strLog);
			m_check_tick.expires_from_now(boost::posix_time::seconds(m_expires_time));
			m_check_tick.async_wait(boost::bind(&SessionManager::check_connection, this));
		}
		catch (std::exception& e)
		{
			std::string strLog = "["+ std::string(e.what())+ "]";
			logInfo(strLog);
		}
		catch (...)
		{
			string strLog = "unknown exception.";
			logInfo(strLog);
		}
	}

	void SessionManager::add_session(socket_session_ptr p)
	{
		writeLock lock(m_mutex);
		session_stu stuSession;
		stuSession.id = p->id();
		stuSession.business_type = 0;
		stuSession.address = p->get_remote_addr();
		stuSession.app_id = p->get_app_id();
		stuSession.session = p;
		m_sessions.insert(stuSession);
	}

	void SessionManager::update_session(socket_session_ptr p)
	{
		writeLock lock(m_mutex);
		if (m_sessions.empty())
		{
			return;
		}

		MULTI_MEMBER_CON(sid) idx = boost::multi_index::get<sid>(m_sessions);
		BOOST_AUTO(iter, idx.find(p->id()));

		if (iter != idx.end())
		{
			const_cast<session_stu&>(*iter).business_type = p->get_business_type();
			const_cast<session_stu&>(*iter).app_id = p->get_app_id();
		}
	}
}
