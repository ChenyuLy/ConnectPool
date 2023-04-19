#include "Connector.h"
#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

template<typename T>
class SafeQueue
{
private:
    std::mutex m_mutex ;

    std::queue<T> m_queue;

public:
    SafeQueue(){};
    ~SafeQueue(){};


    int size(){
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }


    bool empty(){
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }


    void push(T &t){
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(t);
    }

    bool pop(T &t){
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_queue.empty()){
            return false;
        }
        t = std::move( m_queue.front());
        m_queue.pop();
        return true;
    }

    T & operator[](int i) const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue[i];
    }
};



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
    bool is_shutdown;

    bool parseJsonFile();
    ConnectorPool();
    void addConnection();
    void produceConnection();
    void recycleConnection();

    std::mutex m_mutexQ;
    std::condition_variable m_condp;
    std::condition_variable m_condr;
    std::condition_variable m_condt;
    // std::queue<Connector *> m_connQ;
    SafeQueue<Connector *> m_connQ;



    

public:
    ~ConnectorPool();
    ConnectorPool(const ConnectorPool &) = delete;            // 删除拷贝构造函数
    ConnectorPool(ConnectorPool &&) = delete;                 // 删除移动构造汉函数
    ConnectorPool &operator=(const ConnectorPool &) = delete; // 删除拷贝构造
    ConnectorPool &operator=(ConnectorPool &&) = delete;      // 移动拷贝函数
    
    static ConnectorPool* GetPool();
    std::shared_ptr<Connector> getConnector();


    
};
