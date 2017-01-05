#ifndef WORKER_H
#define WORKER_H

#include <iostream>
#include <map>
#include <vector>

namespace homework
{
const long BLOCK_SIZE = 128l*1024l*1024l;

class Worker
{
public:
    Worker(size_t workerId)
    : mWorkerId(workerId)
    {}

    bool UpdateTable(const std::string& tableName, const std::string& content);

    bool ReadTable(const std::string& tableName, std::string& content);

    bool CommitTable(const std::string& tableName);

    bool RollbackTable(const std::string& tableName);

    bool DeleteTable(const std::string& tableName);
protected:
    static std::string GetFileId();
    
    virtual bool CreateFile(const std::string& fileId);

    virtual bool DeleteFile(const std::string& fileId);

    virtual bool RenameFile(const std::string& oriFileId, const std::string& newFileId);

    virtual bool WriteToFile(const std::string& fileId, const std::string& content);

    virtual bool ReadFile(const std::string& fileId, std::string& content);    

    size_t mWorkerId;
    std::map<std::string, std::vector<std::string> > mTableFiles;
    std::map<std::string, std::string> mFiles;
    std::map<std::string, std::vector<std::string> > mTableFilesTmpFilesToCommit;

};
typedef Worker* WorkerPtr;

}
#endif

