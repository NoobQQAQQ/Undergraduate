import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.spark.api.java.*;
import scala.Tuple2;
import java.io.IOException;
import java.util.List;

public class SparkTask1 {
    public static void task1(JavaRDD<String> lines, String output, JavaSparkContext jsc) {
        //统计状态码
        JavaPairRDD<String, Integer> stateCode_sum = lines.mapToPair(
                x -> new Tuple2<String, Integer>(x.split(" ")[7], 1)
        ).reduceByKey(
                (v1,v2) -> v1+v2
        );
        //分时间窗统计状态码, 按照时间窗划分, 再按时间排序
        JavaPairRDD<Integer, Iterable<Tuple2<String, Integer>>> stateCode_hourSum = lines.mapToPair(
                line -> {
                    String[] tokens = line.split(" ");
                    int hour = Integer.parseInt(tokens[1].split(":")[1]);
                    return new Tuple2<Tuple2<Integer, String>, Integer>(new Tuple2<>(hour, tokens[7]), 1); // <<hour, code>, 1>
                }
        ).reduceByKey(
                (v1,v2) -> v1+v2
        ).mapToPair(
                kv -> new Tuple2<Integer, Tuple2<String, Integer>>(kv._1._1, new Tuple2<>(kv._1._2, kv._2))
        ).groupByKey().sortByKey();

        Configuration conf = new Configuration();
        Task1WriteFinal writer = new Task1WriteFinal(output, conf, stateCode_sum, stateCode_hourSum);
        writer.write();
    }
}

class Task1WriteFinal {
    private String outputPath;
    private FileSystem fs;
    private List<Tuple2<String, Integer>> out1;
    private List<Tuple2<Integer, Iterable<Tuple2<String, Integer>>>> out2;
    public Task1WriteFinal(String output, Configuration conf, JavaPairRDD<String, Integer> in1, JavaPairRDD<Integer, Iterable<Tuple2<String, Integer>>> in2) {
        outputPath = output;
        try {
            fs = FileSystem.get(conf);
        } catch (IOException e) {
            System.out.println("IO failed in Task1WriteFinal");
        }
        out1 = in1.collect();
        out2 = in2.collect();
    }
    public void write() {
        try {
            //检查当前路径是否存在, 若存在则需要删除
            if(fs.exists(new Path(outputPath)))
                fs.delete(new Path(outputPath), true);
            FSDataOutputStream outputStream = fs.create(new Path(outputPath+"/1.txt"));
            //写入总的统计信息
            int sum1 = 0, sum2 = 0, sum3 = 0;
            for(Tuple2<String, Integer> i : out1) {
                String code = i._1;
                if(code.equals("200"))
                    sum1 += i._2;
                else if(code.equals("400"))
                    sum2 += i._2;
                else
                    sum3 += i._2;
            }
            String str = String.format("200:%d\n400:%d\n404:%d\n",sum1, sum2, sum3);
            outputStream.write(str.getBytes());
            //写入按时间划分的统计信息
            for(Tuple2<Integer, Iterable<Tuple2<String, Integer>>> i : out2) {
                int hour = i._1;
                sum1 = 0; sum2 = 0; sum3 = 0;
                for(Tuple2<String, Integer> j : i._2) {
                    if(j._1.equals("200"))
                        sum1 += j._2;
                    else if(j._1.equals("400"))
                        sum2 += j._2;
                    else
                        sum3 += j._2;
                }
                String outKey = String.format("%d:00-%d:00 200:%d 400:%d 404:%d\n",hour, (hour+1)%24, sum1, sum2, sum3);
                outputStream.write(outKey.getBytes());
            }
            outputStream.close();
        } catch (IOException e) {
            System.out.println("write failed");
            e.printStackTrace();
        }
    }
}

