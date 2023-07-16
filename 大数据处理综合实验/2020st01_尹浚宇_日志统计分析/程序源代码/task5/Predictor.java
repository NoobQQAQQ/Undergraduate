import java.io.IOException;
import java.util.Arrays;
import java.util.Iterator;

import org.apache.avro.generic.GenericData;
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

public class Predictor {
    public static void predictor(String input, String output) {
        try {
            //通过xml文件获取配置信息
            Configuration conf = new Configuration();

            Path inputPath = new Path(input);
            Path outputPath = new Path(output);
            FileSystem hdfs = FileSystem.get(conf);
            //删除已有输出文件
            if (hdfs.exists(outputPath)) {
                hdfs.delete(outputPath, true);
            }

            Job job1 = Job.getInstance(conf, "task5Predictor");
            job1.setJarByClass(Predictor.class);

            //指定输入输出格式 路径
            job1.setInputFormatClass(TextInputFormat.class);
            FileInputFormat.addInputPath(job1, inputPath);
            //job1.setOutputFormatClass(TextOutputFormat.class);
            //FileOutputFormat.setOutputPath(job1, outputPath);
            PredictorOutputFormat.setOutputPath(job1, outputPath);
            LazyOutputFormat.setOutputFormatClass(job1, PredictorOutputFormat.class);

            job1.setMapperClass(PredictorMapper.class);
            job1.setPartitionerClass(PredictorPartitioner.class);
            job1.setReducerClass(PredictorReducer.class);

            job1.setMapOutputKeyClass(Text.class);
            job1.setMapOutputValueClass(Text.class);
            job1.setOutputKeyClass(Text.class);
            job1.setOutputValueClass(NullWritable.class);

            //指定Reducer个数
            // 后续可增加
            job1.setNumReduceTasks(4);

            //提交job1
            boolean flag = job1.waitForCompletion(true) ? true : false;
            if (!flag) {
                System.out.println("作业非正常结束");
                System.exit(1);
            }
        } catch (Exception e) {
            System.out.println("Predictor Failed!");
            e.printStackTrace();
        }
    }
}

class PredictorMapper extends Mapper<Object, Text, Text, Text> {
    @Override
    protected void map(Object key, Text value, Context context) throws IOException, InterruptedException {
        // every line is of format url#hour freq1 freq2 ... freq15
        // <key, value> = <url#hour, act#{pre_list}>
        String[] tokens = value.toString().split(" ");
        //String[] url_hour = tokens[0].split("#");

        // extract the data from the previous fourteen days
        double[] freqData = new double[14];
        for (int i = 1; i < 15; i++) {
            freqData[i-1] = Double.parseDouble(tokens[i]);
        }

        // prediction
        String outKey = tokens[15] + "#";
        outKey += PredictorAlgorithm.AvgPredictor(freqData, 14) + "#";
        outKey += PredictorAlgorithm.AvgPredictorRemoveMaxMin(freqData, 14) + "#";

        for (int i = 1; i <= 5; i++) {
            outKey += PredictorAlgorithm.polyfitPredictor(freqData, 14, i) + "#";
        }
        outKey += PredictorAlgorithm.arimaPredictor(freqData);

        context.write(new Text(tokens[0]), new Text(outKey));
    }
}

//按照url进行划分
class PredictorPartitioner extends HashPartitioner<Text, Text> {
    @Override
    public int getPartition(Text key, Text value, int numReduceTasks) {
        String url = key.toString().split("#")[0];
        return super.getPartition(new Text(url), value, numReduceTasks);
    }
}

class PredictorReducer extends Reducer<Text, Text, Text, NullWritable> {
    private MultipleOutputs<Text, NullWritable> outputs;
    private String[] info;
    private String url;
    @Override
    protected void setup(Context context) throws IOException, InterruptedException {
        outputs = new MultipleOutputs<Text, NullWritable>(context);
        info = new String[24];
        for(int i=0;i<24;i++)
            info[i] = "NO";
        url = "NO";
    }
    @Override
    protected void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
        /*System.out.println(key.toString());
        for(Text i : values)
            System.out.println(i.toString());
        System.out.println();*/
        String[] tokens = key.toString().split("#");
        String curUrl = tokens[0];
        int time = Integer.parseInt(tokens[1]);
        if(!curUrl.equals(url) && !url.equals("NO")) {
            //TODO: 写入txt文件
            String path = url.replaceFirst("/","").replaceAll("/","-")+".txt";
            for(int i=0; i<24;i++) {
                if(!info[i].equals("NO")) {
                    String outKey = String.format("%d:00-%d:00 %s", i, (i+1)%24, info[i].replaceAll("#", " "));
                    //System.out.println(outKey);
                    outputs.write(new Text(outKey), NullWritable.get(), path);
                }
            }
            url = curUrl;
            for(int i=0;i<24;i++)
                info[i] = "NO";
        }
        if(url.equals("NO"))
            url = curUrl;
        for(Text i : values)
            info[time] = i.toString();
    }

    @Override
    protected void cleanup(Context context) throws IOException, InterruptedException {
        if(url.equals("NO"))
            return;
        String path = url.replaceFirst("/","").replaceAll("/","-")+".txt";
        for(int i=0; i<24;i++) {
            if(!info[i].equals("NO")) {
                String outKey = String.format("%d:00-%d:00 %s", i, (i+1)%24, info[i].replaceAll("#", " "));
                outputs.write(new Text(outKey), NullWritable.get(), path);
            }
        }
        outputs.close();
    }
}

//重写TextOutputFormat取消文件后缀
class PredictorOutputFormat extends TextOutputFormat<Text, NullWritable> {
    @Override
    public Path getDefaultWorkFile(TaskAttemptContext context, String extension) throws IOException {
        //return super.getDefaultWorkFile(context, extension);
        FileOutputCommitter committer = (FileOutputCommitter)getOutputCommitter(context);
        return new Path(committer.getWorkPath(), getOutputName(context));
    }
}