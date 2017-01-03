#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <map>
#include <vector>

#include "worker.h"

namespace homework
{

class Server
{
public:
    friend class Test;

    Server() {}

    ~Server() {}
    
    bool UpdateTable(const std::string& tableName, const std::string& content);
    
protected:
    std::map<size_t, WorkerPtr> mWorkers;
    std::map<std::string, std::vector<size_t> > mTables;
};
}

#endif
