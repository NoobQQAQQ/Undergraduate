import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;

import org.apache.hadoop.fs.*;
import org.apache.hadoop.hdfs.DFSClient;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.lib.LazyOutputFormat;
import org.apache.hadoop.mapred.lib.MultipleOutputFormat;
import org.apache.hadoop.mapred.lib.MultipleTextOutputFormat;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputCommitter;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.MultipleOutputs;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.partition.HashPartitioner;
import org.apache.hadoop.util.GenericOptionsParser;
import org.apache.hadoop.util.Progressable;

public class Preprocess {
    public static void preprocess(String input, String output) {
        try {
            //通过xml文件获取配置信息
            Configuration conf = new Configuration();
            //传参给Task1OutputFormat
            conf.set("outputPath", output);

            Path inputPath = new Path(input);
            Path outputPath = new Path(output);
            FileSystem hdfs = FileSystem.get(conf);
            //删除已有输出文件
            if(hdfs.exists(outputPath)) {
                hdfs.delete(outputPath, true);
            }

            Job job1 = Job.getInstance(conf, "task5Preprocess");
            job1.setJarByClass(Preprocess.class);

            //指定输入输出格式 路径
            job1.setInputFormatClass(TextInputFormat.class);
            job1.setOutputFormatClass(TextOutputFormat.class);
            FileInputFormat.addInputPath(job1, inputPath);
            FileOutputFormat.setOutputPath(job1, outputPath);


            job1.setMapperClass(Task5PreMapper.class);
            job1.setCombinerClass(Task5PreCombiner.class);
            job1.setPartitionerClass(Task5PrePartitioner.class);
            job1.setReducerClass(Task5PreReducer.class);

            job1.setMapOutputKeyClass(Text.class);
            job1.setMapOutputValueClass(IntWritable.class);
            job1.setOutputKeyClass(Text.class);
            job1.setOutputValueClass(NullWritable.class);

            //指定Reducer个数
            //job1.setNumReduceTasks(1);

            //提交job1
            boolean flag = job1.waitForCompletion(true)?true:false;
            if(!flag){
                System.out.println("作业非正常结束");
                System.exit(1);
            }
        } catch(Exception e) {
            System.out.println("Task1 Failed!");
            e.printStackTrace();
        }
    }
}

class Task5PreMapper extends Mapper<Object, Text, Text, IntWritable> {
    @Override
    protected void map(Object key, Text value, Context context) throws IOException, InterruptedException {
        String[] tokens = value.toString().split(" ");
        //tokens[1]包含时间; tokens[4]代表url
        String[] timeTokens = tokens[1].split(":");
        String hour = timeTokens[1];
        String day = timeTokens[0];
        day = day.replace("[","");
        day = day.split("/")[0];
        if(tokens.length == 10) {
            String url = tokens[4];
            if(url.charAt(url.length()-1) == '/')
                url = url.substring(0, url.length()-1);
            String outKey = url + "#" + hour + "#" + day;
            context.write(new Text(outKey), new IntWritable(1));
            //<key, value> = <url#hour#day, 1>
        }
    }
}

//本地reduce, 提升性能
class Task5PreCombiner extends Reducer<Text, IntWritable, Text, IntWritable> {
    @Override
    protected void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        Iterator<IntWritable> i = values.iterator();
        int sum = 0;
        while(i.hasNext()) {
            sum += i.next().get();
        }
        context.write(key, new IntWritable(sum));
    }
}

//按照url进行划分
class Task5PrePartitioner extends HashPartitioner<Text, IntWritable> {
    @Override
    public int getPartition(Text key, IntWritable value, int numReduceTasks) {
        String url = key.toString().split("#")[0];
        return super.getPartition(new Text(url), value, numReduceTasks);
    }
}

class Task5PreReducer extends Reducer<Text, IntWritable, Text, NullWritable> {
    private int[] cnt;
    private String url_hour;

    @Override
    protected void setup(Context context) throws IOException, InterruptedException {
        cnt = new int[15];
        Arrays.fill(cnt, 0);
        url_hour = "NO";
    }

    @Override
    protected void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        String[] tokens = key.toString().split("#");
        String cur_url_hour = tokens[0]+"#"+tokens[1];
        int day = Integer.parseInt(tokens[2]) - 8;
        if(!cur_url_hour.equals(url_hour) && !url_hour.equals("NO")) {
            String outKey = String.format("%s", url_hour);
            for(int num : cnt) {
                outKey = outKey + " " + num;
            }
            context.write(new Text(outKey), NullWritable.get());

            Arrays.fill(cnt, 0);
            url_hour = cur_url_hour;
        }
        if(url_hour.equals("NO"))
            url_hour = cur_url_hour;
        int sum = 0;
        Iterator<IntWritable> i = values.iterator();
        while(i.hasNext()) {
            sum += i.next().get();
        }
        cnt[day] = sum;
    }

    @Override
    protected void cleanup(Context context) throws IOException, InterruptedException {
        //super.cleanup(context);
        if(url_hour.equals("NO"))
            return;
        String outKey = String.format("%s", url_hour);
        for(int num : cnt) {
            outKey = outKey + " " + num;
        }
        context.write(new Text(outKey), NullWritable.get());
    }
}

