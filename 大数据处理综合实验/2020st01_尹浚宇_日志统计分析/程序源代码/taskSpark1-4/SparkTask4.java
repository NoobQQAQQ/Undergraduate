import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapred.lib.MultipleTextOutputFormat;
import org.apache.spark.api.java.*;
import org.apache.spark.api.java.function.Function;
import org.apache.spark.api.java.function.Function2;
import org.apache.spark.api.java.function.PairFunction;
import org.apache.spark.SparkConf;
import scala.Tuple2;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class SparkTask4 {
    public static class RDDOutputFormat extends MultipleTextOutputFormat<String, String> {
        public String generateFileNameForKeyValue(String key, String value, String name) {
            return key+".txt";
        }
        public String generateActualKey(String key, String value) {
            return null;
        }
    }
    public static void task4(JavaRDD<String> lines, String output, JavaSparkContext jsc) {
        JavaPairRDD<Tuple2<String, Integer>, Tuple2<Integer, Integer>> url_hour_resTime= lines.mapToPair(
                line -> {
                    String[] tokens = line.split(" ");
                    String url = tokens[4].replaceAll("/", "-").replaceFirst("-", "");
                    Integer hour = Integer.parseInt(tokens[1].split(":")[1]);
                    Integer time = Integer.parseInt(tokens[9]);
                    return new Tuple2<Tuple2<String, Integer>, Tuple2<Integer, Integer>>(new Tuple2<String, Integer>(url, hour), new Tuple2<Integer, Integer>(time, 1));
                }
        ).reduceByKey(
                (v1, v2) -> new Tuple2<Integer, Integer>(v1._1+v2._1, v1._2+v2._2)
        );

        JavaPairRDD<String, String> url_resTime = url_hour_resTime.mapToPair(
                kv -> new Tuple2<String, Tuple2<Integer, Integer>>(kv._1._1, kv._2)
        ).reduceByKey(
                (v1, v2) -> new Tuple2<Integer, Integer>(v1._1+v2._1, v1._2+v2._2)
        ).mapToPair(
                kv -> {
                    int x = kv._2._1, y = kv._2._2;
                    String resTime = String.format("%.2f", (double)x / (double)y);
                    return new Tuple2<String, String>(kv._1, resTime);
                }
        );
        JavaPairRDD<String, String> url_hourResTime = url_hour_resTime.mapToPair(
                kv -> {
                    String url = kv._1._1;
                    Integer hour = kv._1._2;
                    int x = kv._2._1, y = kv._2._2;
                    double resTime = (double)x / (double)y;
                    String hourResTime = String.format("%d:00-%d:00 %.2f", hour, (hour+1)%24, resTime);
                    return new Tuple2<String, String>(url, hourResTime);
                }
        );

        JavaPairRDD<String, String> finalRes = jsc.parallelize(
                url_resTime.union(url_hourResTime).collect()
        ).sortBy(
                kv -> {
                    String[] tokens = kv._2.split(":");
                    if(tokens.length == 1)
                        return kv._1;
                    else
                        return kv._1+String.valueOf((char)(Integer.parseInt(tokens[0])));
                }, true, 1
        ).mapToPair(
                x -> new Tuple2<String, String>(x._1,x._2)
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
            System.out.println("task4 FS error");
        }
        finalRes.saveAsHadoopFile(output, String.class, String.class, RDDOutputFormat.class);
    }
}