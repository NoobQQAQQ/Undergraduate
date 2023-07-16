import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapred.TextOutputFormat;
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

public class SparkTask3 {
    public static void task3(JavaRDD<String> lines, String output, JavaSparkContext jsc) {
        JavaPairRDD<String, Integer> url_cnt = lines.mapToPair(
                line -> new Tuple2<String, Integer>(line.split(" ")[4], 1)
        ).reduceByKey(
                (v1, v2) -> v1 + v2
        ).map( // 键值对不能使用sortBy方法
                kv -> new Tuple2<String, Integer>(kv._1, kv._2)
        ).sortBy(
                tuple -> tuple._2, false, 1
        ).mapToPair(
                tuple -> new Tuple2(tuple._1, tuple._2)
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
            System.out.println("task3 FS error");
        }
        url_cnt.saveAsHadoopFile(output, String.class, Integer.class, TextOutputFormat.class);
    }
}
