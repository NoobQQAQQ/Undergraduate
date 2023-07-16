package arima;
import static org.apache.commons.math3.stat.StatUtils.*;

import java.util.Random;
import java.util.Vector;

//import static org.apache.commons.math3.stat.StatUtils.mean;
//import static org.apache.commons.math3.stat.StatUtils.variance;

public class MathTool {
    /**
     * 计算自相关函数(系数) rou(k) = C(k) / C(0);
     * 其中 C(k) = sum((x(t) - mu)*(x(t - k) - mu)) / (N - k),
     * C(0) = var =  sum(x(t) - mu) ^2 / N;
     * @param originalData
     * @param order
     * @return 自相关函数(rou(k))
     */
    public static double [] autoCorrData(double [] originalData, int order)
    {
        double [] autoCov = autoCovData(originalData, order);
        double [] autoCorr = new double[order + 1];		//默认初始化为0
        double var = variance(originalData);

        if (var != 0)
        {
            for (int i = 0; i < autoCorr.length; ++i)
            {
                autoCorr[i] = autoCov[i] / var;
            }
        }

        return autoCorr;
    }

    /**
     * @param dataFir
     * @param dataSec
     * @return 皮尔逊相关系数(互相关)
     */
    public static double mutalCorr(double [] dataFir, double [] dataSec)
    {
        double sumX = 0.0;
        double sumY = 0.0;
        double sumXY = 0.0;
        double sumXSq = 0.0;
        double sumYSq = 0.0;
        int len = 0;

        if (dataFir.length != dataSec.length)
        {
            len = Math.min(dataFir.length, dataSec.length);
        }
        else
        {
            len = dataFir.length;
        }
        for (int i = 0; i < len; ++i)
        {
            sumX += dataFir[i];
            sumY += dataSec[i];
            sumXY += dataFir[i] * dataSec[i];
            sumXSq += dataFir[i] * dataFir[i];
            sumYSq += dataSec[i] * dataSec[i];
        }

        double numerator = sumXY - sumX * sumY / len;
        double denominator = Math.sqrt((sumXSq - sumX * sumX / len) * (sumYSq - sumY * sumY / len));

        if (denominator == 0)
        {
            return 0.0;
        }

        return numerator/ denominator;
    }

    /**
     * @param data
     * @return		互相关矩阵
     */
    public static double [][] computeMutalCorrMatrix(double [][] data)
    {
        double [][] result = new double[data.length][data.length];
        for (int i = 0; i < data.length; ++i)
        {
            for (int j = 0; j < data.length; ++j)
            {
                result[i][j] = mutalCorr(data[i], data[j]);
            }
        }

        return result;
    }

    /**
     * 计算自协方差，C(k) = sum((x(t) - mu)*(x(t - k) - mu)) / (N - k);
     * @param originalData
     * @param order
     * @return 自协方差(gama(k))-->认为是自相关系数
     */
    public static double [] autoCovData(double [] originalData, int order)
    {
        double mu = mean(originalData);
        double [] autoCov = new double[order + 1];

        for (int i = 0; i <= order; ++i)
        {
            autoCov[i] = 0.0;
            for (int j = 0; j < originalData.length - i; ++j)
            {
                autoCov[i] += (originalData[i + j] - mu) * (originalData[j] - mu);
            }
            autoCov[i] /= (originalData.length - i);
        }
        return autoCov;
    }

    /**
     * @param vec		模型的系数
     * @param data		数据
     * @return
     */
    public static double getModelAIC(Vector<double []>vec, double [] data)
    {
        int n = data.length;
        int p = 0, q = 0;
        double tmpAR = 0.0, tmpMA = 0.0;
        double sumErr = 0.0;
        Random random = new Random();

        double [] arCoe = vec.get(0);
        double [] maCoe = vec.get(1);
        p = arCoe.length;
        q = maCoe.length;
        double [] errData = new double[q];

        for (int i = p - 1; i < n; ++i)
        {
            tmpAR = 0.0;
            for (int j = 0; j < p - 1; ++j)
            {
                tmpAR += arCoe[j] * data[i - j - 1];
                //System.out.println(arCoe[j]+" "+data[i-j-1]);
            }
            //System.out.println("???");
            tmpMA = 0.0;
            for (int j = 1; j < q; ++j)
            {
                tmpMA += maCoe[j] * errData[j];
                //System.out.println(maCoe[j]+" "+errData[j]);
            }
            //System.out.println("???");
            for (int j = q - 1; j > 0; --j)
            {
                errData[j] = errData[j - 1];
            }
            errData[0] = random.nextGaussian() * Math.sqrt(maCoe[0]);

            sumErr += (data[i] - tmpAR - tmpMA) * (data[i] - tmpAR - tmpMA);
            //System.out.println(data[i]+" "+tmpAR+" "+tmpMA);
        }
        return (n - (q + p - 1)) * Math.log(sumErr / (n - (q + p - 1))) + (p + q) * 2;
    }

    /**
     * Levinson 方法求解
     * @param garma  代表的是数据的协方差
     * @return	返回结果的第一行元素代表的是在迭代过程中的方差，
     * 其余的元素代表的是迭代过程中存储的系数
     */
    public static double [][] LevinsonSolve(double [] garma)
    {
        int order = garma.length - 1;
        double [][] result = new double[order + 1][order + 1];
        double [] sigmaSq = new double[order + 1];

        sigmaSq[0] = garma[0];
        if(garma[0] == 0.0)
            result[1][1] = 0.0;
        else
            result[1][1] = garma[1] / sigmaSq[0];
        sigmaSq[1] = sigmaSq[0] * (1.0 - result[1][1] * result[1][1]);
        for (int k = 1; k < order; ++k)
        {
            double sumTop = 0.0;
            double sumSub = 0.0;
            for (int j = 1; j <= k; ++j)
            {
                sumTop += garma[k + 1 - j] * result[k][j];
                sumSub += garma[j] * result[k][j];
            }
            result[k + 1][k + 1] = (garma[k + 1] - sumTop) / (garma[0] - sumSub);
            for (int j = 1; j <= k; ++j)
            {
                result[k + 1][j] = result[k][j] - result[k + 1][k + 1] * result[k][k + 1 - j];
            }
            sigmaSq[k + 1] = sigmaSq[k] * (1.0 - result[k + 1][k + 1] * result[k + 1][k + 1]);
        }
        result[0] = sigmaSq;
        /*System.out.println("garma");
        for(double i:garma)
            System.out.print(i+" ");
        System.out.println();
        System.out.println("result");
        for(int i=0;i<result.length;i++)
            for(int j=0;j<result[0].length;j++)
                System.out.print(result[i][j]+" ");
        System.out.println();*/
        return result;
    }

    /**
     * 求解ARMA(p, q)的系数
     * @param originalData		原始数据
     * @param p			AR模型阶数
     * @param q			MA模型阶数
     * @return			ARMA模型系数
     */
    public static double [] computeARMACoe(double [] originalData, int p, int q)
    {
        double [] allGarma = autoCovData(originalData, p + q);
        double [] garma = new double[p + 1];
        for (int i = 0; i < garma.length; ++i)
        {
            garma[i] = allGarma[q + i];
        }
        double [][] arResult = LevinsonSolve(garma);
        // AR
        double [] ARCoe = new double[p + 1];
        for (int i = 0; i < p; ++i)
        {
            ARCoe[i] = arResult[p][i + 1];
        }
        ARCoe[p] = arResult[0][p];

        // MA
        double [] alpha = new double[p + 1];
        alpha[0] = -1;
        for (int i = 1; i <= p; ++i)
        {
            alpha[i] = ARCoe[i - 1];
        }

        double [] paraGarma = new double[q + 1];
        for (int k = 0; k <= q; ++k)
        {
            double sum = 0.0;
            for (int i = 0; i <= p; ++i)
            {
                for (int j = 0; j <= p; ++j)
                {
                    sum += alpha[i] * alpha[j] * allGarma[Math.abs(k + i - j)];
                    //System.out.println(alpha[i]+" "+alpha[j]+" "+(k+i-j));
                }
            }
            paraGarma[k] = sum;
        }
        /*System.out.println("paraGarma");
        for(double i : paraGarma)
            System.out.print(i+" ");
        System.out.println();*/
        double [][] maResult = LevinsonSolve(paraGarma);
        double [] MACoe = new double[q + 1];
        for (int i = 1; i <= q; ++i)
        {
            MACoe[i] = maResult[q][i];
        }
        MACoe[0] = maResult[0][q];

        double [] ARMACoe = new double[p + q + 2];
        for (int i = 0; i < ARMACoe.length; ++i)
        {
            if (i < ARCoe.length)
            {
                ARMACoe[i] = ARCoe[i];
            }
            else
            {
                ARMACoe[i] = MACoe[i - ARCoe.length];
            }
        }

        return ARMACoe;
    }
}
