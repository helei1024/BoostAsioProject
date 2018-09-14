#include "LogUtil.h"

#include<iostream>
#include<log4cpp/PatternLayout.hh>
#include<log4cpp/OstreamAppender.hh>
#include<log4cpp/FileAppender.hh>
#include<log4cpp/RollingFileAppender.hh>
#include<log4cpp/Priority.hh>

using namespace std;

Mylog* Mylog::plog_ = NULL;

//��ȡlogָ��
Mylog& Mylog::getInstance() {
	if (plog_ == NULL) {
		plog_ = new Mylog;
	}
	return *plog_;
}


//����
void Mylog::destory() {
	if (plog_) {
		plog_->category_ref_.info("Mylog destroy");
		plog_->category_ref_.shutdown();
		delete plog_;
	}
}


//���캯��
Mylog::Mylog() :
category_ref_(log4cpp::Category::getRoot()) {
	//�Զ��������ʽ
	log4cpp::PatternLayout *pattern_one =
		new log4cpp::PatternLayout;
	pattern_one->setConversionPattern("%d: %p %c %x:%m%n");

	log4cpp::PatternLayout *pattern_two =
		new log4cpp::PatternLayout;
	pattern_two->setConversionPattern("%d: %p %c %x:%m%n");

	//��ȡ��Ļ���
	log4cpp::OstreamAppender *os_appender =
		new log4cpp::OstreamAppender("osAppender", &std::cout);
	os_appender->setLayout(pattern_one);

	//��ȡ�ļ���־��� �� ��־�ļ���:mylog.txt )   ������ļ���С һֱ��һ���ļ������
	//log4cpp::FileAppender *file_appender = new log4cpp::FileAppender("fileAppender", "mylog.txt");

	//��ȡ�ļ���־��� �� ��־�ļ��� : mylog.txt ) ����ָ����С �����־�ļ�
	log4cpp::RollingFileAppender* file_appender = new log4cpp::RollingFileAppender("rollfileAppender", "log/mylog.txt", 1000 * 1024, 1);

	file_appender->setLayout(pattern_two);

	category_ref_.setPriority(log4cpp::Priority::DEBUG);
	category_ref_.addAppender(os_appender);
	category_ref_.addAppender(file_appender);

	category_ref_.info("Mylog created!");
}


//�������ȼ�
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