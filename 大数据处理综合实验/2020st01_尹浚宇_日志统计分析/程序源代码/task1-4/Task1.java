import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import org.apache.hadoop.fs.*;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.partition.HashPartitioner;

public class Task1 {
    public static void task1(String input, String output) {
        try {
            //通过xml文件获取配置信息
            Configuration conf = new Configuration();
            //传参给Task1OutputFormat
            conf.set("outputPath", output);

            Path inputPath = new Path(input);
            //临时路径=原输出路径+TMP
            Path outputPath = new Path(output+"TMP");
            Path outputPathPost = new Path(output);
            FileSystem hdfs = FileSystem.get(conf);
            //删除已有输出文件
            if(hdfs.exists(outputPath)) {
                hdfs.delete(outputPath, true);
            }
            if(hdfs.exists(outputPathPost)) {
                hdfs.delete(outputPathPost, true);
            }

            Job job1 = Job.getInstance(conf, "task1");
            job1.setJarByClass(Task1.class);

            //指定输入输出格式 路径
            job1.setInputFormatClass(TextInputFormat.class);
            FileInputFormat.addInputPath(job1, inputPath);
            FileOutputFormat.setOutputPath(job1, outputPath);
            //若使用mr框架产生最后输出,注释上一行,使用下面两行, 需要新建job2
            //job1.setOutputFormatClass(Task1OutputFormat.class);
            //Task1OutputFormat.setOutputPath(job1, outputPath);
            //使用下行代码解决输出空文件的问题 job1参数有问题
            //LazyOutputFormat.setOutputFormatClass(job1, OutputFormat.class);

            job1.setMapperClass(Task1Mapper.class);
            job1.setCombinerClass(Task1Combiner.class);
            job1.setPartitionerClass(Task1Partitioner.class);
            job1.setReducerClass(Task1Reducer.class);

            job1.setMapOutputKeyClass(Text.class);
            job1.setMapOutputValueClass(IntWritable.class);
            job1.setOutputKeyClass(Text.class);
            job1.setOutputValueClass(NullWritable.class);

            //指定Reducer个数
            job1.setNumReduceTasks(24);

            //提交job1
            boolean flag = job1.waitForCompletion(true)?true:false;
            if(!flag){
                System.out.println("task1非正常结束");
                System.exit(1);
            }

            //对中间结果进行统计, 并删除中间结果
            Task1WriteFinal job2 = new Task1WriteFinal(output, conf);
            job2.read();
            job2.write();
            //注释掉下面的代码可查看中间结果
            if(hdfs.exists(outputPath)) {
                hdfs.delete(outputPath, true);
            }
        } catch(Exception e) {
            System.out.println("Task1 Failed!");
            e.printStackTrace();
        }
    }
}

class Task1Mapper extends Mapper<Object, Text, Text, IntWritable> {
    IntWritable emitValue = new IntWritable(1);
    Text emitKey = new Text();

    @Override
    protected void map(Object key, Text value, Context context) throws IOException, InterruptedException {
        String[] tokens = value.toString().split(" ");
        //tokens[1]包含时间,只需要h; tokens[7]代表状态码
        //只考虑正常的状态码
        if(tokens.length == 10) {
            String outKey = tokens[1].split(":")[1] + "#" + tokens[tokens.length - 3];
            emitKey.set(outKey);
            context.write(emitKey, emitValue);
            //<key, value> = <time#stateCode, 1>
        }
    }
}

//本地reduce, 提升性能
class Task1Combiner extends Reducer<Text, IntWritable, Text, IntWritable> {
    IntWritable emitValue = new IntWritable();

    @Override
    protected void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        int sum = 0;
        for(IntWritable i : values) {
            sum += i.get();
        }
        emitValue.set(sum);
        context.write(key, emitValue);
    }
}

//按照时间进行划分
class Task1Partitioner extends HashPartitioner<Text, IntWritable> {
    @Override
    public int getPartition(Text key, IntWritable value, int numReduceTasks) {
        String time = key.toString().split("#")[0];
        int hashcode = Integer.parseInt(time);
        return hashcode;
        //return super.getPartition(new Text(time), value, numReduceTasks);
    }
}

class Task1Reducer extends Reducer<Text, IntWritable, Text, NullWritable> {
    private int sum1 = 0;
    private int sum2 = 0;
    private int sum3 = 0;
    private int time = -1;
    private Text emitKey = new Text();

    @Override
    protected void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        String[] tokens = key.toString().split("#");
        int curTime = Integer.parseInt(tokens[0]);
        String stateCode = tokens[1];
        if(curTime != time && time != -1) {
            String outKey = String.format("%d:00-%d:00 200:%d 400:%d 404:%d",time, (time+1)%24, sum1, sum2, sum3);
            emitKey.set(outKey);
            context.write(emitKey, NullWritable.get());
            sum1 = 0;
            sum2 = 0;
            sum3 = 0;
            time = curTime;
        }
        if(time == -1)
            time = curTime;
        int sum = 0;
        for(IntWritable i : values)
            sum += i.get();
        if(stateCode.equals("200")) {
            sum1 += sum;
        }
        else if(stateCode.equals("400")) {
            sum2 += sum;
        }
        else if(stateCode.equals("404")) {
            sum3 += sum;
        }
        else {
            System.out.println("未知状态码 "+stateCode);
        }
    }

    @Override
    protected void cleanup(Context context) throws IOException, InterruptedException {
        if(time == -1)
            return;
        String outKey = String.format("%d:00-%d:00 200:%d 400:%d 404:%d",time, (time+1)%24, sum1, sum2, sum3);
        emitKey.set(outKey);
        context.write(emitKey, NullWritable.get());
    }
}

//如果使用Java API操作HDFS来产生最终输出,需要设计类操纵HDFS
class Task1WriteFinal {
    private String outputPathPre;
    private String outputPath;
    private FileSystem fs;
    private ArrayList<String> list;
    public Task1WriteFinal(String output, Configuration conf) {
        outputPath = output;
        outputPathPre = output+"TMP";
        try {
            fs = FileSystem.get(conf);
        } catch (IOException e) {
            System.out.println("IO failed in Task1WriteFinal");
        }
        list = new ArrayList<String>();
    }
    public void read(){
        try{
            FileStatus[] files = fs.listStatus(new Path(outputPathPre));
            for(FileStatus file : files) {
                FSDataInputStream inputStream = fs.open(file.getPath());
                BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
                String line = null;
                while((line = reader.readLine()) != null) {
                    list.add(line);
                }
                reader.close();
            }
        } catch (IOException e) {
            System.out.println("task1 read failed");
        }
    }
    public void write() {
        int sum1 = 0, sum2 = 0, sum3 = 0;
        for(String line : list) {
            String[] strs = line.split(" ");
            sum1 += Integer.parseInt(strs[1].split(":")[1]);
            sum2 += Integer.parseInt(strs[2].split(":")[1]);
            sum3 += Integer.parseInt(strs[3].split(":")[1]);
        }
        String str = String.format("200:%d\n400:%d\n404:%d\n",sum1, sum2, sum3);
        try {
            FSDataOutputStream outputStream = fs.create(new Path(outputPath+"/1.txt"));
            outputStream.write(str.getBytes());
            for(String line : list) {
                outputStream.write((line+"\n").getBytes());
            }
        } catch (IOException e) {
            System.out.println("write failed");
        }
    }
}

//如果要使用mr框架来产生最终的输出,需要自定义输出格式
class Task1OutputFormat extends FileOutputFormat<Text, NullWritable> {
    @Override
    public RecordWriter<Text, NullWritable> getRecordWriter(TaskAttemptContext taskAttemptContext) throws IOException, InterruptedException {
        Configuration conf =taskAttemptContext.getConfiguration();
        FileSystem fs = FileSystem.newInstance(conf);
        final FSDataOutputStream outputStream = fs.create(new Path(conf.get("outputPath")+"/1.txt"));
        RecordWriter<Text, NullWritable> recordWriter = new RecordWriter<Text, NullWritable>() {
            @Override
            public void write(Text text, NullWritable nullWritable) throws IOException, InterruptedException {
                String key = text.toString();
                outputStream.write(key.getBytes());
                System.out.print(text.toString());
            }

            @Override
            public void close(TaskAttemptContext taskAttemptContext) throws IOException, InterruptedException {
                outputStream.close();
            }
        };
        return recordWriter;
    }
}