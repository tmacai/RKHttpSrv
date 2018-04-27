#include "RKHttpSrv.h"  

CRKHttpSrv::handler_map CRKHttpSrv::m_sHandlers;  
CRKHttpSrv::connection_map CRKHttpSrv::m_sConnections;  

void CRKHttpSrv::Init(uint32_t port)
{
	memset(m_szPort, 0, sizeof(m_szPort));  
	snprintf(m_szPort, sizeof(m_szPort), "%u", port);  
}  

bool CRKHttpSrv::Start()
{
	mg_mgr_init(&m_tMgr, NULL);  
	auto nc = mg_bind(&m_tMgr, m_szPort, EvHandler);  

	if (NULL == nc)
	{
		return false;
	}

	mg_set_protocol_http_websocket(nc); 

	//s_http_server_opts.document_root = "./www/";  // Serve current directory
	//s_http_server_opts.enable_directory_listing = "no";    //Set if show dir
	for (;;)
	{
		mg_mgr_poll(&m_tMgr, 1000);
	}
	mg_mgr_free(&m_tMgr);

	return true;  
}  

bool CRKHttpSrv::Close()
{
	mg_mgr_free(&m_tMgr);  

	return true;  
}  

bool CRKHttpSrv::RegisterHandler(std::string uri, handler f)
{
	auto it = m_sHandlers.find(uri);  
	if (m_sHandlers.end() != it)
	{
		return false;
	}

	return m_sHandlers.emplace(uri, f).second;  
}  

void CRKHttpSrv::UnRegisterHandler(std::string uri)
{
	auto it = m_sHandlers.find(uri); 
	if (m_sHandlers.end() != it)
	{
		m_sHandlers.erase(it);
	}
}  

void CRKHttpSrv::Loop(int milli)
{
	mg_mgr_poll(&m_tMgr, milli);  
}  

void CRKHttpSrv::EvHandler(struct mg_connection* nc, int ev, void* ev_data)
{
	switch(ev)
	{
	case MG_EV_HTTP_REQUEST:
	{
		HandleRequst(nc, ev, ev_data);
		break;
	}
	default:
		break;
	}  
}  

void CRKHttpSrv::HandleRequst(struct mg_connection *nc, int ev, void* ev_data)
{
	http_message* hm = (http_message*)ev_data;  
	std::string uri(hm->uri.p, hm->uri.len);  

	auto it = m_sHandlers.find(uri);  
	if(m_sHandlers.end() == it)  
		return;  

	m_sConnections.emplace(uri, nc);  
	it->second(std::string(hm->query_string.p, hm->query_string.len),  
		std::string(hm->body.p, hm->body.len));  
}  

void CRKHttpSrv::SendReply(std::string uri, std::string reply)
{  
	auto range = m_sConnections.equal_range(uri);  
	if(range.first == range.second)  
		return;  

	auto it = range.first;  
	mg_printf(it->second, "HTTP/1.1 200 OK\r\niConnection: close\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s\r\n",   
		(uint32_t)reply.length(), reply.c_str());  

	it->second->flags |= MG_F_SEND_AND_CLOSE;  
	m_sConnections.erase(it);  
}  

void CRKHttpSrv::SendError(std::string uri, int errcode, std::string reply)
{  
	auto range = m_sConnections.equal_range(uri);  
	if(range.first == range.second)  
		return;  

	auto it = range.first;  
	mg_printf(it->second, "HTTP/1.1 %d %s\r\n", errcode, reply.c_str());  

	it->second->flags |= MG_F_SEND_AND_CLOSE;
	m_sConnections.erase(it);  
}