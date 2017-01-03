#include<vector>
#include <cstdio>
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
    long l_content = content.size();
    const long BLOCK_SIZE = 128l*1024l*1024l;
    while(l_content> 0)
    {
         string _id = GetFileId();
         if(!CreateFile(_id))
             return false;
         int begin = ivector.size() * BLOCK_SIZE,end=ivector.size() * BLOCK_SIZE;
         if( content.size() - ivector.size() * BLOCK_SIZE < BLOCK_SIZE)
         {
             end += content.size() - ivector.size() * BLOCK_SIZE;
             l_content -= content.size() - ivector.size() * BLOCK_SIZE;
         }
         else
         {
             end +=  BLOCK_SIZE;
             l_content -= BLOCK_SIZE;
         }
         string s_copy(content.begin()+begin, content.begin()+end);
         if(!WriteToFile(_id,s_copy))
             return false;
         ivector.push_back(_id);
    }
    vector<string> _ = mTableFiles[tableName];
    mTableFiles[tableName].swap(ivector);
    for(size_t i=0; i < _.size(); ++i)
    {
        DeleteFile(_[i]);
    }
    return true;

}
