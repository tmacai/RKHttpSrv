/************************************************************************
** Copyright(c)2016 Ropeok All Right Reserved.
** 名称 ：RKHttpSrv.h
** 作者 ：胡贞财(zhencai.hu@ropeok.com)
** 创建 ：2017年05月23日 11:05:33
** 版本 ：V 1.0.0
** 描述 ：利用mongoose实现http服务
** 修改 ：
** 备注 ：
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