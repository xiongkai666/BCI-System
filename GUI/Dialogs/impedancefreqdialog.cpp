
#include <QtWidgets>

#include "impedancefreqdialog.h"

// Electrode impedance measurement frequency selection dialog - this
// allows users to select a new measurement frequency, while validating
// that the new frequency lies within an acceptable range.
ImpedanceFreqDialog::ImpedanceFreqDialog(double desiredImpedanceFreq, double lowerBandwidth, double upperBandwidth,
                                         double dspCutoffFreq, bool dspEnabled, double sampleRate, QWidget *parent) :
    QDialog(parent)
{
    // Lower frequency limit based on sample rate: FPGA command RAM can only hold 1024 commands,
    // so this limits the period of the sinusoidal waveform generated by the impedance test DAC.
    lowerLimitSample = sampleRate / 1024.0;

    // Upper frequency limit based on sample rate: It is difficult to accurately approximate
    // a sine wave (using the on-chip impedance test DAC) in fewer than 4 steps per period.
    upperLimitSample = sampleRate / 4.0;

    // Lower frequency limit based on amplifier bandwidth:  We should stay at least 1.5X above
    // the effective lower amplifier bandwidth of the amplifier so that our impedance
    // measurements are not affected by a roll-off in gain.  (This may be overly conservative.)
    double effectiveLowerBandwidth = lowerBandwidth;
    if (dspEnabled) {
        if (dspCutoffFreq > lowerBandwidth) {
            effectiveLowerBandwidth = dspCutoffFreq;
        }
    }
    lowerLimitBandwidth = effectiveLowerBandwidth * 1.5;

    // Upper frequency limit based on amplifier bandwidth:  We should stay at least 1.5X below
    // the effective upper amplifier bandwidth of the amplifier so that our impedance
    // measurements are not affected by a roll-off in gain.  (This may be overly conservative.)
    upperLimitBandwidth = upperBandwidth / 1.5;

    impedanceFreqLineEdit = new QLineEdit(QString::number(desiredImpedanceFreq, 'f', 0), this);
    connect(impedanceFreqLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(onLineEditTextChanged()));

    QString sampleRangeText(tr("The sampling rate of "));
    sampleRangeText.append(QString::number(sampleRate / 1000.0, 'f', 2));
    sampleRangeText.append(tr(" Hz restricts this range to "));
    sampleRangeText.append(QString::number(lowerLimitSample, 'f', 1));
    sampleRangeText.append(tr(" Hz to "));
    sampleRangeText.append(QString::number(upperLimitSample, 'f', 0));
    sampleRangeText.append(tr(" Hz."));
    sampleRangeLabel = new QLabel(sampleRangeText, this);

    bandwidth1RangeLabel = new QLabel(tr("For acceptable accuracy, the frequency should "
                                         "lie within the amplifier bandwidth by a factor of 1.5."), this);
    bandwidth1RangeLabel->setWordWrap(true);

    QString bandwidthRangeText(tr("This restricts the measurement frequency range to "));
    bandwidthRangeText.append(QString::number(lowerLimitBandwidth, 'f', 1));
    bandwidthRangeText.append(tr(" Hz to "));
    bandwidthRangeText.append(QString::number(upperLimitBandwidth, 'f', 0));
    bandwidthRangeText.append(tr(" Hz."));
    bandwidth2RangeLabel = new QLabel(bandwidthRangeText, this);

    QHBoxLayout *impedanceFreqSelectLayout = new QHBoxLayout;
    impedanceFreqSelectLayout->addWidget(new QLabel(tr("Electrode Impedance Measurement Frequency"), this));
    impedanceFreqSelectLayout->addWidget(impedanceFreqLineEdit);
    impedanceFreqSelectLayout->addWidget(new QLabel(tr("Hz"), this));
    impedanceFreqSelectLayout->addStretch(1);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(impedanceFreqSelectLayout);
    mainLayout->addWidget(sampleRangeLabel);
    mainLayout->addWidget(bandwidth1RangeLabel);
    mainLayout->addWidget(bandwidth2RangeLabel);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    setWindowTitle(tr("Select Electrode Impedance Measurement Frequency"));

    onLineEditTextChanged();
}

// Validate new entry, and enable OK button if entry is valid.
void ImpedanceFreqDialog::onLineEditTextChanged()
{
    double value = impedanceFreqLineEdit->text().toDouble();
    bool inRange = true;

    if (value < lowerLimitSample || value > upperLimitSample) {
        sampleRangeLabel->setStyleSheet("color: red");
        inRange = false;
    } else {
        sampleRangeLabel->setStyleSheet("");
    }
    if (value < lowerLimitBandwidth || value > upperLimitBandwidth) {
        bandwidth1RangeLabel->setStyleSheet("color: red");
        bandwidth2RangeLabel->setStyleSheet("color: red");
        inRange = false;
    } else {
        bandwidth1RangeLabel->setStyleSheet("");
        bandwidth2RangeLabel->setStyleSheet("");
    }
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(inRange);
}