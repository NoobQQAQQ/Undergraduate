import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.fs.*;
import org.apache.hadoop.hdfs.DFSClient;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.RecordWriter;
import org.apache.hadoop.mapred.lib.MultipleOutputFormat;
import org.apache.hadoop.mapred.lib.MultipleTextOutputFormat;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.*;
import org.apache.hadoop.mapreduce.lib.output.*;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.lib.partition.HashPartitioner;
import org.apache.hadoop.mapreduce.lib.partition.InputSampler;
import org.apache.hadoop.mapreduce.lib.partition.TotalOrderPartitioner;
import org.apache.hadoop.util.GenericOptionsParser;
import org.apache.hadoop.util.Progressable;
import org.mockito.internal.matchers.Null;

public class Task3 {
    public static void task3(String input, String output) {
        try {
            //通过xml文件获取配置信息
            Configuration conf = new Configuration();

            Path inputPath = new Path(input);
            Path outputPathPre = new Path(output+"TMP");
            Path partitionPath = new Path(output+"_partition.lst");
            Path outputPath = new Path(output);
            FileSystem hdfs = FileSystem.get(conf);
            //删除已有输出文件
            if(hdfs.exists(outputPathPre)) {
                hdfs.delete(outputPathPre, true);
            }
            if(hdfs.exists(partitionPath)) {
                hdfs.delete(partitionPath, true);
            }
            if(hdfs.exists(outputPath)) {
                hdfs.delete(outputPath, true);
            }


            Job job1 = Job.getInstance(conf, "task3-Statistic");
            job1.setJarByClass(Task3.class);
            //指定输入输出格式 路径
            job1.setInputFormatClass(TextInputFormat.class);
            FileInputFormat.addInputPath(job1, inputPath);
            job1.setOutputFormatClass(TextOutputFormat.class);
            FileOutputFormat.setOutputPath(job1, outputPathPre);

            job1.setMapperClass(Task3StatisMapper.class);
            job1.setCombinerClass(Task3StatisCombiner.class);
            job1.setPartitionerClass(Task3StatisPartitioner.class);
            job1.setReducerClass(Task3StatisReducer.class);

            job1.setMapOutputKeyClass(Text.class);
            job1.setMapOutputValueClass(IntWritable.class);
            job1.setOutputKeyClass(Text.class);
            job1.setOutputValueClass(NullWritable.class);

            //指定Reducer个数
            job1.setNumReduceTasks(10);

            //提交job1
            boolean flag = job1.waitForCompletion(true)?true:false;
            if(!flag){
                System.out.println("task3-1非正常结束");
                System.exit(1);
            }

            Configuration conf2 = new Configuration();
            conf2.set("mapreduce.totalorderpartitioner.naturalorder","false");
            //conf2.set(KeyValueLineRecordReader.KEY_VALUE_SEPERATOR, "#");
            Job job2 = Job.getInstance(conf2, "task3-Sort");
            job2.setJarByClass(Task3.class);

            job2.setInputFormatClass(KeyValueTextInputFormat.class);
            job2.setOutputFormatClass(TextOutputFormat.class);
            FileInputFormat.addInputPath(job2, outputPathPre);
            FileOutputFormat.setOutputPath(job2, outputPath);

            job2.setSortComparatorClass(KeyComparator.class);
            job2.setMapperClass(Task3SortMapper.class);
            job2.setReducerClass(Task3SortReducer.class);
            job2.setNumReduceTasks(3);

            job2.setMapOutputKeyClass(Text.class);
            job2.setMapOutputValueClass(NullWritable.class);
            job2.setOutputKeyClass(Text.class);
            job2.setOutputValueClass(NullWritable.class);

            TotalOrderPartitioner.setPartitionFile(job2.getConfiguration(), partitionPath);
            job2.setPartitionerClass(TotalOrderPartitioner.class);
            InputSampler.Sampler<Text, Text> sampler = new InputSampler.RandomSampler<Text, Text>(0.3,3,5);
            InputSampler.writePartitionFile(job2, sampler);

            boolean flag2 = job2.waitForCompletion(true)?true:false;
            if(!flag2) {
                System.out.println("task3-2非正常结束");
                System.exit(1);
            }
            if(hdfs.exists(outputPathPre)) {
                hdfs.delete(outputPathPre, true);
            }
            if(hdfs.exists(partitionPath)) {
                hdfs.delete(partitionPath, true);
            }
        } catch(Exception e) {
            System.out.println("Task3 Failed!");
            e.printStackTrace();
        }
    }
}

class Task3StatisMapper extends Mapper<Object, Text, Text, IntWritable> {
    private Text emitKey = new Text();
    private IntWritable emitValue = new IntWritable(1);

    @Override
    protected void map(Object key, Text value, Context context) throws IOException, InterruptedException {
        //<key, value> = <interface, 1>
        String[] tokens = value.toString().split(" ");
        //tokens[4]是接口
        if(tokens.length == 10) {
            String outKey = tokens[4];
            emitKey.set(outKey);
            context.write(emitKey, emitValue);
        }
    }
}

//本地reduce, 提升性能
class Task3StatisCombiner extends Reducer<Text, IntWritable, Text, IntWritable> {
    private IntWritable emitValue = new IntWritable();

    @Override
    protected void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        int sum = 0;
        for(IntWritable i : values)
            sum += i.get();
        emitValue.set(sum);
        context.write(key, emitValue);
    }
}

//按照接口进行划分
class Task3StatisPartitioner extends HashPartitioner<Text, IntWritable> {
    @Override
    public int getPartition(Text key, IntWritable value, int numReduceTasks) {
        return super.getPartition(key, value, numReduceTasks);
    }
}

class Task3StatisReducer extends Reducer<Text, IntWritable, Text, NullWritable> {
    private int sum;
    private String url;
    @Override
    protected void setup(Context context) throws IOException, InterruptedException {
        sum = 0;
        url = "NO";
    }
    @Override
    protected void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        String curUrl = key.toString();
        if(!curUrl.equals(url) && !url.equals("NO")) {
            //TODO: 写入txt文件
            String outKey = String.format("%s %d", url, sum);
            context.write(new Text(outKey), NullWritable.get());
            url = curUrl;
            sum = 0;
        }
        if(url.equals("NO"))
            url = curUrl;
        Iterator<IntWritable> i = values.iterator();
        while(i.hasNext()) {
            sum += i.next().get();
        }
    }

    @Override
    protected void cleanup(Context context) throws IOException, InterruptedException {
        //super.cleanup(context);
        if(url.equals("NO"))
            return;
        //TODO: 写入txt文件
        String outKey = String.format("%s %d", url, sum);
        context.write(new Text(outKey), NullWritable.get());
    }
}

class KeyComparator extends WritableComparator {
    protected KeyComparator() {
        super(Text.class, true);
    }

    @Override
    public int compare(WritableComparable writableComparable1, WritableComparable writableComparable2) {
        double num1 = Double.parseDouble(writableComparable1.toString().split(" ")[1]);
        double num2 = Double.parseDouble(writableComparable2.toString().split(" ")[1]);
        if(num1 > num2)
            return -1;
        else if(num1 < num2)
            return 1;
        else
            return 0;
    }
}
class Task3SortMapper extends Mapper<Text, Text, Text, NullWritable> {
    @Override
    protected void map(Text key, Text value, Context context) throws IOException, InterruptedException {
        //super.map(key, value, context);
        //System.out.println("Mapper"+key.toString());
        context.write(key, NullWritable.get());
    }
}
class Task3SortReducer extends Reducer<Text, NullWritable, Text, NullWritable> {
    @Override
    protected void reduce(Text key, Iterable<NullWritable> values, Context context) throws IOException, InterruptedException {
        //super.reduce(key, values, context);
        Iterator<NullWritable> i = values.iterator();
        while(i.hasNext()) {
            i.next();
            //System.out.println("Reducer" + key.toString());
            context.write(key, NullWritable.get());
        }
    }
}