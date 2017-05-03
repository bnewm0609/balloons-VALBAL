/*
  Stanford Student Space Initiative
  Balloons | VALBAL | April 2017
  Davy Ragland | dragland@stanford.edu
  Michal Adamkiewicz | mikadam@stanford.edu

  File: Filters.cpp
  --------------------------
  Implementation of Filters.h
*/

#include "Filters.h"

/**********************************  SETUP  ***********************************/
/*
 * Function: init
 * -------------------
 * This function initializes the filter objects.
 */
bool Filters::init() {
	bool sucess = true;
    findLastAccepted();
	return sucess;
}

/********************************  FUNCTIONS  *********************************/
/*
 * Function: enableSensors
 * -------------------
 * This function selectivly enables and disables the
 * sensors included in fused calculations.
 */
void Filters::enableSensors(bool BMP1Enable, bool BMP2Enable, bool BMP3Enable, bool BMP4Enable) {
  enabledSensors[0] = BMP1Enable;
  enabledSensors[1] = BMP2Enable;
  enabledSensors[2] = BMP3Enable;
  enabledSensors[3] = BMP4Enable;
	numSensors = 0;
	for (size_t i = 0; i < 4; i++) if (enabledSensors[i]) numSensors++;
}

/*
 * Function: getTemp
 * -------------------
 * This function returns a sensor fused temperature.
 */
double Filters::getTemp(double RAW_TEMP_1, double RAW_TEMP_2, double RAW_TEMP_3, double RAW_TEMP_4) {
  double temp = 0;
  if (enabledSensors[0]) temp += RAW_TEMP_1;
  if (enabledSensors[1]) temp += RAW_TEMP_2;
  if (enabledSensors[2]) temp += RAW_TEMP_3;
  if (enabledSensors[3]) temp += RAW_TEMP_4;
  return temp / numSensors;
}

/*
 * Function: getPressure
 * -------------------
 * This function returns a bounds checked pressure mean
 */
double Filters::getPressure(double RAW_PRESSURE_1, double RAW_PRESSURE_2, double RAW_PRESSURE_3, double RAW_PRESSURE_4,double pressureBaselineArg) {
    pressureBaseline = pressureBaselineArg;

    pressures[0] = RAW_PRESSURE_1;
    pressures[1] = RAW_PRESSURE_2;
    pressures[2] = RAW_PRESSURE_3;
    pressures[3] = RAW_PRESSURE_4;

	for(int i = 0; i<4;i++) if(!((MIN_PRESURE < pressures[i]) && (pressures[i] < MAX_PRESURE))) markFailure(i);

	int numSensors = 0;
	for (size_t i = 0; i < 4; i++) if (enabledSensors[i]) numSensors++;
	// Calculate mean of sensors which passed

	double press = 0;
	for(int i = 0; i<4;i++) if (enabledSensors[i]) press += pressures[i];

  return press / numSensors;
}

/***************************  GET FUNCTIONS  **********************************/

/*
 * Function: getAscentRate
 * -------------------
 * This function returns a higher precision altitude value
 * based on the US 1976 Standard Atmosphere.
 */
double Filters::getAscentRate() {


      float meanAscentRate = 0;
      int acceptedStreams = 0;


    for(int i = 0; i < 4; i++){


        float downSampledAltitudes[ALTITUDE_BUFFER_SIZE/ALTITUDE_DOWNSAMPLE_SIZE];
        int j = 0;

        float sum = 0;
        int numberOfDataPoints = 0;

        //Downsample
        for (int t = 0; t < ALTITUDE_BUFFER_SIZE; t++){
            int index = (altitudeIndex + t +1) % ALTITUDE_BUFFER_SIZE;
            sum += altitudeBuffer[i][index];
            numberOfDataPoints++;

            if(numberOfDataPoints == ALTITUDE_DOWNSAMPLE_SIZE || index == altitudeIndex){
                downSampledAltitudes[j] = sum/numberOfDataPoints;
                numberOfDataPoints = 0;
                j++;
            }
        }

        float numerator = 0;
        float denominator = 0;

        // Dejankiy: denominator is calculating a constant!
        for(int j = 0; j < ALTITUDE_BUFFER_SIZE/ALTITUDE_DOWNSAMPLE_SIZE; j++){
            numerator += (j - ALTITUDE_BUFFER_SIZE/(2*ALTITUDE_DOWNSAMPLE_SIZE)) * (downSampledAltitudes[j] - meanAltitudes[i]);
            denominator += pow((j - ALTITUDE_BUFFER_SIZE/(2*ALTITUDE_DOWNSAMPLE_SIZE)),2);
        }

        // 1000 comes from LOOP_INTERVAL being in miliseconds
        meanAscentRates[i] = (((float)1000*ALTITUDE_DOWNSAMPLE_SIZE)/(LOOP_INTERVAL * ALTITUDE_BUFFER_SIZE)) * numerator/denominator;
        if(sensorsAccepted[i]){
            meanAscentRate += meanAscentRates[i];
            acceptedStreams++;
        }
    }

    return meanAscentRate/acceptedStreams;

}

/*
 * Function: getAltitude
 * -------------------
 * This function returns an error checked and smoothed
 * altitude value
 */
double Filters::getAltitude() {

  filterAltitudes();

  float meanAltitude = 0;
  int acceptedStreams = 0;

  for(int i = 0; i<4;i++){
      float altitudesSum =0;
      sensorsAccepted[i] = true;

      for(int t = 0; t<ALTITUDE_BUFFER_SIZE;t++){
          altitudesSum += altitudeBuffer[i][t];
          sensorsAccepted[i] = sensorsAccepted[i] && !altitudeErrors[i][t];
      }

      meanAltitudes[i] = altitudesSum / ALTITUDE_BUFFER_SIZE;
      if(sensorsAccepted[i]){
          meanAltitude += meanAltitudes[i];
          acceptedStreams++;
      }
  }
  return meanAltitude / acceptedStreams;
}


/********************************  CHECKERS  **********************************/


/*
 * Function: filterAltitudes
 * -------------------
 * This function returns a higher precision altitude value
 * based on the US 1976 Standard Atmosphere.
 */
void Filters::filterAltitudes() {
  altitudeIndex = (altitudeIndex + 1) % ALTITUDE_BUFFER_SIZE;
  for(int i = 0; i<4;i++) altitudeBuffer[i][altitudeIndex] = calculateAltitude(pressures[i]);

  consensousCheck();
  velocityCheck();
  findLastAccepted();
}

/*
 * Function: findLastAccepted
 * -------------------
 * This function checkes if velocity since last accepted
 * altitude is within an acceptible range
 */
void Filters::findLastAccepted() {

    for(int i = 0; i<4;i++){
        if (enabledSensors[i]){
            lastAcceptedAltitudes[i] = altitudeBuffer[i][altitudeIndex];
            lastAcceptedTimes[i] = millis();
        }
    }
}


/*
 * Function: velocityCheck
 * -------------------
 * This function checkes if velocity since last accepted
 * altitude is within an acceptible range
 */
void Filters::velocityCheck() {
    for(int i = 0; i<4;i++){
        // 1000 comes from times being in miliseconds
        if (fabs(1000* (altitudeBuffer[i][altitudeIndex] - lastAcceptedAltitudes[i])/(millis() - lastAcceptedTimes[i])) > MAX_VELOCITY){
            markFailure(i);
        }
    }
}


/*
 * Function: consensousCheck
 * -------------------
 * This function returns a higher precision altitude value
 * based on the US 1976 Standard Atmosphere.
 */
void Filters::consensousCheck(){
    int maxAgreement = 0;
    int maxSensors = 0;
    int minDistance = 0;

    // for each sensor combination
    for(int activeSensors = 1; activeSensors<16; activeSensors++){
        int numberOfSensors = 0;
        int numberOfCorrectSensors = 0;
        float meanAltitude = 0;
        float distance = 0;

        // calculate mean
        for(int sensor = 0; sensor < 4; sensor++){
            if( 1 & (activeSensors>>sensor)){
                numberOfSensors++;
                meanAltitude += altitudeBuffer[sensor][altitudeIndex];
            }
        }
        meanAltitude /= numberOfSensors;

        // count sensors in range
        for(int sensor = 0; sensor < 4; sensor++){
            if(1 & (activeSensors>>sensor)){
                distance += pow(altitudeBuffer[sensor][altitudeIndex] - meanAltitude,2);
                if(fabs(altitudeBuffer[sensor][altitudeIndex] - meanAltitude) < MAX_CONSENSUS_DEVIATION) numberOfCorrectSensors +=1;
            }
        }

        // if arangemnt is better
        if(numberOfCorrectSensors > maxSensors || (numberOfCorrectSensors == maxSensors && distance < minDistance)){
            maxAgreement = activeSensors;
            maxSensors = numberOfSensors;
            minDistance = distance;
        }
    }

    for(int sensor = 0; sensor < 4; sensor++){
        if(!(1 & (maxAgreement>>sensor))){
            markFailure(sensor);
        }
    }
}

/*********************************  HELPERS  **********************************/



/*
 * Function: calculateAltitude
 * -------------------
 * This function returns a higher precision altitude value
 * based on the US 1976 Standard Atmosphere.
 */
double Filters::calculateAltitude(double pressure) {
  double calculatedAltitude;
  if (pressure > 22632.1) calculatedAltitude = (44330.7 * (1 - pow(pressure / pressureBaseline, 0.190266)));
  else calculatedAltitude =  -6341.73 * log((0.176481 * pressure) / 22632.1);

  return calculatedAltitude;
}

/*
 * Function: getNumRejections
 * -------------------
 * This function returns the numer of rejections
 * a specific sensor has encountered.s
 */
uint32_t Filters::getNumRejections(uint8_t sensor) {
	return rejectedSensors[sensor - 1];
}

/*
 * Function: markFailure
 * -------------------
 * This function marks a specific
 * sensor failure.
 */
void Filters::markFailure(uint8_t sensor){
    if(enabledSensors[sensor]) rejectedSensors[sensor]++;
	enabledSensors[sensor] = false;
    altitudeErrors[sensor][altitudeIndex] = true;
}
