/*
 
	PlainDSP: Data Acquisition and Digital Signal Processing library
	Revision 1g

	Applicable license :

	This program and related paper documentation are owned by HL2 group SAS. 
	The program is subject to the license GNU GPL version 3, which content 
	is available in the file attached to this program. This license allows 
	you to use, reproduce and adapt this program for private, educational, 
	research purposes. 

	If you intend to (i) use this program on a large scale and/or (ii) adapt 
	this code, for COMMERCIAL PURPOSES, please contact HL2 at the following 
	email address : contact@hl2.fr in order to be granted a commercial license, 
	subject to specific terms and conditions.
	
*/

/* Include libraries */
#include <PlainDSP.h>

/* Data acquisition status */
#define DSP_DISABLED 0x00 
#define DSP_IDLE 0x01 /* Idle, no acquisition */
#define DSP_WAITING 0x02 /* Ready to acquire, waiting for an ADC sample to be taken */
#define DSP_TRIGGERED 0x03 /* An ADC sample has been taken, ready to be recorded */

/* Volatile variables: <!> Cannot be declared in the header file */
volatile uint8_t _dataAcqStatus = DSP_DISABLED; 
volatile uint8_t _adcLSB;
volatile uint8_t _adcMSB;
uint8_t _originalTIMSK0; 
uint8_t _originalTIMSK2; 
uint8_t _originalDIDR0; 
#define FLT_ZERO 0.0
#define FLT_ONE 1.0
#define FLT_TWO 2.0
#define SQR_ROO_TWO 1.4142135623731
#define TWO_PI 6.28318530717958647693
#define FOUR_PI 12.56637061435917295385

/*!
 * @brief constructor
 * @note no memory allocation is made here
 * @see SetAcquisitionEngine()
 */
PlainDSP::PlainDSP(void) 
{

	/* Set default acquisition status */
	_dataAcqStatus = DSP_DISABLED; 
	/* Set default value */
	_lastSamples = 0;
	/* Null data (real) vector */
	_vData = NULL;
	/* Null imaginary vector */
	_vImag = NULL; 
}


PlainDSP::~PlainDSP(void) 
/*!
	@brief Destructor 
	
	ReleaseAcquisitionEngine() is internally called
*/
{
	ReleaseAcquisitionEngine();
}


/* Public functions */

void PlainDSP::ReleaseAcquisitionEngine(void)
/*!
	@brief Release acquisition engine and relative functions
	@return nothing
*/
{
	if (_dataAcqStatus == DSP_IDLE) {
		cli(); /* Disable all interrupts */
		TIMSK1 &= ~(1 << OCIE1B); /* Disable timer1 interrupt */
		DIDR0 = _originalDIDR0;
		sei(); /* Enable all interrupts */
		_dataAcqStatus = DSP_DISABLED; /* Set acquisition status */
	}
}


uint8_t PlainDSP::Channel(uint8_t adcChannel)
/*!
	@brief Set the actual data acquisition channel and returns its value
	
	@param adcChannel (8 bits unsigned integer): optional, one adc channel, allowed range from 0 to 5
	
	@return actual adcChannel (8 bits unsigned integer), allowed range from 0 to 5
*/
{
	
	_adcChannel = adcChannel;
	/* Digital Input Disable Register */
	DIDR0 = _adcChannel;
	/* Set ADC channel */
	ADMUX &= ~(0x07); /* Clear previous adc channel setting */
	ADMUX |= _adcChannel; 
	return(_adcChannel);
}


uint8_t PlainDSP::Channel(void)
/*!
	@brief returns the actual data acquisition channel
	
	@return actual adcChannel (8 bits unsigned integer), allowed range from 0 to 5
*/
{
	return(_adcChannel);
}
	

float *PlainDSP::SetAcquisitionEngine(uint8_t adcChannel, uint8_t refVoltage, float samplingFrequency, uint16_t samples, uint8_t options) 
/*!
	@brief Set acquisition parameters, full options
	
	@param adcChannel (8 bits unsigned integer): one adc channel, allowed range from 0 to 5
	@param refVoltage: the adc reference voltage, as per @ref DSP_REF_VOL_x constants  
	@param samplingFrequency (32 bits float): in hertz, check constants for min (@ref DSP_MIN_SMP_FREQUENCY) and max (@ref DSP_MAX_SMP_FREQUENCY) values 
	@param samples (16 bits unsigned integer): number of samples acquired during each run (should be a power of 2)
	@param  options: data scanning options, as per @ref DSP_OPT_x constants

	@return pointer to the vector of acquired data (pointer to 32 bits floats). 
	This address is nulled while releasing the acquisition engine with ReleaseAcquisitionEngine()
*/
{
	if (_dataAcqStatus == DSP_DISABLED) {
		/* Set internal variables */
		_options = options;
		_refVoltage = refVoltage;
		_lastSamples = _samples;
		_samples = samples;
		_adcChannel = adcChannel;
		if (samplingFrequency < DSP_MIN_SMP_FREQUENCY) {
			samplingFrequency = DSP_MIN_SMP_FREQUENCY;
		} else if (samplingFrequency > DSP_MAX_SMP_FREQUENCY) {
			samplingFrequency = DSP_MAX_SMP_FREQUENCY;
		}
		_samplingFrequency = samplingFrequency;
		/* Data (Real) vector */
		ReleaseVector(DSP_DAT_TYP_REAL);
		_vData = CreateVector(_samples); /* Create data (real) vector */
		/* Imaginary values */
		ReleaseVector(DSP_DAT_TYP_IMAG);
		_vImag = CreateVector(_samples);; /* Create imaginary vector */
		/* Disable all interrupts */
		cli();
		_originalDIDR0 = DIDR0;
		/* Digital Input Disable Register */
		DIDR0 = _adcChannel;
		/* Clear ADC control registers */
		ADCSRA = 0x00;
		ADCSRB = 0x00;
		ADMUX  = 0x00;
		ADMUX |= (_refVoltage << 6); /* Set reference voltage */
		ADMUX |= _adcChannel; /* Set ADC channel */
		/* Compute ADC pre-scaler */
		float divisionFactor = ((13.0 * _samplingFrequency * 1.0E+6) / 16.0);
		uint8_t adcPrescaler = 7; /* Default division factor exponent */
		while ((divisionFactor > float(1 << adcPrescaler)) && (adcPrescaler > 2)) {
			adcPrescaler -= 1;
		}
		ADCSRA |= adcPrescaler; /* Set ADC pre-scaler */
		ADCSRA |= (1 << ADEN); /* Enable ADC */
		ADCSRA |= (1 << ADATE); /* ADC Auto Trigger Enable */
		ADCSRA |= (1 << ADIE); /* ADC interrupt Enable */
		ADCSRB |= ((1 << ADTS0) | (1 << ADTS2)); /* ADC Auto Trigger Source Selection */
		/* Noise reduction mode */
		SMCR = (1 << SM0); /* Set ADC noise reduction mode */
		/* Reset Timer/Counter1 control registers and Interrupt Mask Register */
		TCCR1A = 0x00; 
		TCCR1B = 0x00; 
		TIMSK1 = 0x00; 
		/* Compute clock timer1 pre-scaler */
		uint8_t preScalers[] = {0, 3, 6, 8, 10}; /* Log2 of pre-scalers */	
		uint8_t timerPrescaler = 0;
		uint32_t upperCount;
		do {
			upperCount = uint32_t(F_CPU / (_samplingFrequency * (1 << preScalers[timerPrescaler]))) - 1 ;
			timerPrescaler += 1;
		} while ((upperCount > 0xFFFF) && (timerPrescaler < 5));
		OCR1A = uint16_t(upperCount);
		TCCR1B |=  (1 << WGM12); /* Set clear timer1 on Compare Match (CTC) Mode (Mode 4) */
		TCCR1B |= timerPrescaler; /* Set timer1 pre-scaler */
		TIMSK1 |= (1 << OCIE1B); /* Enable timer1 interrupt  */
		_dataAcqStatus = DSP_IDLE; /* Set acquisition status */
		sei(); /* Enable all interrupts */
	}
	return(_vData);
}

void PlainDSP::GetScanData(void) 
/*! 
	@brief Acquire data from one specific detector (base 0)
	
	@note on completion of this function, data are available in the data vector as per the acquisition parameter settings 
	
	@return nothing
*/
{
	if (_dataAcqStatus == DSP_IDLE) {
		if ((_options & DSP_OPT_DIS_TIM_0) == DSP_OPT_DIS_TIM_0) {
			_originalTIMSK0 = TIMSK0; /* Record timer/counter0 mask */
			TIMSK0 = 0x00; /* Disable timer/counter0 */
		} 
		if ((_options & DSP_OPT_DIS_TIM_2) == DSP_OPT_DIS_TIM_2) {
			_originalTIMSK2 = TIMSK2; /* Record timer/counter2 mask */
			TIMSK2 = 0x00; /* Disable timer/counter2 */
		} 
		if ((_options & DSP_OPT_NOI_CANCELLER) == DSP_OPT_NOI_CANCELLER) {
			SMCR |= (1 << SE); /* Activate ADC noise reduction mode */	
		}
		float *ptrData = _vData;
		float *ptrDataMax = (_vData + _samples);
		do {
			/* Set waiting state */
			_dataAcqStatus = DSP_WAITING;
			/* Wait for triggered state */
			while (_dataAcqStatus != DSP_TRIGGERED);
			/* Write data in vector LSB->MSB */
			*ptrData = float(((_adcMSB << 8) | _adcLSB)); /* Record data */
			ptrData += 1;
		} while (ptrData != ptrDataMax);
		_dataAcqStatus = DSP_IDLE; /* Reset acquisition status */
		/* Reset options */
		if ((_options & DSP_OPT_DIS_TIM_0) == DSP_OPT_DIS_TIM_0) {
			TIMSK0 = _originalTIMSK0; /* Restore timer/counter0 operation */
		} 
		if ((_options & DSP_OPT_DIS_TIM_2) == DSP_OPT_DIS_TIM_2) {
			TIMSK2 = _originalTIMSK2; /* Restore timer/counter2 operation */
		} 
		if ((_options & DSP_OPT_NOI_CANCELLER) == DSP_OPT_NOI_CANCELLER) {
			SMCR &= ~(1 << SE); /* Deactivate ADC noise reduction mode */
		}
	}
}

void PlainDSP::GetScanData(uint16_t triggerLevel, uint8_t horizontalPosition, uint16_t timeOut) 
/*!
	@brief Acquire data from one specific detector (base 0) 
	
	@param triggerLevel (16 bits unsigned integer): in adc counts, signal above which the scan is completed
	@param horizontalPosition (8 bits unsigned integer): specifies the location of the data point which triggered the acquisition. 
	Expressed as a percent of the size of the data vector (see comments below)
	@param timeOut (16 bits unsigned integer): in seconds, if no triggering signal is detected within the timeout interval, the scan is completed and released. 
	Omitting timeOut or setting timeOut to 0 means endless timeout
		
	@note On completion of this function, data are available in the data vector as per the acquisition parameter settings 
	
	@note Setting 50 as the horizontalPosition value will make the data vector contain the 64 samples before the trigger level detection, 
	the 65th sample corresponds to the signal which triggered the acquisition followed by the 63 samples acquired after the trigger level detection
		
	@return nothing
*/
{
	if (_dataAcqStatus == DSP_IDLE) {
		if ((_options & DSP_OPT_DIS_TIM_0) == DSP_OPT_DIS_TIM_0) {
			_originalTIMSK0 = TIMSK0; /* Record timer/counter0 mask */
			TIMSK0 = 0x00; /* Disable timer/counter0 */
		} 
		if ((_options & DSP_OPT_DIS_TIM_2) == DSP_OPT_DIS_TIM_2) {
			_originalTIMSK2 = TIMSK2; /* Record timer/counter2 mask */
			TIMSK2 = 0x00; /* Disable timer/counter2 */
		} 
		if ((_options & DSP_OPT_NOI_CANCELLER) == DSP_OPT_NOI_CANCELLER) {
			SMCR |= (1 << SE); /* Activate ADC noise reduction mode */	
		}
		uint16_t targetPreRecords = ((_samples * horizontalPosition) / 100);
		uint16_t targetPostRecords = _samples;
		uint16_t targetRollOvers = (uint16_t)(timeOut * (_samplingFrequency / _samples));
		uint16_t rollOverCounts = 0; 
		uint16_t intValue = 0; 
		uint8_t triggered = 0; /* Boolean */
		uint16_t preRecords = 0; /* Preset pre-records */
		uint16_t postRecords = 0; /* Preset post-records */
		float *ptrRecords = _vData;
		float *ptrRecordsMax = (ptrRecords + _samples); /* For roll over process */
		do {
			/* Set waiting state */
			_dataAcqStatus = DSP_WAITING;
			/* Wait for triggered state */
			while (_dataAcqStatus != DSP_TRIGGERED);
			intValue = ((_adcMSB << 8) | _adcLSB);
			if (triggered) {
				postRecords += 1;
			} else {
				if (intValue > triggerLevel) { /* Detect signal peak */
					triggered = 1;
					targetPostRecords = (_samples - preRecords);
				} else if (preRecords < targetPreRecords) {
					preRecords += 1; /* Saturate pre-record counts */
				}
			}
			*ptrRecords = (float)intValue; /* Record data */
			ptrRecords += 1; /* Update records pointer position */
			if (ptrRecords == ptrRecordsMax) { /* Pointer roll over */
				ptrRecords -= _samples;
				rollOverCounts += 1;
				if (timeOut && (rollOverCounts == targetRollOvers)) {
					break;
				}
			}
		} while (postRecords != targetPostRecords);
		/* Roll down data */
		uint16_t recordsOffset = 0;
		if (rollOverCounts) {
			recordsOffset = (ptrRecords - _vData);
		}
		RollDown(_vData, recordsOffset, _samples);		
		/* Stop sequential recording mode */
		_dataAcqStatus = DSP_IDLE; /* Reset status */
		/* Reset options */
		if ((_options & DSP_OPT_DIS_TIM_0) == DSP_OPT_DIS_TIM_0) {
			TIMSK0 = _originalTIMSK0; /* Restore timer/counter0 operation */
		} 
		if ((_options & DSP_OPT_DIS_TIM_2) == DSP_OPT_DIS_TIM_2) {
			TIMSK2 = _originalTIMSK2; /* Restore timer/counter2 operation */
		} 
		if ((_options & DSP_OPT_NOI_CANCELLER) == DSP_OPT_NOI_CANCELLER) {
			SMCR &= ~(1 << SE); /* Deactivate ADC noise reduction mode */
		}
	}
}

uint8_t PlainDSP::Exponent(uint16_t value) 
/* Computes the Exponent of a powered 2 value */
{
	uint8_t result = 0;
	while (((value >> result) & 1) != 1) result++;
	return(result);
}

void PlainDSP::ComputeForwardFFT(void)
/*!
	@brief Compute the Fast Fourier Transform (time scale to frequency scale)
	
	@note In place execution. Results are recorded in the original vectors
	@note vImag is nulled before computing the forward FFT in this function
	
	@return nothing
*/
{
	ClearVector(DSP_DAT_TYP_IMAG);
	ComputeFFT(DSP_FORWARD);
}

void PlainDSP::ComputeInverseFFT(void)
/*!
	@brief Compute the Fast Fourier Transform (frequency scale to time scale)

	@note In place execution. Results are recorded in the original vectors
	
	@return nothing
*/
{
	
	ComputeFFT(DSP_INVERSE);
}

void PlainDSP::ComputeFFT(uint8_t dir)
/*!
 * @deprecated use ComputeForwardFFT() or ComputeInverseFFT() instead
	@brief Compute the Fast Fourier Transform

	@param dir (8 bits unsigned integer): direction, must be DSP_FORWARD (forward FFT) or DSP_INVERSE (inverse FFT)
		
	@note In place execution. Results are recorded in the original vectors
	@note In forward mode, vImag must contain null entries
	
	@return nothing
*/
{
	/* Reverse bits */
	uint16_t i;
	uint16_t j = 0;
	float *vRealPtr;
	float *vImagPtr;
	for (i = 0, vRealPtr = _vData, vImagPtr = _vImag; i < (_samples - 1); i++, vRealPtr++, vImagPtr++) {
		if (j > i) {
			Swap(vRealPtr, (_vData + j));
			Swap(vImagPtr, (_vImag + j));
		}
		uint16_t k = (_samples >> 1);
		while (j >= k) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}
	/* Compute the FFT  */
	float c1 = -FLT_ONE; 
	float c2 = FLT_ZERO;
	uint16_t l2 = 1;
	uint8_t log2Samples = Exponent(_samples);
	uint8_t l;
	for (l = 0; (l < log2Samples); l++) {
		uint16_t l1 = l2;
		l2 <<= 1;
		float u1 = FLT_ONE; 
		float u2 = FLT_ZERO;
		uint16_t m;
		for (m = 0; m < l1; m++) {
			vRealPtr = (_vData + m);
			vImagPtr = (_vImag + m);
			for (i = m; i < _samples; i += l2) {
				float *vReal_i1_Ptr = (vRealPtr + l1);
				float *vImag_i1_Ptr = (vImagPtr + l1);	
				float t1 = (u1 * *vReal_i1_Ptr) - (u2 * *vImag_i1_Ptr);
				float t2 = (u1 * *vImag_i1_Ptr) + (u2 * *vReal_i1_Ptr);
				*vReal_i1_Ptr = (*vRealPtr - t1); 
				*vImag_i1_Ptr = (*vImagPtr - t2);
				*vRealPtr += t1;
				*vImagPtr += t2;
				/* Increment variables */
				vRealPtr += l2;
				vImagPtr += l2;
			 }
			 float z = ((u1 * c1) - (u2 * c2));
			 u2 = ((u1 * c2) + (u2 * c1));
			 u1 = z;
		}
		c2 = sqrt((FLT_ONE - c1) / FLT_TWO); 
		if (dir == DSP_FORWARD) {
			c2 = -c2;
		}
		c1 = sqrt((FLT_ONE + c1) / FLT_TWO);
	}
	/* Scaling for inverse transform */
	if (dir == DSP_INVERSE) {
		vRealPtr = _vData;
		vImagPtr = _vImag;
		for (i = 0; i < _samples; i++, vRealPtr++, vImagPtr++) {
			*vRealPtr /= _samples;
			*vImagPtr /= _samples;
		}
	}
}

void PlainDSP::ComplexToReal(uint8_t scaleType) 
/*!
	@brief Converts a complex number in a real (e.g. frequency spectrum). The result lies in the real values vector

	@param scaleType (8 bits unsigned integer): as per @ref DSP_SCL_TYP_x constants
	
	@note In place execution. Results are recorded in the original vectors
	
	@return nothing
*/
{
	float *ptrData;
	float *ptrImag;
	uint16_t i;
	for (i = 0, ptrData = _vData, ptrImag = _vImag; i < _samples; i++, ptrData++, ptrImag++) {
		float real = *ptrData;
		float imag = *ptrImag;
		float result = FLT_ZERO;
		switch(scaleType) {
		case DSP_SCL_TYP_MAGNITUDE:
			result = (FLT_TWO * sqrt((real * real) + (imag * imag)));
			break;
		case DSP_SCL_TYP_AMPLITUDE:
			result = (FLT_TWO * sqrt((real * real) + (imag * imag))) / (_samples >> 1);
			break;
		case DSP_SCL_TYP_RMS:
			result = (FLT_TWO * SQR_ROO_TWO * sqrt((real * real) + (imag * imag)) / _samples);
			break;
		}
		*ptrData = result;
	}
}

/* This macro is executed within the Windowing function in order to reduce the execution time of this function */
#define APPLY_WINDOWING_FACTOR() { \
	if (dir == DSP_FORWARD) { \
		*forwardIterator *= weightingFactor; \
		*backwardIterator *= weightingFactor; \
	} else { \
		*forwardIterator /= weightingFactor; \
		*backwardIterator /= weightingFactor; \
	}    \
	forwardIterator++ ; \
	backwardIterator--; \
}
	

void PlainDSP::Windowing(uint8_t windowType, uint8_t dir) 
/*!
	@brief Apply weighting factor on real data prior to FFT

	@param windowType (8 bits unsigned integer): specifies the windowing type, as per @ref DSP_WIN_TYP_x constants
	@param dir (8 bits unsigned integer): direction, must be DSP_FORWARD (forward FFT) or DSP_INVERSE (inverse FFT)

	@note Weighting factors are computed once before multiple use of FFT.
	The weighting function is symmetric; half the weights are recorded 
	@note In place execution. Results are recorded in the original vector.
	
	@return nothing
*/
{
	
	uint16_t samplesMinusOne = (_samples - 1);
	float weightingFactor = FLT_ONE;
	float* forwardIterator = _vData;
	float* backwardIterator = ((_vData + _samples) - 1);
	switch (windowType) {
	case DSP_WIN_TYP_RECTANGLE: /* Rectangle (box car) */
		/* nothing to do : a * 1.0 = a */
		break;
	case DSP_WIN_TYP_HAMMING: /* Hamming */
		for (uint16_t i = 0; i < (_samples >> 1); i++) {
			float ratio = (float(i) / samplesMinusOne);
			weightingFactor = (0.54 - (0.46 * cos(TWO_PI * ratio)));
			APPLY_WINDOWING_FACTOR();
		}
		break;
	case DSP_WIN_TYP_HANN: /* Hann */
		for (uint16_t i = 0; i < (_samples >> 1); i++) {
			float ratio = (float(i) / samplesMinusOne);
			weightingFactor = (0.5 * (FLT_ONE - cos(TWO_PI * ratio)));
			APPLY_WINDOWING_FACTOR();
		}
		break;
	case DSP_WIN_TYP_TRIANGLE: /* Triangle (Bartlett) */
		for (uint16_t i = 0; i < (_samples >> 1); i++) {
			weightingFactor = (FLT_ONE - ((FLT_TWO * fabs(float(i) - (samplesMinusOne / FLT_TWO))) / samplesMinusOne));
			APPLY_WINDOWING_FACTOR();
		}
		break;
	case DSP_WIN_TYP_BLACKMAN: /* Blackmann */
		for (uint16_t i = 0; i < (_samples >> 1); i++) {
			float ratio = (float(i) / samplesMinusOne);
			weightingFactor = (0.42 - (0.5 * (cos(TWO_PI * ratio))) + (0.08 * (cos(FOUR_PI * ratio))));
			APPLY_WINDOWING_FACTOR();
		}
		break;
	case DSP_WIN_TYP_FLT_TOP: /* Flat top */
		for (uint16_t i = 0; i < (_samples >> 1); i++) {
			float ratio = (float(i) / samplesMinusOne);
			weightingFactor = (0.2810639 - (0.5208972 * cos(TWO_PI * ratio)) + (0.1980399 * cos(FOUR_PI * ratio)));
			APPLY_WINDOWING_FACTOR();
		}
		break;
	case DSP_WIN_TYP_WELCH: /* Welch */
		for (uint16_t i = 0; i < (_samples >> 1); i++) {
			float a = ((float(i) - (samplesMinusOne / FLT_TWO)) / (samplesMinusOne / FLT_TWO));
			weightingFactor = FLT_ONE - (a * a);
			APPLY_WINDOWING_FACTOR();
		}
		break;
	}
}

void PlainDSP::MajorPeak(struct strPeakProperties *result) 
/*!
	@brief Find the major peak frequency from a spectrum

	@param result (pointer to strPeakProperties structure): contains the result values
	
	@return nothing
*/
{
	MajorPeak(FLT_ZERO, (_samplingFrequency / FLT_TWO), result);
}

void PlainDSP::MajorPeak(float loFrequency, float upFrequency, struct strPeakProperties *result) 
/*!
	@brief Find the major peak frequency from a spectrum

	A peak is determined by three consecutive points where the central one is above the others
	The major peak is the highest peak within a range. No interpolation is applied!

	@param loFrequency (32 bits float): apply the function from this specified lower frequency (inclusive)
	@param upFrequency (32 bits float): apply the function up to this specified upper frequency (inclusive)
	@param result (pointer to strPeakProperties structure)
		
	@return nothing
*/
{
	
	uint16_t loBin = PositionToBin(loFrequency);
	uint16_t upBin = PositionToBin(upFrequency);
	/* Check parameters */
	if (loBin < 1) {
		loBin = 1;
	}
	if (upBin > ((_samples >> 1) - 1)) {
		upBin = ((_samples >> 1) - 1);
	}	
	/* Compute results */
	float yMax = 0;
	uint16_t binOfYMax = 0;
	for (uint16_t i = loBin; i <= upBin; i++) {
		float y = *(_vData + i); /* actual y value */
		float y_m_1 = *((_vData + i) - 1); /* y - 1 value */
		float y_p_1 = *((_vData + i) + 1); /* y + 1 value */
		/* Is the actual point above its right and left neighbors? */
		if ((y_m_1 < y) && (y > y_p_1)) {
			/* Is the actual point higher than any other previously analyzed point? */
			if (y > yMax) {
				/* Record values */
				yMax = y;
				binOfYMax = i;
			}
		}
	}
	/* Update results */
	result->bin = binOfYMax;
	result->position = ((binOfYMax * _samplingFrequency) / _samples); 
	result->height = yMax;
}


void PlainDSP::TargetPeak(float targetPosition, float tolerance, struct strPeakProperties *result) 
/*!
	@brief Find the major peak from a spectrum

	@param targetPosition (32 bits float): abscissa of the target peak
	@param tolerance (32 bits float): find the major peak within the range: 
	(targetPosition - tolerance), (targetPosition + tolerance) including limits
	@param result (pointer to strPeakProperties structure)

	@note peak position and height are interpolated
	
	@return nothing
*/
{
	uint16_t loBin = PositionToBin(targetPosition - tolerance);
	uint16_t upBin = PositionToBin(targetPosition + tolerance);
	/* Check parameters */
	if (loBin < 1) {
		loBin = 1;
	}
	if (upBin > ((_samples >> 1) - 1)) {
		upBin = ((_samples >> 1) - 1);
	}
	float freqInterval = (_samplingFrequency / _samples);
	float least_x_err = DBL_MAX;
	float best_x_calc = FLT_ZERO;
	float best_y_calc = FLT_ZERO;
	uint16_t bestBin = 0;
	for (uint16_t i = loBin; i <= upBin; i++) {
		float x = (i * freqInterval); /* actual x value */
		float y = *(_vData + i); /* actual y value */
		float y_m_1 = *((_vData + i) - 1); /* y - 1 value */
		float y_p_1 = *((_vData + i) + 1); /* y + 1 value */
		if ((y_m_1 < y) && (y > y_p_1)) {
			float x_diff = (((y_m_1 - y_p_1) / (y_m_1 - (FLT_TWO * y) + y_p_1)) / FLT_TWO);
			float x_calc = (x + (x_diff * freqInterval));
			float y_calc = (y - (((y_m_1 - y_p_1) * x_diff) / 4.0));
			float x_err = fabs(x - targetPosition);
			if (x_err < least_x_err){
				/* Retain best peak properties */
				least_x_err = x_err;
				best_x_calc = x_calc;
				best_y_calc = y_calc;
				bestBin = i;
			}
		}
	}
	/* Returned results */
	result->bin = bestBin;
	result->position = best_x_calc; /* eq. to (*result).position = best_x_calc; */
	result->height = best_y_calc; /* eq. to (*result).height = best_y_calc; */
}

void PlainDSP::InterpolatedPeak(float targetPosition, struct strPeakProperties *result)
/*! 
	@brief Find quadratically interpolated peaks
	
	Compute the most approaching bin from the target frequency
	
	@param targetPosition (32 bits float): abscissa of the target peak
	@param result (pointer to strPeakProperties structure)
	
	@note peak position and height are interpolated
	
	@return nothing
*/
{
	
	uint16_t bin = PositionToBin(targetPosition);
	float freqInterval, x, y, y_m_1, y_p_1;
	freqInterval = (_samplingFrequency / _samples);
	x = (bin * freqInterval); /* actual x value */
	y = *(_vData + bin); /* y value */
	y_m_1 = *(_vData + bin - 1); /* y - 1 value */
	y_p_1 = *(_vData + bin + 1); /* y + 1 value */
	float x_calc, y_calc;
	if ((y > y_m_1) && (y > y_p_1)) {
		float x_diff = (((y_m_1 - y_p_1) / (y_m_1 - (FLT_TWO * y) + y_p_1)) / FLT_TWO);
		x_calc = (x + (x_diff * freqInterval));
		y_calc = (y - (((y_m_1 - y_p_1) * x_diff) / 4.0));
	} else {
		x_calc = -FLT_ONE;
		y_calc = -FLT_ONE;
	}
	/* Returned results */
	result->bin = bin;
	result->position = x_calc; /* eq. to (*result).position = x_calc; */
	result->height = y_calc; /* eq. to (*result).height = y_calc; */
}



void PlainDSP::ReleaseVector(uint8_t dataType) 
/*!
	@brief Delete a vector of data and release memory space 
	
	@param dataType (8 bits unsigned integer): type of data to be cleared, as per @ref DSP_DAT_TYP_x constants
		
	@return nothing
*/
{
	switch(dataType) {
	case DSP_DAT_TYP_IMAG:
		free(_vImag);
		_vImag = NULL;
		break;
	case DSP_DAT_TYP_REAL:
		free(_vData);
		_vData = NULL;
		break;
	}
} 

float *PlainDSP::GetDataAddress(uint8_t dataType) 
/*!
	@brief Get the address (pointer value) of a specified set of data
	
	@param dataType (8 bits unsigned integer): type of data to be cleared, as per @ref DSP_DAT_TYP_x constants
	
	@return pointer to the vector containing acquired data (pointer to 32 bits floats). 
	This address is nulled while releasing the acquisition engine with ReleaseAcquisitionEngine()
*/
{
	float *ptrData = NULL;
	switch(dataType) {
	case DSP_DAT_TYP_IMAG:
		ptrData = _vImag;
		break;
	case DSP_DAT_TYP_REAL:
		ptrData = _vData;
		break;
	}
	return(ptrData);	
}

float PlainDSP::ReadData(uint16_t index)
/*!
	@brief Read one single data from the _vData vector at the specified index
	
	@param index (16 bits unsigned integer): index of the data within the vector of data to be read

	@return 32 bits float value
*/
{
	return(ReadData((uint8_t)DSP_DAT_TYP_REAL, index));
}

float PlainDSP::ReadData(uint8_t dataType, uint16_t index)
/*!
	@brief Read one single data from a specified index
	
	@param dataType (8 bits unsigned integer): type of data vector to read from, as per @ref DSP_DAT_TYP_x constants
	@param index (16 bits unsigned integer): index of the data within the vector of data to be read

	@return 32 bits float value
*/
{
	float *ptrData = GetDataAddress(dataType);
	return(*(ptrData + index));
}

float PlainDSP::ReadData(float *ptrData, uint16_t index)
/*!
	@brief Read one single data from a specified index
	
	@param ptrData (32 bits float pointer): pointer to the vector data to be read
	@param index (16 bits unsigned integer): index of the data within the vector of data to be read

	@return 32 bits float value
*/
{
	return(*(ptrData + index));
}

void PlainDSP::WriteData(uint16_t index, float data)
/*! 
	@brief Write one single data at a specified index
	
	@param index (16 bits unsigned integer): target index within the vector of data to write to
	@param data (32 bits float): source data to write
	
	@returns nothing
*/
{
	WriteData(DSP_DAT_TYP_REAL, index, data);
}

void PlainDSP::WriteData(uint8_t dataType, uint16_t index, float data)
/*! 
	@brief Write one single data at a specified index
	
	@param dataType (8 bits unsigned integer): type of data vector to read from, as per @ref DSP_DAT_TYP_x constants
	@param index (16 bits unsigned integer): target index within the vector of data to write to
	@param data (32 bits float): source data to write
	
	@returns nothing
*/
{
	float *ptrData = GetDataAddress(dataType);
	*(ptrData + index) = data;
}


/* Complementary functions */

float PlainDSP::Max(void)
/*!
	@brief Compute the maximal value from the content of the _vData internal vector

	@return maximum value (32 bits float) read form the vector containing data  
*/
{
	return(Max(0, (_samples - 1)));	
} 

float PlainDSP::Max(uint16_t lowBound, uint16_t highBound)
/*!
	@brief Compute the maximal value from the content of the _vData internal vector
	
	@param lowBound (base 0, 16 bits unsigned integer): function applies to value starting at this specified index (inclusive)
	@param highBound (base 0, 16 bits unsigned integer): function applies to value ending at this specified index (inclusive)
		
	@return maximum value (32 bits float) read form the vector containing data  
*/
{
	float res =  DBL_MIN;
	uint16_t i;
	float *ptrData;
	for (i = lowBound, ptrData = _vData; i <= highBound; i++, ptrData++) {
		if (*ptrData > res) {
			res = *ptrData;
		}	
	}	
	return(res);	
} 

float PlainDSP::Min(void)
/*!
	@brief Compute the minimal value from the content of the _vData internal vector

	@return minimum value (32 bits float) read form the vector containing data  
*/
{
	return(Min(0, (_samples - 1)));	
} 

float PlainDSP::Min(uint16_t lowBound, uint16_t highBound)
/*!
	@brief Compute the minimal value from the content of the _vData internal vector

	@param lowBound (base 0, 16 bits unsigned integer): function applies to value starting at this specified index (inclusive)
	@param highBound (base 0, 16 bits unsigned integer): function applies to value ending at this specified index (inclusive)
	
	@return minimum value (32 bits float) read form the vector containing data  
*/
{
	float res = FLT_MAX;
	uint16_t i;
	float *ptrData;
	for (i = lowBound, ptrData = _vData; i <= highBound; i++, ptrData++) {
		if (*ptrData < res) {
			res = *ptrData;
		}	
	}	
	return(res);	
} 
	
float PlainDSP::Average(void) 
/*!
	@brief Compute the mean value from the content of the _vData internal vector
	
	@note Average is equivalent to arithmetic mean
	@note The whole data from the main vector of data is used

	@return mean value (32 bits float) read form the vector containing data  
*/
{
	return(Average(0, (_samples - 1)));
}

float PlainDSP::Average(uint16_t lowBound, uint16_t highBound)
/*!
	@brief Compute the mean value from the content of the _vData internal vector
	
	@param lowBound (base 0, 16 bits unsigned integer): function applies to value starting at this specified index (inclusive)
	@param highBound (base 0, 16 bits unsigned integer): function applies to value ending at this specified index (inclusive)
	
	@note Average is equivalent to arithmetic mean

	@return mean value (32 bits float) read form the vector containing data  
*/
{
	/* Compute average */
	float sum = FLT_ZERO;
	uint16_t i;
	float *ptrData;
	for (i = lowBound, ptrData = _vData; i <= highBound; i++, ptrData++) {
		sum += *ptrData;	
	}	
	return(sum / ((highBound - lowBound) + 1));	
}

float PlainDSP::RMS(void) 
/*!
	@brief Compute the root mean square value from the content of the _vData internal vector
	
	@note The whole data from the main vector of data is used
	@note Root Mean Square (RMS) is also known as the quadratic mean.
	
	@return the true root mean square computed over the sample intensities
*/
{
	return(RMS(0, (_samples - 1)));	
}

float PlainDSP::RMS(uint16_t lowBound, uint16_t highBound)
/*!
	@brief Compute the root mean square value from the content of the _vData internal vector
	
	@param lowBound (base 0, 16 bits unsigned integer): function applies to value starting at this specified index (inclusive)
	@param highBound (base 0, 16 bits unsigned integer): function applies to value ending at this specified index (inclusive)
	
	@note Root Mean Square (RMS) is also known as the quadratic mean.
	
	@return the true root mean square computed over the sample intensities
*/
{
	/* Compute true RMS */
	float sumOfSquares = FLT_ZERO;
	uint16_t i;
	float *ptrData;
	for (i = lowBound, ptrData = _vData; i <= highBound; i++, ptrData++) {
		sumOfSquares += (*ptrData * *ptrData);
	}	
	return(sqrt(sumOfSquares / ((highBound - lowBound) + 1)));	
}

void PlainDSP::ResetOffset(void) 
/*!
	@brief Reset the offset of all data in internal _vData vector
	
	@returns nothing
*/
{
	/* Compute average */
	float sum = FLT_ZERO;
	uint16_t i;
	float *ptrData;
	for (i = 0, ptrData = _vData; i < _samples; i++, ptrData++) {
		sum += *ptrData;
	}	
	float average = (sum / (float)_samples);
	/* Subtract average */
	for (i = 0, ptrData = _vData; i < _samples; i++, ptrData++) {
		*ptrData -= average;
	}	
} 


void PlainDSP::Gain(float gain)
/*!
	@brief Apply gain factor to data in internal _vData vector 
	
	@param gain (32 bits float): the gain to apply
	
	@note The whole data from the main vector of data is used
	
	@return nothing
*/
{
	Gain(gain, 0, (_samples - 1));
}

void PlainDSP::Gain(float gain, uint16_t lowBound, uint16_t highBound)
/*!
	@brief Apply gain factor to data in internal _vData vector 
	
	@param gain (32 bits float): the gain to apply
	@param lowBound (base 0, 16 bits unsigned integer): function applies to value starting at this specified index (inclusive)
	@param highBound (base 0, 16 bits unsigned integer): function applies to value ending at this specified index (inclusive)
	
	@return nothing
*/
{
	uint16_t i;
	float *ptrData;
	for (i = lowBound, ptrData = _vData; i <= highBound; i++, ptrData++) {
		*ptrData *= gain;
	}	
}

void PlainDSP::Offset(float offset)
/*!
	@brief Apply offset constant to data in internal _vData vector 
	
	@param offset (32 bits float): the offset to apply
	
	@note The whole data from the main vector of data is used
	
	@return nothing
*/
{
	Offset(offset, 0, (_samples - 1));
}

void PlainDSP::Offset(float offset, uint16_t lowBound, uint16_t highBound)
/*!
	@brief Apply offset constant to data in internal _vData vector 
	
	@param offset (32 bits float): the offset to apply
	@param lowBound (base 0, 16 bits unsigned integer): function applies to value starting at this specified index (inclusive)
	@param highBound (base 0, 16 bits unsigned integer): function applies to value ending at this specified index (inclusive)
	
	@return nothing
*/
{
	uint16_t i;
	float *ptrData;
	for (i = lowBound, ptrData = _vData; i <= highBound; i++, ptrData++) {
		*ptrData += offset;
	}	
}

void PlainDSP::Normalize(float normalizingValue)
/*!
	@brief Normalize data. Attribute the normalization value to the most intense signal

	@param normalizingValue (32 bits float): the value attributed to the most intense signal
		
	@note The function applies to the whole data contained in the default set of data (real)
	@note In place execution. Results are recorded in the original vector
	
	@return nothing
*/
{
	Normalize(_vData, (_samples - 1), normalizingValue, 0, (_samples - 1));
} 

void PlainDSP::Normalize(float normalizingValue, uint16_t lowBound, uint16_t highBound)
/*!
	@brief Normalize data. Attribute the normalizing value to the most intense signal

	@param normalizingValue (32 bits float): the value attributed to the most intense signal
	@param lowBound (base 0, 16 bits unsigned integer): function applies to value starting at this specified index (inclusive)
	@param highBound (base 0, 16 bits unsigned integer): function applies to value ending at this specified index (inclusive)
		
	@note In place execution. Results are recorded in the original vector
	
	@return nothing
*/
{
	Normalize(_vData, (_samples - 1), normalizingValue, lowBound, highBound);
} 


void PlainDSP::Normalize(float* vector, uint16_t size, float normalizingValue) 
/*!
	@brief Normalize data from the given vector. Attribute the normalizing value to the most intense signal

	@param vector (pointer to 32 bits floats): the vector to normalize
	@param size (16 bits unsigned integer): the size of the vector to normalize
	@param normalizingValue (32 bits float): the value attributed to the most intense signal
		
	@note In place execution. Results are recorded in the original vector.
	
	@return nothing
*/
{
	Normalize(vector, size, normalizingValue, 0, (size - 1));
}


void PlainDSP::Normalize(float* vector, uint16_t size, float normalizingValue, uint16_t lowBound, uint16_t highBound) 
/*!
	@brief Normalize data from the given vector. Attribute the normalizing value to the most intense signal

	@param vector (pointer to 32 bits floats): the vector to normalize
	@param size (16 bits unsigned integer): the size of the vector to normalize
	@param normalizingValue (32 bits float): the value attributed to the most intense signal
	@param lowBound (base 0, 16 bits unsigned integer): function applies to value starting at this specified index (inclusive)
	@param highBound (base 0, 16 bits unsigned integer): function applies to value ending at this specified index (inclusive)
		
	@note In place execution. Results are recorded in the original vector.
	
	@return nothing
*/
{
	float maxValue = FLT_ZERO;
	uint16_t i;
	float *ptrData;
	for (i = lowBound, ptrData = vector; i <= highBound; i++, ptrData++) {
		if (*ptrData > maxValue) {
			maxValue = *ptrData;
		}
	}
	float factor = (normalizingValue / maxValue);
	/* Apply normalization value */
	for (i = lowBound, ptrData = vector; i <= highBound; i++, ptrData++) {
		*ptrData *= factor;
	}
} 


/* General purpose functions */


inline uint16_t PlainDSP::PositionToBin(float position) 
/*!
	@brief Compute the most approaching bin from the target frequency 
	
	@param position (32 bits float): the target frequency
	
	@return the corresponding bin according to the SetAcquisitionEngine() parameters (16 bits unsigned integer)
*/
{
	return(round((_samples * position) / _samplingFrequency));
}


uint16_t PlainDSP::ReversedBits(uint16_t value, uint8_t bits)
/* Compute bits reversal */
{	
	uint16_t result = 0;
	for (uint8_t i = 0; i < bits; i++) {
		result <<= 1;
		result |= (value & 0x01);		
		value >>= 1;
	}
	return(result);
}

inline void PlainDSP::Swap(float *x, float *y) 
/* Swap floats: this swap method is faster than the Xor method */
{
	float temp = *x;
	*x = *y;
	*y = temp;
}


void PlainDSP::RollDown(float *ptr, uint16_t steps, uint16_t upLimit) 
/* Complementary function used in the GetScanData function, triggered mode */
{
	for (uint16_t i = 0; i < steps; i++) {
		float *tempPtr = ptr;
		for (uint16_t j = 0; j < (upLimit - 1); j++, tempPtr++) {
			Swap ((tempPtr + 1), (tempPtr));		
		}
	}	
}


float* PlainDSP::CreateVector(uint16_t size)
/*!
	@brief Allocate a new vector of data

	@param size (16 bits unsigned integer): the size of the vector to create in number of 
		records

	@note The vector is not cleared and may contain random values (see ClearVector())
	@note The vector is allocated in the SRAM, if this memory space overflows, this function hangs
	
	@return pointer to the newly allocated vector (pointer to 32 bits floats)
*/
{
	float* vAuxiliary = (float*)malloc(size * sizeof(float)); 
	while(vAuxiliary == NULL);
	return (vAuxiliary);
}


void PlainDSP::ClearVector(float* vector, uint16_t vectorSize)
/*!
	@brief Clears the content of the specified vector of data

	Fill all places with 0.0s 
	
	@param vector (pointer to 32 bits floats) to the target vector
	@param vectorSize (16 bits unsigned integer) the number of records to set to 0.0, this function is performed from the index 0 up to vectorSize - 1

	@note The memory allocation stays the same after clearing the vector
	
	@return nothing
*/
{
	uint16_t i;
	for (i = 0; i < vectorSize; i++, vector++) {
		*vector= FLT_ZERO;
	}	
}


void PlainDSP::ClearVector(uint8_t dataType) 
/*!
	@brief Clears the content of the specified vector of data
	
	Fill all places with 0.0s
	
	@param dataType (8 bits unsigned integer): type of data to be cleared, as per @ref DSP_DAT_TYP_x constants

	@note The memory allocation stays the same after clearing the vector

	@return nothing
*/
{
	float *ptrData = GetDataAddress(dataType);
	ClearVector(ptrData, _samples);
} 


void PlainDSP::AddVectors(float* augendVector, float* addendVector, uint16_t vectorSize)
/*!
	@brief Add the values from addend vector to the values contained in the augend vector

	translates in augendVector += addendVector
	
	@param augendVector augend vector (pointer to 32 bits floats)
	@param addendVector addend vector (pointer to 32 bits floats)
	@param vectorSize (base 0, 16 bits unsigned integer): the number of records in both vectors

	@note Both vectors must have the same size
		
	@return nothing
*/
{
	
	for (uint16_t i = 0; i < vectorSize; i++, augendVector++, addendVector++) {
		*augendVector += *addendVector;
	}
}

void PlainDSP::SubtractVectors(float* minuendVector, float* subtrahendVector, uint16_t vectorSize)
/*!
	@brief Subtract the values of the subtrahend vector from the values contained in the minuend vector

	translates in minuendVector -= subtrahendVector 
	
	@param minuendVector minuend vector (pointer to 32 bits floats)
	@param subtrahendVector subtrahend vector (pointer to 32 bits floats)
	@param vectorSize (base 0, 16 bits unsigned integer): the number of records in both vectors

	@return nothing
*/
{
	for (uint16_t i = 0; i < vectorSize; i++, minuendVector++, subtrahendVector++) {
		*minuendVector -= *subtrahendVector;
	}
}


void PlainDSP::MultiplyVectors(float* multiplicandVector, float* multiplierVector, uint16_t vectorSize)
/*!
	@brief Multiply the values from the multiplicand vector by the values contained in the multiplier vector

	translates in multiplicandVector *= multiplierVector  
	
	@param multiplicandVector multiplicand vector (pointer to 32 bits floats)
	@param multiplierVector multiplier vector (pointer to 32 bits floats)
	@param vectorSize (base 0, 16 bits unsigned integer): the number of records in both vectors

	@return nothing

*/
{
	for (uint16_t i = 0; i < vectorSize; i++, multiplicandVector++, multiplierVector++) {
		*multiplicandVector *= *multiplierVector;
	}
}

void PlainDSP::DivideVectors(float* dividendVector, float* divisorVector, uint16_t vectorSize)
/*!
	@brief Divide the values from dividend vector by the values contained in the divisor vector

	dividendVector /= divisorVector  
	
	@param dividendVector dividend vector (pointer to 32 bits floats)
	@param divisorVector divisor vector (pointer to 32 bits floats)
	@param vectorSize (base 0, 16 bits unsigned integer): the number of records in both vectors

	@return nothing
*/
{
	for (uint16_t i = 0; i < vectorSize; i++, dividendVector++, divisorVector++) {
		*dividendVector /= *divisorVector;
	}
}

void PlainDSP::CopyVectors(float* destinationVector, float* sourceVector, uint16_t vectorSize) 
/*!
	@brief Copy the values from the source vector to the destination vector

	destinationVector = sourceVector  
	
	@param destinationVector destination vector (pointer to 32 bits floats)
	@param sourceVector source vector (pointer to 32 bits floats)
	@param vectorSize (base 0, 16 bits unsigned integer): the number of records in both vectors

	@return nothing
*/
{
	for (uint16_t i = 0; i < vectorSize; i++, destinationVector++, sourceVector++) {
		*destinationVector = *sourceVector;
	}
}

/* Interrupts management */

ISR(TIMER1_COMPB_vect)
/* Invoked on completion of counting (Compare mode) */
{
	/* Do not ask me why, the presence of this function is mandatory!!! */
}


ISR(ADC_vect)
/* Invoked on completion of conversions */
{
	if (_dataAcqStatus == DSP_WAITING) {
		/* When both LSBs and MSBs are to be read, LSBs MUST be read first */
		_adcLSB = ADCL;
		_adcMSB = ADCH;
		_dataAcqStatus = DSP_TRIGGERED; /* Set new acquisition status */
	}
}  

