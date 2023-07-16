import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;

import org.apache.hadoop.fs.*;
import org.apache.hadoop.hdfs.DFSClient;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.lib.MultipleOutputFormat;
import org.apache.hadoop.mapred.lib.MultipleTextOutputFormat;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.*;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.partition.HashPartitioner;
import org.apache.hadoop.util.GenericOptionsParser;
import org.apache.hadoop.util.Progressable;

public class RMSE {
    private static final double M = 24.0;
    public static void rmse(String input, String output) {
        try {
            //通过xml文件获取配置信息
            Configuration conf = new Configuration();

            Path inputPath = new Path(input);
            Path outputPath = new Path(output);
            FileSystem hdfs = FileSystem.get(conf);
            //删除已有输出文件
            if(hdfs.exists(outputPath)) {
                hdfs.delete(outputPath, true);
            }

            Job job1 = Job.getInstance(conf, "task5RMSE");
            job1.setJarByClass(RMSE.class);

            //指定输入输出格式 路径
            job1.setInputFormatClass(TextInputFormat.class);
            FileInputFormat.addInputPath(job1, inputPath);
            job1.setOutputFormatClass(TextOutputFormat.class);
            FileOutputFormat.setOutputPath(job1, outputPath);

            job1.setMapperClass(RMSEMapper.class);
            job1.setPartitionerClass(RMSEPartitioner.class);
            job1.setReducerClass(RMSEReducer.class);

            job1.setMapOutputKeyClass(Text.class);
            job1.setMapOutputValueClass(Text.class);
            job1.setOutputKeyClass(Text.class);
            job1.setOutputValueClass(NullWritable.class);

            // 指定Reducer个数
            job1.setNumReduceTasks(24);

            //提交job1
            boolean flag = job1.waitForCompletion(true)?true:false;
            if(!flag){
                System.out.println("作业非正常结束");
                System.exit(1);
            }

            // 处理完成 遍历生成文件产生最后的RMSE
            double[] rmse = new double[8];
            Arrays.fill(rmse, 0.0);
            FileStatus[] files = hdfs.listStatus(outputPath);
            
            for(FileStatus file : files) {
                FSDataInputStream inputStream = hdfs.open(file.getPath());
                BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
                // assert 每个文件只有一行
                String line;
                if ((line = reader.readLine()) != null)
                {
                    String[] tokens = line.split("#");
                    for (int i = 0; i < tokens.length; ++i)
                        rmse[i] += Double.parseDouble(tokens[i]);
                }
                inputStream.close();
                reader.close();
            }
            System.out.println("RMSE list is ");
            String result = "";
            for (int i = 0; i < 8; ++i)
            {
                rmse[i] /= M;
                System.out.println(rmse[i] + " ");
                result += String.format("%.2f", rmse[i]) + " ";
            }
            FSDataOutputStream outputStream = hdfs.create(new Path(output.replace("RMSE","") + "/rmse.txt"));
            outputStream.write(result.getBytes());
            outputStream.close();
            //若想要查看RMSE的输出结果, 注释掉下面3行
            if(hdfs.exists(outputPath)) {
                hdfs.delete(outputPath, true);
            }
            //若想要查看预处理的输出结果, 注释掉下面3行
            Path outputPath1 = new Path(output.replace("RMSE","Preprocess"));
            if(hdfs.exists(outputPath1)) {
                hdfs.delete(outputPath1, true);
            }

        } catch(Exception e) {
            System.out.println("RMSE Failed!");
            e.printStackTrace();
        }
    }
}

class RMSEMapper extends Mapper<Object, Text, Text, Text> {
    @Override
    protected void map(Object key, Text value, Context context) throws IOException, InterruptedException {
        // output <key, value> = <hour, {(pre-act)^2}>
        // input value is hour:00-hour+1:00 act {pre_list}
        String[] tokens = value.toString().split(" ");
        double actualFreq = Double.parseDouble(tokens[1]);
        String outKey = "";

        for (int i = 2; i < tokens.length - 1; ++i) {
            double predictFreq = Double.parseDouble(tokens[i]);
            outKey += String.format("%.2f", Math.pow(predictFreq - actualFreq, 2)) + "#";
        }

        double predictFreq = Double.parseDouble(tokens[tokens.length - 1]);
        outKey += String.format("%.2f", Math.pow(predictFreq - actualFreq, 2));

        context.write(new Text(tokens[0].split(":")[0]), new Text(outKey));
    }
}

class RMSEPartitioner extends HashPartitioner<Text, Text> {
    @Override
    public int getPartition(Text key, Text value, int numReduceTasks) {
        int hour = Integer.parseInt(key.toString());
        return hour % 24;
    }
}


class RMSEReducer extends Reducer<Text, Text, Text, NullWritable> {
    @Override
    protected void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
        
        double[] result = new double[8]; // 8个预测结果
        Arrays.fill(result, 0.0);

        int N = 0;
        for (Text value : values) {
            String[] tokens = value.toString().split("#");
            for (int i = 0; i < tokens.length; ++i) {
                result[i] += Double.parseDouble(tokens[i]);
            }
            N++;
        }
        String outKey = "";
        for (int i = 0; i < 7; ++i) {
            result[i] = Math.sqrt(result[i] / (double) N);
            outKey += String.format("%.2f", result[i]) + "#";
        }
        result[7] = Math.sqrt(result[7] / (double) N);
        outKey += String.format("%.2f", result[7]);

        context.write(new Text(outKey), NullWritable.get());
    }
}
