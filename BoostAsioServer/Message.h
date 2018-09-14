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
	// 描述Body的长度
	//int m_length;
	// 表示数据包的类型，0表示心跳包（长连接需要心跳来检测连接是否正常），1表示注册包（客户端连接上服务器之后要将相关信息注册给服务器），2表示业务消息包；
	short m_command;
	// 业务消息包类型，服务器会根据业务消息包类型将数据路由到对应的客户端（客户端是有业务类型分类的）；
	short m_business_type;
	// 客户端唯一标识
	int m_app_id;
	// 消息数据
	string m_data;

};


//序列化方法
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

