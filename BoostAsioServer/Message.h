#pragma once

#include <string>
#include <boost/serialization/access.hpp>

using namespace std;
class Message
{
public:
	Message();
	Message(char *tag,int len);
	~Message();

public:
	//char m_tag[3];
	// ����Body�ĳ���
	//int m_length;
	// ��ʾ���ݰ������ͣ�0��ʾ����������������Ҫ��������������Ƿ���������1��ʾע������ͻ��������Ϸ�����֮��Ҫ�������Ϣע�������������2��ʾҵ����Ϣ����
	short m_command;
	// ҵ����Ϣ�����ͣ������������ҵ����Ϣ�����ͽ�����·�ɵ���Ӧ�Ŀͻ��ˣ��ͻ�������ҵ�����ͷ���ģ���
	short m_business_type;
	// �ͻ���Ψһ��ʶ
	int m_app_id;
	// ��Ϣ����
	string m_data;

};


//���л�����
namespace boost {
	namespace serialization {
		template<class Archive>
		void serialize(Archive & ar, Message & m, const unsigned int version)
		{
			//ar & m.m_tag;
			//ar & m.m_length;
			ar & m.m_command;
			ar & m.m_business_type;
			ar & m.m_app_id;
			ar & m.m_data;
		}

	} // namespace serialization
} // namespace boost

