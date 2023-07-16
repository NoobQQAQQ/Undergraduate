
public class Main {
    public static void main(String[] args) {
        String input = args[0];
        String output = args[1];
        String output1 = output+"Preprocess";
        //String output2 = output+"Predict";
        String output3 = output+"RMSE";
        Preprocess.preprocess(input, output1);
        Predictor.predictor(output1, output);
        RMSE.rmse(output, output3);
    }
}
