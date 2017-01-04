#include "server.h"

using namespace std;
using namespace homework;

#include <stdlib.h>
#include <algorithm>

bool Server::UpdateTable(const string& tableName, const string& content)
{
    std::map<size_t, WorkerPtr>::iterator  it_workers;
    std::vector<size_t> worker_list;
    
    bool result = true;
    
    for (it_workers = mWorkers.begin(); it_workers != mWorkers.end(); ++it_workers) {
        result = it_workers->second->UpdateTable(tableName, content);
        
        if (result == false) {
            cout<<"worker "<<it_workers->first<<" UpdateTable fail"<<endl;
            
            break;
        }
    }
    
    if (result == false)
    {
        for (it_workers = mWorkers.begin(); it_workers != mWorkers.end(); ++it_workers)
        {
            result = it_workers->second->Rollback(tableName);
            
        }
        
        result = false;
    }
    else
    {
        for (it_workers = mWorkers.begin(); it_workers != mWorkers.end(); ++it_workers)
        {
            result = it_workers->second->Commit(tableName);
            
            if (result) {
                worker_list.push_back(it_workers->first);
            }
        }
        
        result = true;
        mTables[tableName] = worker_list;
    }
    
    return result;
}

bool Worker::UpdateTable(const string& tableName, const string& content)
{
    std::map<std::string, std::vector<std::string> >::iterator it;
    std::vector<std::string> tmpFileList;
      
      
    if ( (it = mTableFilesTmpToCommit.find(tableName) ) != mTableFilesTmpToCommit.end()) {
        return false;
    }

    int BlockSize = 128l*1024l*1024l;
    int fileNum = content.length()/BlockSize;
    if (content.length() % BlockSize > 0) fileNum++;
      
    for (size_t i=0; i< fileNum; i++) {
        
        std::string newFileId = GetFileId();
        
        if (CreateFile(newFileId) == false)
            return false;
        
        if (WriteToFile(newFileId,content.substr(i*BlockSize, BlockSize)) == false) {
            DeleteFile(newFileId);
            for (size_t j=0; j<tmpFileList.size();j++)
            {
                DeleteFile(tmpFileList[j]);
            }
            
            return false;
        }
        
        tmpFileList.push_back(newFileId);
    }
    
    mTableFilesTmpToCommit[tableName] = tmpFileList;
    
    return true;
}


bool Worker::Commit(const std::string& tableName){
    
    std::map<std::string, std::vector<std::string> >::iterator it_new, it_old;
    
    it_new = mTableFilesTmpToCommit.find(tableName);
    it_old = mTableFiles.find(tableName);
    
    if ( (it_new == mTableFilesTmpToCommit.end()) && (it_old == mTableFiles.end())) {
        return true;
    }
    
    std::vector<std::string> files;
    if ( it_old != mTableFiles.end()) {
        files = it_old->second;
    }
    
    if (it_new != mTableFilesTmpToCommit.end() ) {
        mTableFiles[tableName] = it_new->second;
        mTableFilesTmpToCommit.erase(it_new);
    }
    
    for (size_t i=0; i<files.size(); i++) {
        DeleteFile(files[i]);
    }
    return true;
    
}


bool Worker::Rollback(const std::string& tableName){
    
    std::map<std::string, std::vector<std::string> >::iterator it_new;
    it_new = mTableFilesTmpToCommit.find(tableName);
    
    if ( it_new == mTableFilesTmpToCommit.end()) {
        return true;
    }
    
    std::vector<std::string> files = it_new->second;
    mTableFilesTmpToCommit.erase(it_new);
    
    for (size_t i=0; i<files.size(); i++) {
        DeleteFile(files[i]);
    }
    
    return true;
}

