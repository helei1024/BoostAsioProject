#include "LogUtil.h"

#include<iostream>
#include<log4cpp/PatternLayout.hh>
#include<log4cpp/OstreamAppender.hh>
#include<log4cpp/FileAppender.hh>
#include<log4cpp/RollingFileAppender.hh>
#include<log4cpp/Priority.hh>

using namespace std;

Mylog* Mylog::plog_ = NULL;

//获取log指针
Mylog& Mylog::getInstance() {
	if (plog_ == NULL) {
		plog_ = new Mylog;
	}
	return *plog_;
}


//销毁
void Mylog::destory() {
	if (plog_) {
		plog_->category_ref_.info("Mylog destroy");
		plog_->category_ref_.shutdown();
		delete plog_;
	}
}


//构造函数
Mylog::Mylog() :
category_ref_(log4cpp::Category::getRoot()) {
	//自定义输出格式
	log4cpp::PatternLayout *pattern_one =
		new log4cpp::PatternLayout;
	pattern_one->setConversionPattern("%d: %p %c %x:%m%n");

	log4cpp::PatternLayout *pattern_two =
		new log4cpp::PatternLayout;
	pattern_two->setConversionPattern("%d: %p %c %x:%m%n");

	//获取屏幕输出
	log4cpp::OstreamAppender *os_appender =
		new log4cpp::OstreamAppender("osAppender", &std::cout);
	os_appender->setLayout(pattern_one);

	//获取文件日志输出 （ 日志文件名:mylog.txt )   不拆分文件大小 一直在一个文件中输出
	//log4cpp::FileAppender *file_appender = new log4cpp::FileAppender("fileAppender", "mylog.txt");

	//获取文件日志输出 （ 日志文件名 : mylog.txt ) 按照指定大小 拆分日志文件
	log4cpp::RollingFileAppender* file_appender = new log4cpp::RollingFileAppender("rollfileAppender", "log/mylog.txt", 1000 * 1024, 1);

	file_appender->setLayout(pattern_two);

	category_ref_.setPriority(log4cpp::Priority::DEBUG);
	category_ref_.addAppender(os_appender);
	category_ref_.addAppender(file_appender);

	category_ref_.info("Mylog created!");
}


//设置优先级
void Mylog::setPriority(Priority priority) {
	switch (priority) {
	case (MYERROR) :
		category_ref_.setPriority(log4cpp::Priority::ERROR);
		break;

	case (MYWARN) :
		category_ref_.setPriority(log4cpp::Priority::WARN);
		break;

	case (MYINFO) :
		category_ref_.setPriority(log4cpp::Priority::INFO);
		break;

	case (MYDEBUG) :
		category_ref_.setPriority(log4cpp::Priority::DEBUG);
		break;

	default:
		category_ref_.setPriority(log4cpp::Priority::DEBUG);
		break;
	}
}


void Mylog::error(const char* msg) {
	category_ref_.error(msg);
}

void Mylog::warn(const char* msg) {
	category_ref_.warn(msg);
}

void Mylog::info(const char* msg) {
	category_ref_.info(msg);
}

void Mylog::debug(const char* msg) {
	category_ref_.debug(msg);
}