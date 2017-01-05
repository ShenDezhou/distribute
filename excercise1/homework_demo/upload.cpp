#include "server.h"


using namespace std;
using namespace homework;
////
//1.检查更新模式mode
//2.|-创建模式不保存原纪录，|-更新模式保存原纪录roll---content
//3.|-循环所有工人，保存工号，工人更新内容
//  3.a|--第x个工人更新失败:|-创建模式删除表，|-更新模式将roll---content更新回原表
//  3.b|--保存会滚成功的工人列表。
//4.|-成功了保存（表，工号列表），失败不保存。
////更新失败返回false，更新成功返回true
bool Server::UpdateTable(const string& tableName, const string& content)
{    
    printf("enter server update table\n");
    bool result = true;
    vector<size_t> table_workers;
    for(map<size_t, WorkerPtr>::iterator worker_it = mWorkers.begin(); worker_it!= mWorkers.end(); ++worker_it)
    {
        if(!worker_it->second->UpdateTable(tableName,content))
        {
            result = false;
            break;
        }
        
    }
    if(result)
    {
        for(map<size_t, WorkerPtr>::iterator worker_it = mWorkers.begin(); worker_it!= mWorkers.end(); ++worker_it)
        {
            if(!worker_it->second->CommitTable(tableName))
            {
                result = false;
                break;
            }
            else
                table_workers.push_back(worker_it->first);          
        }   
        result = true;
        mTables[tableName] = table_workers; 
    }
    else
    {
        for(map<size_t, WorkerPtr>::iterator worker_it = mWorkers.begin(); worker_it!= mWorkers.end(); ++worker_it)
            worker_it->second->RollbackTable(tableName);
        result = false;
    }

    printf("exit server update table\n");
    return result;
}
////
//1.更新模式mode
//2.创建模式不保存文件列表，更新模式保存原文件列表
//3.计算所需文件数量
//4.循环创建文件，保存块，在文件列表中保存文件id。
//  4.a创建文件失败，循环将已创建文件删除，
//  4.b写文件失败，同4.a
//5.在表列表中保存文件列表list（文件id）,更新模式删除旧列表中文件
////成功返回true，创建或者写文件失败返回false
bool Worker::UpdateTable(const string& tableName, const string& content)
{
    printf("enter worker update table\n");
    bool result = true;
    map<string, vector<string> >::iterator it;
    // multiple update?
    if ( (it = mTableFilesTmpFilesToCommit.find(tableName) ) != mTableFilesTmpFilesToCommit.end()) {
        return false;
    }
    
    vector<string> files_list;
    size_t files_num = ceil((float)content.size() / BLOCK_SIZE);
    for (size_t i = 0; i < files_num; ++i)
    {
        string _id = GetFileId();

        if(!CreateFile(_id))
        {
            result = false;
            break;
        }

        if(!WriteToFile(_id,string(content.begin()+i * BLOCK_SIZE,content.begin()+min((i+1)*BLOCK_SIZE,content.size()))))
        {
            DeleteFile(_id);
            //写失败，将此次生成文件列表全部删除。
            for(size_t i = 0; i < files_list.size(); ++i)
            {
                DeleteFile(files_list[i]);
            }
            result = false;
            break;
        }else
            files_list.push_back(_id);
    }
    if(result)
        mTableFilesTmpFilesToCommit[tableName] = files_list;

    printf("exit worker update table\n");
    return result;

}