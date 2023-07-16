package arima;

import java.util.ArrayList;
import java.util.Random;
import java.util.Vector;

public class ARIMAModel
{
    double [] originalData = {};

    Vector<double []>arimaCoe = new Vector<>();

    public ARIMAModel(double [] originalData)
    {
        this.originalData = originalData;
    }

    public double [] preFirstDiff(double [] preData)		// 一阶差分(1)
    {
        double [] tmpData = new double[preData.length - 1];
        for (int i = 0; i < preData.length - 1; ++i)
        {
            tmpData[i] = preData[i + 1] - preData[i];
        }
        return tmpData;
    }

    public int [] getARIMAModel()
    {
        double [] data = this.preFirstDiff(originalData);

        double minAIC = Double.MAX_VALUE;
        int [] bestModel = new int[3];
        Vector<double []>coe = new Vector<>();

        for (int p = 1; p <= 2; ++p)
        {
            for (int q = 1; q <= 2; ++q)
            {
                ARMAModel arma = new ARMAModel(data, p, q);
                coe = arma.solveCoeOfARMA();
                //System.out.println("coe length :" +coe.size());
                double aic = MathTool.getModelAIC(coe, data);
                //System.out.println("num1 :"+aic+"; num2 :"+minAIC);
                if (aic < minAIC)
                {
                    minAIC = aic;
                    bestModel[0] = p;
                    bestModel[1] = q;
                    bestModel[2] = (int)Math.round(minAIC);
                    this.arimaCoe = coe;
                    //System.out.println("赋值coe length :"+coe.size()+"; arimaCoe length :"+arimaCoe.size());
                }
                //System.out.println("循环中 arimaCode length :"+arimaCoe.size());
            }
        }
        //System.out.println("arimaCoe length :" + arimaCoe.size());
        //System.out.println("************************************************************");
        return bestModel;
    }

    public int predictValue(int p, int q)
    {
        double [] data = this.preFirstDiff(originalData);

        int n = data.length;
        int predict = 0;
        double tmpAR = 0.0, tmpMA = 0.0;
        double [] errData = new double[q + 1];

        Random random = new Random();
        double [] arCoe = this.arimaCoe.get(0);
        double [] maCoe = this.arimaCoe.get(1);

        for(int k = p; k < n; ++k)
        {
            tmpAR = 0;
            tmpMA = 0;
            for(int i = 0; i < p; ++i)
            {
                tmpAR += arCoe[i] * data[k- i - 1];
            }
            for(int i = 1; i <= q; ++i)
            {
                tmpMA += maCoe[i] * errData[i];
            }

            //产生各个时刻的噪声
            for(int j = q; j > 0; --j)
            {
                errData[j] = errData[j-1];
            }

            errData[0] = random.nextGaussian() * Math.sqrt(maCoe[0]);
        }

        predict = (int)(tmpAR + tmpMA);
        return predict + (int)Math.round(originalData[originalData.length - 1]);
    }
}
