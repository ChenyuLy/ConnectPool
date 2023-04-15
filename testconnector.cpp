#include <iostream>
#include <memory>
#include "Connector.h"

using namespace std;
int query()
{
    Connector conn;
    bool res = conn.connect("root", "123", "testdb", "127.0.0.1", 3306);
    cout << "connect : " << res << endl;
    string sql = "insert into person values(2,'czy',25);";
    res = conn.updata(sql);
    cout << "res: " << res << endl;

    sql = "select * from person";
    conn.query(sql);
    while (conn.next())
    {
        cout << conn.value(0) << ", " << conn.value(1) << ", " << conn.value(2) << endl;
    }

    // cout<<"----------------";
    return 0;
}

int main(int argc, char const *argv[])
{
    query();
    return 0;
}


