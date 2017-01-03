package example;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.HashMap;
import com.aliyun.odps.data.Record;
import com.aliyun.odps.data.TableInfo;
import com.aliyun.odps.mapred.JobClient;
import com.aliyun.odps.mapred.MapperBase;
import com.aliyun.odps.mapred.ReducerBase;
import com.aliyun.odps.mapred.conf.JobConf;
import com.aliyun.odps.mapred.utils.InputUtils;
import com.aliyun.odps.mapred.utils.OutputUtils;
import com.aliyun.odps.mapred.utils.SchemaUtils;
/*
* Implement ODPS MapReduce to get query result as below:
* INSERT OVERWRITE TABLE join_out
* SELECT c.customer_name, o.order_id
* FROM customers s LEFT OUTER JOIN orders o
* ON c.customer_id = o.customer_id AND c.country = 'UK';
*
* Input table schema:
* customers (customer_id string, customer_name string, address string, city string, country string)
* orders (order_id bigint, customer_id string, employee_id string, order_date string)
* Output table schema:
* join_out (customer_name string, order_id bigint)
*/
public class LeftOuterJoin {
    public static class LeftOuterJoinMapper extends MapperBase {
         private Record mapKey;
         private Record mapValue;
         private long tag;

        @Override
        public void setup(TaskContext context) throws IOException {
          // mapKey: customer_id, tag
          mapKey = context.createMapOutputKeyRecord();
          // mapValue: order_id, customer_name
          mapValue = context.createMapOutputValueRecord();
          tag = context.getInputTableInfo().getLabel().equals("left") ? 0 : 1;
        }
         @Override
        public void map(long key, Record record, TaskContext context) throws IOException {
            if(tag==0)
            {
             mapKey.set(0, record.get(0));  //  cid
             mapValue.set(0, record.get(1));//  c.name
             mapValue.set(1, record.get(4));//  c.country
            }
            else
            {
             mapKey.set(0, record.get(1));//   cid
             mapValue.set(2, record.getBigint(0));// o.id
            }
            mapKey.set(1, tag);          //   tag
            context.write(mapKey, mapValue);
        }
    }
    public static class LeftOuterJoinReducer extends ReducerBase {
        private Record result = null;
        Map<String,List<Object[]>> leftValues = null;
        @Override
        public void setup(TaskContext context) throws IOException {
           result = context.createOutputRecord();
           leftValues = new HashMap<String,List<Object[]>>();
        }
        @Override
        public void reduce(Record key, Iterator<Record> values, TaskContext context) throws
        IOException {

            while (values.hasNext()) {
              Record value = values.next();
              long tag = (Long) key.get(1);
              if (tag == 0) {
                if(leftValues.get(key.getString(0))==null)
                  leftValues.put(key.getString(0),new ArrayList<Object[]>());
                else
                  leftValues.get(key.getString(0)).add(value.toArray().clone());
              } else {
                for (Object[] leftValue : leftValues.get(key.getString(0))){
                  int index = 0;
                  result.set(index++, leftValue[0]);
                  if(leftValue[1].equals("UK"))
                      result.set(index++, value.getBigint(2));
                  context.write(result);  
                }
                leftValues.remove(key.getString(0));
              }
            }
        }
    }

    public static void main(String[] args)
        throws Exception {
        if (args.length != 3) {
        System.err.println("Usage: LeftOuterJoin <customers> <orders> <join_out>");
        System.exit(-1);
        }
        JobConf job = new JobConf();
        job.setMapperClass(LeftOuterJoinMapper.class);
        // job.setCombinerClass(LeftOuterJoinCombiner.class);
        job.setReducerClass(LeftOuterJoinReducer.class);
        job.setMapOutputKeySchema(SchemaUtils.fromString("customer_id:string,tag:bigint"));
        job.setMapOutputValueSchema(SchemaUtils.fromString("customer_name:string,country:string,order_id:bigint"));
        job.setPartitionColumns(new String[] {"customer_id"});
        job.setOutputKeySortColumns(new String[] {"customer_id","tag"});
        job.setOutputGroupingColumns(new String[] {"customer_id"});
        job.setNumReduceTasks(1);
        InputUtils.addTable(TableInfo.builder().tableName(args[0]).label("left").build(), job);
        InputUtils.addTable(TableInfo.builder().tableName(args[1]).label("right").build(), job);
        OutputUtils.addTable(TableInfo.builder().tableName(args[2]).build(), job);
        JobClient.runJob(job);
    }
}
