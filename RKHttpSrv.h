/************************************************************************
** Copyright(c)2016 Ropeok All Right Reserved.
** ���� ��RKHttpSrv.h
** ���� �������(zhencai.hu@ropeok.com)
** ���� ��2017��05��23�� 11:05:33
** �汾 ��V 1.0.0
** ���� ������mongooseʵ��http����
** �޸� ��
** ��ע ��
************************************************************************/
#pragma once  

#include "mongoose.h"  
#include <map>  
#include <string>  
#include <functional>

class CRKHttpSrv
{
public:  
	using handler = std::function<void(std::string, std::string)>;  

public:  
	virtual ~CRKHttpSrv(){};

	void Init(uint32_t port);  
	bool Start();  
	bool Close();  
	bool RegisterHandler(std::string uri, handler f);  
	void UnRegisterHandler(std::string uri);  
	void Loop(int milli);  

	void SendReply(std::string uri, std::string reply);  
	void SendError(std::string uri, int errcode, std::string reply);  

protected:  
	using handler_map = std::map<std::string, handler>;  
	using connection_map = std::multimap<std::string, mg_connection*>;  

private:  
	static void EvHandler(struct mg_connection* nc, int ev, void* ev_data);  
	static void HandleRequst(struct mg_connection* nc, int ev, void *ev_data);   

public:  
	static handler_map		m_sHandlers;  
	static connection_map	m_sConnections;  

	char	m_szPort[11];  
	struct	mg_mgr m_tMgr;
};  