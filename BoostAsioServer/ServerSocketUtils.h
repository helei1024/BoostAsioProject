#pragma once
#include "SocketSession.h"  
#include "SessionManager.h"  
#include <boost/format.hpp>  
#include "Message.h"  

namespace pingcuo{
	using boost::asio::ip::tcp;

	class  ServerSocketUtils
	{
	private:
		boost::asio::io_service m_io_srv;
		boost::asio::io_service::work m_work;
		tcp::acceptor m_acceptor;

		void handle_accept(socket_session_ptr session, const boost::system::error_code& error);

		void close_callback(socket_session_ptr session);
		void read_data_callback(const boost::system::error_code& e,
			socket_session_ptr session, Message& msg);

	protected:
		virtual void handle_read_data(Message& msg, socket_session_ptr pSession) = 0;

	public:
		ServerSocketUtils(int port);
		~ServerSocketUtils(void);

		void start();
		boost::asio::io_service& get_io_service() { return m_io_srv; }

		SessionManager m_manager;
	};
}