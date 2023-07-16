import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapred.lib.MultipleTextOutputFormat;
import org.apache.spark.api.java.*;

import scala.Tuple2;

public class SparkTask2 {
    public static class RDDMultipleTextOutputFormat extends MultipleTextOutputFormat<String, String> {
        public String generateFileNameForKeyValue(String key, String value, String name) {
            return key+".txt";
        }
        public String generateActualKey(String key, String value) {
            return null;
        }
    }
    public static void task2(JavaRDD<String> lines, String output, JavaSparkContext jsc) {
        //按时间窗统计ip频次
        JavaPairRDD<String, String> ip_hourCnt = lines.mapToPair(
                line -> {
                    String[] tokens = line.split(" ");
                    return new Tuple2<Tuple2<String, Integer>, Integer>(new Tuple2<>(tokens[0], Integer.parseInt(tokens[1].split(":")[1])), 1);
                    // <<ip, hour>, 1>
                }
        ).reduceByKey(
                (v1,v2) -> v1+v2
        ).mapToPair(
                kv -> {
                    int hour = kv._1._2;
                    return new Tuple2<String, String>(kv._1._1, String.format("%d:00-%d:00 %d", hour, (hour+1)%24, kv._2.intValue()));
                    // <ip, "5:00-6:00 123">
                }
        );
        //统计总的ip频次
        JavaPairRDD<String, String> ip_cnt = ip_hourCnt.mapToPair(
                kv -> new Tuple2<String, String>(kv._1, kv._2.split(" ")[1])
        ).reduceByKey(
                (v1, v2) -> ((Integer)(Integer.parseInt(v1) + Integer.parseInt(v2))).toString()
        );

        JavaPairRDD<String, String> res = jsc.parallelize(
                ip_hourCnt.union(ip_cnt).collect()
        ).sortBy(
                data -> {
                    String[] tokens = data._2.split(":");
                    if(tokens.length == 1)
                        return data._1;
                    else
                        return data._1+String.valueOf((char)(Integer.parseInt(tokens[0])));

                }, true, 1
        ).mapToPair( //上述数据不是键值对, 而是二元组
                data -> new Tuple2<String, String>(data._1,data._2)
        );

        Configuration conf = new Configuration();
        try {
            Path outputPath = new Path(output);
            FileSystem hdfs = FileSystem.get(conf);
            //删除已有输出文件
            if (hdfs.exists(outputPath)) {
                hdfs.delete(outputPath, true);
            }
        } catch (Exception e) {
            System.out.println("task2 FS error");
        }
        res.saveAsHadoopFile(output, String.class, String.class, RDDMultipleTextOutputFormat.class);
    }
}

