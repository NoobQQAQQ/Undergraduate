import org.apache.commons.math3.linear.*;
import arima.*;

public class PredictorAlgorithm {
    // size equals to length of data

    public static long AvgPredictor(double[] freqData, int size) {
        double sum = 0;
        for (double d : freqData) {
            sum += d;
        }
        // 访问频次一定是整数
        long predictFreq = Math.round(sum / size);
        return predictFreq;
    }

    public static long AvgPredictorRemoveMaxMin(double[] freqData, int size) {
        double freqMax = freqData[0];
        double freqMin = freqData[0];
        double sum = 0;

        for (double d : freqData) {
            sum += d;
            if (d > freqMax) {
                freqMax = d;
            }
            else if (d < freqMin) {
                freqMin = d;
            }
        }

        sum = sum - freqMax - freqMin;
        // 访问频次一定是整数
        long predictFreq = Math.round(sum / (size - 2));
        return predictFreq;
    }

    public static long polyfitPredictor(double[] freqData, int size, int degree) {
        double[][] Date = new double[size][degree + 1];
        // fill Date like
        // 1 x1 x1^2 ... x1^n
        // ...
        // 1 xm xm^2 ... xm^n
        for (int i = 0; i < size; i++) {
            for (int j = 0; j <= degree; j++) {
                Date[i][j] = Math.pow(i + 1, j);
            }
        }
        RealMatrix X = MatrixUtils.createRealMatrix(Date);
        RealVector y = MatrixUtils.createRealVector(freqData);
        // use the least squares as cost function or solve the equation Xw=y directly
        // we have the coefficients vector w is (X'X)^(-1)X'y
        RealVector w = new QRDecomposition(X).getSolver().solve(y);
        double[] x_15 = new double[degree + 1];
        for (int i = 0; i <= degree; i++) {
            x_15[i] = Math.pow(15, i);
        }
        return (long) w.dotProduct(MatrixUtils.createRealVector(x_15));
    }

    public static long arimaPredictor(double[] freqData) {
        ARIMAModel arima = new ARIMAModel(freqData);

        int[] res = new int[5];
        // 随机五次取平均
        for (int i = 0; i < 5; ++i) {
            int[] bestModel = arima.getARIMAModel();
            res[i] = arima.predictValue(bestModel[0], bestModel[1]);
        }
        int sum = 0;
        for (int i : res)
            sum += i;
        
        return (long)(sum/5);
    }
}