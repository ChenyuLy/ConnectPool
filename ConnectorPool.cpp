#include "ConnectorPool.h"
#include <json/json.h>
#include <fstream>
#include <thread>

bool ConnectorPool::parseJsonFile()
{
    std::ifstream ifs("dbconf.json");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs, root);

    is_shutdown = false;

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
    if (!parseJsonFile())
    {
        printf("error resding JsonFile\n");
        return;
    }

    // 初始化连接
    for (int i = 0; i < m_minSize; i++)
    {
        addConnection();
    }

    std::thread producer(&ConnectorPool::produceConnection, this);
    std::thread recycler(&ConnectorPool::recycleConnection, this);
    producer.detach();
    recycler.detach();
}

void ConnectorPool::addConnection()
{
    Connector *conn = new Connector;
    conn->connect(m_user, m_pd, m_dbName, m_ip, m_port);
    conn->refreshAlivetime();
    m_connQ.push(conn);
}

void ConnectorPool::produceConnection()
{
    while (!is_shutdown)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutexQ);
            while (m_connQ.size() >= m_minSize && !is_shutdown)
            {
                m_condp.wait(lock);
            }
        }
        if (!is_shutdown)
            addConnection();
    }
    std::cout<<"增加线程结束"<<std::endl;
}

void ConnectorPool::recycleConnection()
{

    while (!is_shutdown)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutexQ);
            while (m_connQ.size() < m_maxSize && !is_shutdown)
            {
                m_condr.wait(lock);
            }
        }

        if (!is_shutdown)
        {
            Connector *conn = nullptr;
            if (!m_connQ.pop(conn))
            {
                std::cout << "reduce err" << std::endl;
            }
            delete conn;
        }
    }


    std::cout<<"削减线程结束"<<std::endl;
}

ConnectorPool::~ConnectorPool()
{
    is_shutdown = true;
    m_condp.notify_all();
    m_condr.notify_all();

    while (m_connQ.empty())
    {
        Connector *conn = nullptr;
        if (!m_connQ.pop(conn))
        {
            std::cout << "pop err" << std::endl;
        }
        delete conn;
    }
}

ConnectorPool *ConnectorPool::GetPool()
{
    static ConnectorPool pool;

    return &pool;
}

std::shared_ptr<Connector> ConnectorPool::getConnector()
{
    bool isdequeue;
    while (m_connQ.empty())
    {
        std::unique_lock<std::mutex> lock(m_mutexQ);
        m_condt.wait_for(lock, std::chrono::milliseconds(m_timeout));
    }
    Connector *conn1 = nullptr;
    if (!m_connQ.pop(conn1))
    {
        printf("get pop err \n");
        return nullptr;
    }
    std::shared_ptr<Connector> connptr(conn1, [this](Connector *conn)
                                       {
        conn->refreshAlivetime();
        this->m_connQ.push(conn);
        this->m_condr.notify_one(); });

    m_condp.notify_one();

    return connptr;
}
