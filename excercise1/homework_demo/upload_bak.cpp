#include "server.h"



using namespace std;
using namespace homework;

bool Server::UpdateTable(const string& tableName, const string& content)
{
      printf("enter server update table\n");
      string _content;
      map<size_t, WorkerPtr>::iterator it;
      vector<size_t> _iv;
      for(it = mWorkers.begin(); it!= mWorkers.end(); ++it)
      {
         if(_content.size()==0)
             it->second->ReadTable(tableName,_content);
         _iv.push_back(it->first);
          if(!it->second->UpdateTable(tableName,content))
          {
               map<size_t, WorkerPtr>::iterator rb_it;
               for(rb_it=mWorkers.begin();rb_it!=it; ++ rb_it)
               {
                   if(!rb_it->second->UpdateTable(tableName,_content))
                   {
                         printf("worker: %lu roll back failed",rb_it->first);
                         return false;
                   }
               }
               printf("roll back server update table");
               return false;
          }
      }
      mTables[tableName].swap(_iv);
      printf("exit server update table");
     return true;
}

bool Worker::UpdateTable(const string& tableName, const string& content)
{
    vector<string> ivector;
    const long BLOCK_SIZE = 128l*1024l*1024l;
    
    map<string, vector<string> >::iterator it;
    if ((it = mTableFiles.find(tableName)) == mTableFiles.end())
    {
        cerr << "Can not find table name " << tableName << " from worker " << ToString(mWorkerId) << " when read table" << endl;
        return false;
    }

    vector<string> old_files = it->second;
    vector<string> new_files;

    size_t files_num = math.ceil((long)content.size() / BLOCK_SIZE);
    size_t index = 0;
    for (size_t i = 0; i < files_num; ++i)
    {
        string _id = GetFileId();
        if(!CreateFile(_id))
            return false;
        size_t i_start = i * BLOCK_SIZE;
        size_t i_end = math.min(i*BLOCK_SIZE,content.size());
        string _(content.begin()+i_start,content.begin()+i_end);
        if(!WriteToFile(_id,_))
        {
          for(vector<string>::iterator delete_it = new_files.begin();delete_it!= new_files.end(); ++ delete_it)
          {
            DeleteFile(*delete_it);
          }
          return false;
        }else
          new_files.add(_id);
    }

    mTableFiles[tableName] = new_files;
    for(size_t i=0; i < old_files.size(); ++i)
    {
        DeleteFile(old_files[i]);
    }
    return true;

}
