import org.apache.spark.SparkConf;
import org.apache.spark.api.java.JavaRDD;
import org.apache.spark.api.java.JavaSparkContext;

public class Main {
    public static void main(String[] args) {
        String input = args[0];
        String output1 = args[1];
        String output2 = args[2];
        String output3 = args[3];
        String output4 = args[4];
        JavaSparkContext jsc = new JavaSparkContext(new SparkConf().setAppName("spark").setMaster("local[4]"));
        JavaRDD<String> lines = jsc.textFile(input).filter(x -> x.split(" ").length == 10 ? true : false);
        SparkTask1.task1(lines, output1, jsc);
        SparkTask2.task2(lines, output2, jsc);
        SparkTask3.task3(lines, output3, jsc);
        SparkTask4.task4(lines, output4, jsc);
    }
}
