
#ifndef IMPEDANCEREADER_H
#define IMPEDANCEREADER_H

#include <vector>
#include <deque>
#include "systemstate.h"
#include "abstractrhxcontroller.h"
#include "rhxdatablock.h"

using namespace std;

struct ComplexPolar {
    double magnitude;
    double phase;
};

class ImpedanceReader
{
public:
    ImpedanceReader(SystemState* state_,  AbstractRHXController* rhxController_);
    bool measureImpedances();
    bool saveImpedances();

private:
    SystemState* state;
    AbstractRHXController* rhxController;

    static double approximateSaturationVoltage(double actualZFreq, double highCutoff);
    static ComplexPolar factorOutParallelCapacitance(ComplexPolar impedance, double frequency, double parasiticCapacitance);
    ComplexPolar measureComplexAmplitude(const deque<RHXDataBlock*> &dataQueue, int stream, int chipChannel,
                                         double sampleRate, double frequency, int numPeriods, QDataStream *outStream = nullptr) const;
    void applyNotchFilter(vector<double> &waveform, double fNotch, double bandwidth, double sampleRate) const;
    static ComplexPolar amplitudeOfFreqComponent(const vector<double> &waveform, int startIndex, int endIndex,
                                                 double sampleRate, double frequency);
    void runDemoImpedanceMeasurement();
};

#endif // IMPEDANCEREADER_H
