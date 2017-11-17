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

#ifndef PlainDSP_h /* Prevent loading library twice */
#define PlainDSP_h

/* Include std libraries */
#include <inttypes.h> /* integer types */
#include <avr/interrupt.h> /* interrupts */
#include <math.h> /* sqrt, round */
#include <string.h> /* memcopy */
#include <stdlib.h> /* malloc */
#include <stdint.h>
#include <float.h>

/** @defgroup DSP_OPT_x DSP_OPT_x 
 @brief Data acquisition options 
 
 These options may used together using the pipe character '|' */
/** No option @ingroup DSP_OPT_x */
#define DSP_OPT_NONE 0x00
/** Disable timer 0 @ingroup DSP_OPT_x */
#define DSP_OPT_DIS_TIM_0 0x01
/** Disable timer 2 @ingroup DSP_OPT_x */
#define DSP_OPT_DIS_TIM_2 0x02
/** Enable Atmega internal noise canceler @ingroup DSP_OPT_x */
#define DSP_OPT_NOI_CANCELLER 0x04

/** @defgroup DSP_REF_VOL_x DSP_REF_VOL_x
@brief Voltage references */
/** As per AREF pin voltage @ingroup DSP_REF_VOL_x */
#define DSP_REF_VOL_EXTERNAL 0x00
/** VCC @ingroup DSP_REF_VOL_x */
#define DSP_REF_VOL_DEFAULT 0x01 
/** 1V1 on ATMEGA 168/328 @ingroup DSP_REF_VOL_x */
#define DSP_REF_VOL_INTERNAL 0x03 

/** @defgroup DSP_DAT_TYP_x DSP_DAT_TYP_x
@brief Data type */
/** Data vector (same as real data vector) @ingroup DSP_DAT_TYP_x */
#define DSP_DAT_TYP_DATA 0x00
/** Real part vector (same as data vector) @ingroup DSP_DAT_TYP_x */
#define DSP_DAT_TYP_REAL 0x00
/** Imaginary part vector  @ingroup DSP_DAT_TYP_x */
#define DSP_DAT_TYP_IMAG 0x01

/** @defgroup DSP_x_SMP_FREQUENCY DSP_x_SMP_FREQUENCY
@brief Sampling frequency specifications*/
/** Minimum sampling frequency @ingroup DSP_x_SMP_FREQUENCY */
#define DSP_MIN_SMP_FREQUENCY 0.125
/** Maximum sampling frequency @ingroup DSP_x_SMP_FREQUENCY */
#define DSP_MAX_SMP_FREQUENCY 80000.0

/** @defgroup DSP_x DSP_x
@brief FFT direction constant */
/** Forward FFT (time domain to frequency domain) @ingroup DSP_x */
#define DSP_FORWARD 0x01
/** Inverse FFT (frequency domain to time domain) @ingroup DSP_x */
#define DSP_INVERSE 0x00

/** @defgroup DSP_WIN_TYP_x DSP_WIN_TYP_x 
@brief Windowing type */
/** Rectangle (Box car)  @ingroup DSP_WIN_TYP_x */
#define DSP_WIN_TYP_RECTANGLE 0x00 
/** Hamming  @ingroup DSP_WIN_TYP_x */
#define DSP_WIN_TYP_HAMMING 0x01 
/** Hann  @ingroup DSP_WIN_TYP_x */
#define DSP_WIN_TYP_HANN 0x02 
/** Triangle (Bartlett)  @ingroup DSP_WIN_TYP_x */
#define DSP_WIN_TYP_TRIANGLE 0x03 
/** Blackmann  @ingroup DSP_WIN_TYP_x */
#define DSP_WIN_TYP_BLACKMAN 0x04
/** Flat top  @ingroup DSP_WIN_TYP_x */
#define DSP_WIN_TYP_FLT_TOP 0x05
/** Welch @ingroup DSP_WIN_TYP_x */
#define DSP_WIN_TYP_WELCH 0x06 

/** @defgroup DSP_SCL_TYP_x DSP_SCL_TYP_x
@brief Frequency spectrum scale types */
/** Magnitude scale @ingroup DSP_SCL_TYP_x */
#define DSP_SCL_TYP_MAGNITUDE 0x00
/** Amplitude scale @ingroup DSP_SCL_TYP_x */
#define DSP_SCL_TYP_AMPLITUDE 0x01
/** RMS scale @ingroup DSP_SCL_TYP_x */
#define DSP_SCL_TYP_RMS 0x02

/*! @brief This structure contains all information of a peak. */
struct strPeakProperties {
	float position;/*!< the peak position in the frequency domain */
	float height;/*!< the peak height, ie its amplitude */
	uint16_t bin;/*!< the peak bin, see the FFT algorithm details for more information */
};

/*! @mainpage 
The PlainDSP library is a Data Acquisition and Digital Signal Processing library.
This library works with all Arduino UNO compatible boards
*/

/*!
 
	@brief Data Acquisition and Digital Signal Processing library
	@version Revision 1g

	@author HL2 group
	@copyright 
	@parblock
	This program and related paper documentation are owned by HL2 group SAS. 
	The program is subject to the license GNU GPL version 3, which content 
	is available in the file attached to this program. This license allows 
	you to use, reproduce and adapt this program for private, educational, 
	research purposes. 

	If you intend to (i) use this program on a large scale and/or (ii) adapt 
	this code, for COMMERCIAL PURPOSES, please contact HL2 at the following 
	email address : contact@hl2.fr in order to be granted a commercial license, 
	subject to specific terms and conditions.
	@endparblock
*/
class PlainDSP
{
public:
	/* Constructor */
	PlainDSP(void);
	/* Destructor */
	~PlainDSP(void);
	/* Functions */
	void AddVectors(float* augendVector, float* addendVector, uint16_t vectorSize);
	float Average(void);
	float Average(uint16_t lowBound, uint16_t highBound);
	void ClearVector(uint8_t dataType);
	void ClearVector(float* vector, uint16_t vectorSize);
	void ComplexToReal(uint8_t scaleType);
	void ComputeFFT(uint8_t dir); 
	void ComputeForwardFFT(void);
	void ComputeInverseFFT(void);
	void CopyVectors(float* destinationVector, float* sourceVector, uint16_t vectorSize);
	uint8_t Channel(uint8_t adcChannel);
	uint8_t Channel(void);
	float* CreateVector(uint16_t size);
	void DivideVectors(float* dividendVector, float* divisorVector, uint16_t vectorSize);	
	void Gain(float gain);
	void Gain(float gain, uint16_t lowBound, uint16_t highBound);
	float *GetDataAddress(uint8_t dataType);
	void GetScanData(void);
	void GetScanData(uint16_t triggerLevel, uint8_t horizontalPosition, uint16_t timeOut = 0);
	void InterpolatedPeak(float targetPosition, struct strPeakProperties *result);
	void MajorPeak(struct strPeakProperties *result);
	void MajorPeak(float loFrequency, float upFrequency, struct strPeakProperties *result); 
	float Max(void); 
	float Max(uint16_t lowBound, uint16_t highBound); 
	float Min(void); 
	float Min(uint16_t lowBound, uint16_t highBound); 
	void MultiplyVectors(float* multiplicandVector, float* multiplierVector, uint16_t vectorSize);
	void Normalize(float normalizingValue); 
	void Normalize(float normalizingValue, uint16_t lowBound, uint16_t highBound); 
	void Normalize(float* vector, uint16_t size, float normalizingValue);
	void Normalize(float* vector, uint16_t size, float normalizingValue, uint16_t lowBound, uint16_t highBound);
	void Offset(float offset);
	void Offset(float offset, uint16_t lowBound, uint16_t highBound);
	inline uint16_t PositionToBin(float position);
	float ReadData(uint16_t index);	
	float ReadData(uint8_t dataType, uint16_t index);
	float ReadData(float *ptrData, uint16_t index);
	void ReleaseAcquisitionEngine(void);
	void ReleaseVector(uint8_t dataType);
	void ResetOffset(void);
	float RMS(void);
	float RMS(uint16_t lowBound, uint16_t highBound);
	float *SetAcquisitionEngine(uint8_t adcChannel, uint8_t refVoltage, float samplingFrequency, uint16_t samples, uint8_t options);
	void SubtractVectors(float* minuendVector, float* subtrahendVector, uint16_t vectorSize);
	void TargetPeak(float targetPosition, float tolerance, struct strPeakProperties *result);
	void Windowing(uint8_t windowType, uint8_t dir); 
	void WriteData(uint16_t index, float data);
	void WriteData(uint8_t dataType, uint16_t index, float data);
	
private:
	/* Data acquisition parameters */
 	uint8_t _refVoltage;
 	uint16_t _samples;
	uint8_t _expSamples;
 	uint16_t _lastSamples;
	uint8_t _adcChannel;
 	float _samplingFrequency;
	float *_vData;
	float *_vImag;
	uint8_t _options;
	/* Functions */
	uint8_t Exponent(uint16_t value);
	uint16_t ReversedBits(uint16_t value, uint8_t bits);
	void RollDown(float *ptr, uint16_t steps, uint16_t upLimit);
	inline void Swap(float *x, float *y);
};

#endif /* PlainDSP_h */
