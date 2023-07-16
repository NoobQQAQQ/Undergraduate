import java.io.IOException;
import java.util.Arrays;
import java.util.Iterator;

import org.apache.hadoop.fs.*;
import org.apache.hadoop.hdfs.DFSClient;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.RecordWriter;
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
import org.mockito.internal.matchers.Null;

public class Task2 {
    public static void task2(String input, String output) {
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

            Job job1 = Job.getInstance(conf, "task2");
            job1.setJarByClass(Task2.class);

            //指定输入输出格式 路径
            job1.setInputFormatClass(TextInputFormat.class);
            FileInputFormat.addInputPath(job1, inputPath);
            Task2OutputFormat.setOutputPath(job1, outputPath);
            LazyOutputFormat.setOutputFormatClass(job1, Task2OutputFormat.class);

            job1.setMapperClass(Task2Mapper.class);
            job1.setCombinerClass(Task2Combiner.class);
            job1.setPartitionerClass(Task2Partitioner.class);
            job1.setReducerClass(Task2Reducer.class);

            job1.setMapOutputKeyClass(Text.class);
            job1.setMapOutputValueClass(IntWritable.class);
            job1.setOutputKeyClass(Text.class);
            job1.setOutputValueClass(NullWritable.class);

            //指定Reducer个数
            job1.setNumReduceTasks(10);

            //提交job1
            boolean flag = job1.waitForCompletion(true)?true:false;
            if(!flag){
                System.out.println("task2非正常结束");
                System.exit(1);
            }
        } catch(Exception e) {
            System.out.println("Task2 Failed!");
            e.printStackTrace();
        }
    }
}

class Task2Mapper extends Mapper<Object, Text, Text, IntWritable> {
    private Text emitKey = new Text();
    private IntWritable emitValue = new IntWritable(1);

    @Override
    protected void map(Object key, Text value, Context context) throws IOException, InterruptedException {
        String[] tokens = value.toString().split(" ");
        //tokens[0]是ip地址; tokens[1]包含时间,只需要h
        if(tokens.length == 10) { //可能要过滤掉更多情况
            String outKey = tokens[0] + "#" + tokens[1].split(":")[1];
            emitKey.set(outKey);
            context.write(emitKey, emitValue);
            //<key, value> = <ip#time, 1>
        }
    }
}

//本地reduce, 提升性能
class Task2Combiner extends Reducer<Text, IntWritable, Text, IntWritable> {
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

//按照ip地址进行划分
class Task2Partitioner extends HashPartitioner<Text, IntWritable> {
    private Text emitKey = new Text();

    @Override
    public int getPartition(Text key, IntWritable value, int numReduceTasks) {
        String ip = key.toString().split("#")[0];
        emitKey.set(ip);
        return super.getPartition(emitKey, value, numReduceTasks);
    }
}

class Task2Reducer extends Reducer<Text, IntWritable, Text, NullWritable> {
    private int[] cnt;
    private String ip;
    //MultipleOutputs 可以指定输出文件，但文件名会带着0000这样的后缀
    private MultipleOutputs outputs;
    private Text emitKey;
    @Override
    protected void setup(Context context) throws IOException, InterruptedException {
        cnt = new int[24];
        Arrays.fill(cnt, 0);
        ip = "NO";
        outputs = new MultipleOutputs<Text, NullWritable>(context);
        emitKey = new Text();
    }
    @Override
    protected void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        String[] tokens = key.toString().split("#");
        String curIP = tokens[0];
        int time = Integer.parseInt(tokens[1]);
        if(!curIP.equals(ip) && !ip.equals("NO")) {
            //TODO: 写入txt文件
            String path = ip+".txt";
            int sum = 0;
            for(int i=0; i < 24; i++)
                sum += cnt[i];
            String outKey = String.format("%d", sum);
            emitKey.set(outKey);
            outputs.write(emitKey, NullWritable.get(), path);
            for(int i=0; i<24;i++) {
                outKey = String.format("%d:00-%d:00 %d", i, (i+1)%24, cnt[i]);
                emitKey.set(outKey);
                outputs.write(emitKey, NullWritable.get(), path);
            }
            ip = curIP;
            Arrays.fill(cnt, 0);
        }
        if(ip.equals("NO"))
            ip = curIP;
        int sum = 0;
        for(IntWritable i : values)
            sum += i.get();
        cnt[time] += sum;
    }

    @Override
    protected void cleanup(Context context) throws IOException, InterruptedException {
        //super.cleanup(context);
        if(ip.equals("NO"))
            return;
        //TODO: 写入txt文件
        String path = ip+".txt";
        int sum = 0;
        for(int i=0; i < 24; i++)
            sum += cnt[i];
        String outKey = String.format("%d", sum);
        emitKey.set(outKey);
        outputs.write(emitKey, NullWritable.get(), path);
        for(int i=0; i<24;i++) {
            outKey = String.format("%d:00-%d:00 %d", i, (i+1)%24, cnt[i]);
            emitKey.set(outKey);
            outputs.write(emitKey, NullWritable.get(), path);
        }
        outputs.close();
    }
}

//重写TextOutputFormat取消文件后缀
class Task2OutputFormat extends TextOutputFormat<Text, NullWritable> {
    @Override
    public Path getDefaultWorkFile(TaskAttemptContext context, String extension) throws IOException {
        //return super.getDefaultWorkFile(context, extension);
        FileOutputCommitter committer = (FileOutputCommitter)getOutputCommitter(context);
        return new Path(committer.getWorkPath(), getOutputName(context));
    }
}
