#include <iostream>
#include <mysql/mysql.h>
#include <chrono>
#include<string>

class Connector
{
private:
    MYSQL * m_conn = nullptr;
    MYSQL_RES *m_res = nullptr;
    MYSQL_ROW m_row = nullptr;
    std::chrono::steady_clock::time_point m_alivetime;

    void freeResult();
    void error_end(std::string info);

public:
    //初始化连接
    Connector();
    //释放连接
    ~Connector();
    //连接数据库
    bool connect(std::string user,std::string passwd,std::string dbName,std::string ip, unsigned short port);
    //更新数据库
    bool updata(std::string sql);
    //查询数据库得到结果集
    bool query(std::string sql);
    //遍历查询结果集合
    bool next(); 
    //得到结果集中的字段
    std::string value(int index);
    //事物操作
    bool transaction();
    //事物提交
    bool commit();
    //事物回滚
    bool rollback();
    //刷新空闲时间
    void refreshAlivetime();
    //统计空闲总时长
    long long getAliveTime();
};


