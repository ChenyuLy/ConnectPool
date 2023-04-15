#include "Connector.h"

void Connector::freeResult()
{
    if(m_res != nullptr){
        mysql_free_result(m_res);
    }
    m_res = nullptr;
}

void Connector::error_end(std::string info)
{
    fprintf(stderr, "Failed to %s to database: Error: %s\n",info.c_str(),mysql_error(m_conn));
}

Connector::Connector()
{
    m_conn = mysql_init(nullptr);
    mysql_set_character_set(m_conn,"utf8");
}

Connector::~Connector()
{
    if(m_conn != nullptr){
        mysql_close(m_conn);
    }
    freeResult();
}

bool Connector::connect(std::string user,std::string passwd,std::string dbName,std::string ip, unsigned short port)
{

    if(!mysql_real_connect(m_conn,ip.c_str(),user.c_str(),passwd.c_str(),dbName.c_str(),port,NULL,0)){
        error_end(std::string("connect"));
        return false;
    }

    return true;
}

bool Connector::updata(std::string sql)
{
    if (mysql_query(m_conn,sql.c_str()))
    {
        error_end(std::string("updata"));
        return false;
    }

    return true;
}

bool Connector::query(std::string sql)
{
    if (mysql_query(m_conn,sql.c_str()))
    {
        error_end(std::string("query"));
        return false;
    }
    m_res = mysql_store_result(m_conn);
    return true;
}

bool Connector::next()
{

    if(m_res!= nullptr){
        m_row = mysql_fetch_row(m_res);
        if(m_row != nullptr)
        return true;    
    }
    return false; //没有数据了
}

std::string Connector::value(int index)
{
    int fieldsnums = mysql_num_fields(m_res);
    if(index >= fieldsnums || index<0) return std::string();

    char* val = m_row[index];
    unsigned long len = mysql_fetch_lengths(m_res)[index];

    return std::string(val,len);
}

bool Connector::transaction()
{
    return mysql_autocommit(m_conn,false);
}

bool Connector::commit()
{
    return mysql_commit(m_conn);
}

bool Connector::rollback()
{
    return mysql_rollback(m_conn);
}

void Connector::refreshAlivetime()
{
    m_alivetime = std::chrono::steady_clock::now();
}

long long Connector::getAliveTime()
{
    std::chrono::nanoseconds res = std::chrono::steady_clock::now() - m_alivetime;
    std::chrono::milliseconds  millsec = std::chrono::duration_cast<std::chrono::milliseconds>(res);

    return millsec.count();
}


