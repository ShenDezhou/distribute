#ifndef TEST_WORKER_H
#define TEST_WORKER_H

#include "worker.h"
#include <fstream>
#include <vector>

namespace homework
{

class TestWorker : public Worker
{    
public:
    TestWorker(size_t workerId) : Worker(workerId)
    {}

private:    
    bool CreateFile(const std::string& fileId)
    {
        return false;
    }
};

class TestWorkerWrite : public Worker
{
public:
    TestWorkerWrite(size_t workerId) : Worker(workerId)
    {}

private:
    bool WriteToFile(const std::string& fileId, const std::string& content)
    {
        return false;
    }
};
}
#endif
