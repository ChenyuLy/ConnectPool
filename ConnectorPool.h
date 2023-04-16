#include "Connector.h"
#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>

class ConnectorPool
{
private:
    std::string m_ip;
    std::string m_user;
    std::string m_pd;
    std::string m_dbName;
    unsigned short m_port;
    int m_minSize;
    int m_maxSize;
    int m_timeout;
    int m_maxIdleTime;

    bool parseJsonFile();
    ConnectorPool();
    void addConnection();

    std::mutex m_mutexQ;
    std::condition_variable m_cond;
    std::queue<Connector *> m_connQ;

    void produceConnection();
    void recycleConnection();

public:
    ~ConnectorPool();
    ConnectorPool(const ConnectorPool &) = delete;            // 删除拷贝构造函数
    ConnectorPool(ConnectorPool &&) = delete;                 // 删除移动构造汉函数
    ConnectorPool &operator=(const ConnectorPool &) = delete; // 删除拷贝构造
    ConnectorPool &operator=(ConnectorPool &&) = delete;      // 移动拷贝函数

    ConnectorPool &operator=(const ConnectorPool &obj) = delete; // 删除赋值构造函数
};
