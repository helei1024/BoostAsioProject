#ifndef _MYLOG_H
#define _MYLOG_H

#include<log4cpp/Category.hh>
#include<iostream>

//��־���ȼ�
enum Priority {
	MYERROR,
	MYWARN,
	MYINFO,
	MYDEBUG
};

class MyLog;
//�õ���ģʽ��װlog4cpp
class Mylog {
public:
	static Mylog& getInstance();
	static void destory();

	void setPriority(Priority priority);
	void error(const char* msg);
	void warn(const char* msg);
	void info(const char* msg);
	void debug(const char* msg);

private:
	Mylog();  //����ģʽ�����캯��˽�л�

private:
	static Mylog *plog_;
	log4cpp::Category &category_ref_;
};

//*****************************************************
//ע�⣺
//�ļ��� __FILE__ ,������ __func__ ���к�__LINE__ �Ǳ�����ʵ�ֵ�
//����C++ͷ�ļ��ж���� 
//ǰ����������string���ͣ���__LINE__�����Σ�������ҪתΪstring����
//******************************************************

//���������ļ��к� ->ת��Ϊstring����
inline std::string int2string(int line) {
	std::ostringstream oss;
	oss << line;
	return oss.str();
}


//����һ������־����� �ļ��� ������ �к� �ĺ궨��
#define suffix(msg)  std::string(msg).append(" ##")\
        .append(__FILE__).append(":")\
        .append(":").append(int2string(__LINE__))\
        .append("##").c_str()


//����ÿ��ʹ��ʱд getInstance��� ������ extern Mylog
//ֻ��Ҫ���������ļ���д: #define _LOG4CPP_����������������ʹ��
#ifdef _LOG4CPP_
Mylog &loggo = Mylog::getInstance();
#else
extern Mylog &loggo;
#endif

//���̲��򻯺���������ʽ
#define logError(msg) loggo.error(suffix(msg))
#define logWarn(msg) loggo.warn(suffix(msg))
#define logInfo(msg) loggo.info(suffix(msg))
#define logDebug(msg) loggo.debug(suffix(msg))

#endif