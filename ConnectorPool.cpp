#include "ConnectorPool.h"
#include <json/json.h>
#include <fstream>
#include <thread>

bool ConnectorPool::parseJsonFile()
{
    std::ifstream ifs("dbconf.json");
    std::ifstream ifs("dbconf.json");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs, root);

    if (root.isObject())
    {
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_pd = root["password"].asString();
        m_dbName = root["ipName"].asString();
        m_minSize = root["minSize"].asInt();
        m_maxSize = root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        m_timeout = root["timeout"].asInt();
        return true;
    }

    return false;
}

ConnectorPool::ConnectorPool()
{
    if(!parseJsonFile()){
        printf("error resding JsonFile\n");
        return;
    }

    //初始化连接
    for(int i = 0; i < m_minSize; i++){
        addConnection();
    }

    std::thread producer(&ConnectorPool::produceConnection,this);
    std::thread recycler(&ConnectorPool::recycleConnection,this);
    producer.detach();
    recycler.detach();
}


void ConnectorPool::addConnection()
{
    Connector* conn = new Connector;
    conn->connect(m_user,m_pd,m_dbName,m_ip,m_port);
    conn->refreshAlivetime();
    m_connQ.push(conn);
}

void ConnectorPool::produceConnection()
{
}

ConnectorPool::~ConnectorPool()
{
    while (m_connQ.empty())
    {
        Connector * conn = m_connQ.front();
        m_connQ.pop();
        delete conn;
    }
}
