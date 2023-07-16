import java.io.IOException;
import java.util.Arrays;

import org.apache.hadoop.fs.*;
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

public class Task4 {
    public static void task4(String input, String output) {
        try {
            //通过xml文件获取配置信息
            Configuration conf = new Configuration();

            Path inputPath = new Path(input);
            Path outputPath = new Path(output);
            FileSystem hdfs = FileSystem.get(conf);
            //删除已有输出文件
            if(hdfs.exists(outputPath)) {
                hdfs.delete(outputPath, true); //true代表递归删除
            }

            Job job1 = Job.getInstance(conf, "Task4");
            job1.setJarByClass(Task4.class);

            //指定输入输出格式 路径
            job1.setInputFormatClass(TextInputFormat.class);
            FileInputFormat.addInputPath(job1, inputPath);
            Task4OutputFormat.setOutputPath(job1, outputPath);
            LazyOutputFormat.setOutputFormatClass(job1, Task4OutputFormat.class);

            job1.setMapperClass(Task4Mapper.class);
            job1.setCombinerClass(Task4Combiner.class);
            job1.setPartitionerClass(Task4Partitioner.class);
            job1.setReducerClass(Task4Reducer.class);

            job1.setMapOutputKeyClass(Text.class);
            job1.setMapOutputValueClass(Text.class);
            job1.setOutputKeyClass(Text.class);
            job1.setOutputValueClass(NullWritable.class);

            //指定Reducer个数
            job1.setNumReduceTasks(10);

            //提交job1
            boolean flag = job1.waitForCompletion(true)?true:false;
            if(!flag){
                System.out.println("task4非正常结束");
                System.exit(1);
            }
        } catch(Exception e) {
            System.out.println("Task4 Failed!");
            e.printStackTrace();
        }
    }
}

//输出格式为<接口#时间窗, 响应时间#访问次数>
class Task4Mapper extends Mapper<Object, Text, Text, Text> {
    @Override
    protected void map(Object key, Text value, Context context) throws IOException, InterruptedException {
        //<key, value> = <url#hour, response_time#count>
        String[] tokens = value.toString().split(" ");
        //tokens[4]是url; tokens[1]包含时间,只需要h; tokens[9]是响应时间
        if(tokens.length == 10) {
            String outKey = tokens[4] + "#" + tokens[1].split(":")[1];
            String outVal = tokens[9] + "#1";
            context.write(new Text(outKey), new Text(outVal));
        }
    }
}

//本地reduce, 提升性能
class Task4Combiner extends Reducer<Text, Text, Text, Text> {
    @Override
    protected void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
        int sumTime = 0;
        int sumCount = 0;
        for (Text t : values) {
            sumTime += Integer.parseInt(t.toString().split("#")[0]);
            sumCount += Integer.parseInt(t.toString().split("#")[1]);
        }
        context.write(key, new Text(sumTime + "#" + sumCount));
    }
}

//按照url进行划分
class Task4Partitioner extends HashPartitioner<Text, Text> {
    @Override
    public int getPartition(Text key, Text value, int numReduceTasks) {
        String url = key.toString().split("#")[0];
        return super.getPartition(new Text(url), value, numReduceTasks);
    }
}

class Task4Reducer extends Reducer<Text, Text, Text, NullWritable> {
    private int[] resTimeArr = new int[24];
    private int[] countArr = new int[24];
    private String url;
    //MultipleOutputs 可以指定输出文件，但文件名会带着0000这样的后缀
    private MultipleOutputs multiOutputs = null;

    @Override
    protected void setup(Context context) throws IOException, InterruptedException {
        Arrays.fill(resTimeArr, 0);
        Arrays.fill(countArr, 0);
        url = "@";
        multiOutputs = new MultipleOutputs<Text, NullWritable>(context);
    }
    @Override
    protected void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
        String[] tokens = key.toString().split("#");
        String curUrl = tokens[0];
        int hour = Integer.parseInt(tokens[1]);

        //上一个url已经统计完成, 需要输出
        if(!curUrl.equals(url) && !url.equals("@")) {
            int sumTime = 0;
            int sumCount = 0;
            for (int t : resTimeArr) {
                sumTime += t;
            }
            for (int t : countArr) {
                sumCount += t;
            }
            String outKey = String.format("%.2f\n",((double)sumTime) / ((double)sumCount));
            for(int i = 0; i < 24; i++) {
                if (countArr[i] > 0) {// 每个时间窗内可能有未访问的情况
                    outKey += String.format("%d:00-%d:00 %.2f\n", i, (i+1)%24, ((double)resTimeArr[i] / (double)countArr[i]));
                }
                else if (countArr[i] == 0) {
                    outKey += String.format("%d:00-%d:00 null\n", i, (i+1)%24);
                }
            }
            // 假定出现的接口肯定被访问过一次
            multiOutputs.write(new Text(outKey), NullWritable.get(), url.replaceFirst("/", "").replace("/", "-") + ".txt");
            url = curUrl;
            Arrays.fill(resTimeArr, 0);
            Arrays.fill(countArr, 0);
        }
        // 根据时间窗进行统计
        if (url.equals("@"))
            url = curUrl;

        for (Text t : values) {
            resTimeArr[hour] += Integer.parseInt(t.toString().split("#")[0]);
            countArr[hour] += Integer.parseInt(t.toString().split("#")[1]);
        }
    }

    @Override
    protected void cleanup(Context context) throws IOException, InterruptedException {
        //flush buffer into .txt file
        if(url.equals("@"))
            return;
        int sumTime = 0;
        int sumCount = 0;
        for (int t : resTimeArr) {
            sumTime += t;
        }
        for (int t : countArr) {
            sumCount += t;
        }
        String outKey = String.format("%.2f\n",((double)sumTime) / ((double)sumCount));
        for(int i = 0; i < 24; i++) {
            if (countArr[i] > 0) {// 每个时间窗内可能有未访问的情况
                outKey += String.format("%d:00-%d:00 %.2f\n", i, (i+1)%24, ((double)resTimeArr[i] / (double)countArr[i]));
            }
            else if (countArr[i] == 0) {
                outKey += String.format("%d:00-%d:00 null\n", i, (i+1)%24);
            }
        }
        multiOutputs.write(new Text(outKey), NullWritable.get(), url.replaceFirst("/", "").replace("/", "-") + ".txt");
        multiOutputs.close();
    }
}

//重写TextOutputFormat取消文件后缀
class Task4OutputFormat extends TextOutputFormat<Text, NullWritable> {
    @Override
    public Path getDefaultWorkFile(TaskAttemptContext context, String extension) throws IOException {
        FileOutputCommitter committer = (FileOutputCommitter)getOutputCommitter(context);
        return new Path(committer.getWorkPath(), getOutputName(context));
    }
}