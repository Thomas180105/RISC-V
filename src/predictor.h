#ifndef RISC_V_PREDICTOR_H
#define RISC_V_PREDICTOR_H
#include "type.h"
#include <cmath>//for NAN
//四位两级自适应器
class Predictor{
private:
    static const int instructionBits = 12;
    static const int historyBits = 4;
    static const int mask = 1 << instructionBits;
    bool PHT[1 << instructionBits][1 << historyBits];//Pattern History Table 给定指令以及当前历史情况，可以确定预测值
    Context BHR[1 << instructionBits];//Branch History Register              给定指令就可以得到历史记录,记录最近的四次正确答案
    int correctCount;
    int wrongCount;
public:
    Predictor() : correctCount(0), wrongCount(0) {}
    bool Predict(Context instruction)
    {
        Context index = instruction & mask;
        return PHT[index][BHR[index]];
    }
    //维护计数，同时更新PHT、BHR状态
    void update(Context instruction, bool answer)
    {
        Context index = instruction & mask;
        bool advise = PHT[index][BHR[index]];
        if (advise == answer) ++correctCount;
        else
        {
            ++wrongCount;
            PHT[index][BHR[index]] = answer;//覆写PHT
        }
        BHR[index] = (BHR[index] >> 1) | (answer ? 0b1000 : 0);
    }
    float tellAccuracy() const
    {
        if (!correctCount && !wrongCount) return NAN;
        else return static_cast<float>(correctCount) / static_cast<float>(correctCount + wrongCount);
    }
};
#endif //RISC_V_PREDICTOR_H
