#pragma once

#include "SocketSession.h"
#include "SessionManager.h"  
#include <boost/algorithm/string.hpp>  
#include "Message.h"  

namespace pingcuo{
	class  ClientSocketUtils
	{
	public:
		ClientSocketUtils();
		~ClientSocketUtils();

		void session_connect(std::vector<socket_session_ptr>& vSession);
		void session_connect(socket_session_ptr pSession);
		//socket_session_ptr get_session(std::string& addr);  
		boost::asio::io_service& get_io_service() { return m_io_srv; }

	protected:
		virtual void handle_read_data(Message& msg, socket_session_ptr pSession) = 0;

	private:
		boost::asio::io_service m_io_srv;
		boost::asio::io_service::work m_work;
		SessionManager m_manager;

		void handle_connect(const boost::system::error_code& error,
			tcp::resolver::iterator endpoint_iterator, socket_session_ptr pSession);

		void close_callback(socket_session_ptr session);
		void read_data_callback(const boost::system::error_code& e,
			socket_session_ptr session, Message& msg);
	};
}
