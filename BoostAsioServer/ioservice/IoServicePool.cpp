#include "IoServicePool.h"
#include <mutex>
#include <thread>
#include <functional>
//#include <boost/thread/thread.hpp>
//using namespace boost;
using namespace std;


void IoServicePool::Run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<std::shared_ptr<thread> > threads;
	for (std::size_t i = 0; i < io_services_.size(); ++i)
	{
		std::shared_ptr<thread> thread(new thread(bind(static_cast<std::size_t(boost::asio::io_service::*)()>(&boost::asio::io_service::run), io_services_[i])));
		threads.push_back(thread);
	}

	// Wait for all threads in the pool to exit.
	for (auto& thread : threads)
		thread->join();
}

void IoServicePool::Stop()
{
	// Explicitly stop all io_services.
	for (auto& io_service : io_services_)
		io_service->stop();
}

boost::asio::io_service& IoServicePool::GetNextIoService_()
{
	std::lock_guard<std::mutex> lock(mutex_);
	// Use a round-robin scheme to choose the next io_service to use.
	boost::asio::io_service& io_service = *io_services_[next_io_service_];
	next_io_service_ = (next_io_service_ + 1) % io_services_.size();

	return io_service;
}

IoServicePool::IoServicePool(const size_t pool_size)
{
	for (std::size_t i = 0; i < pool_size; ++i)
	{
		io_service_ptr io_service(new boost::asio::io_service);
		work_ptr work(new boost::asio::io_service::work(*io_service));
		io_services_.push_back(io_service);
		work_.push_back(work);
	}
}
