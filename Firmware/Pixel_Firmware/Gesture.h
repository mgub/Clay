#ifndef GESTURE_H
#define GESTURE_H

#include "Gestures.h"

#define MAX_INTEGER_VALUE 32767

#define ANNOUNCE_GESTURE_AT_REST 1
//#define ANNOUNCE_GESTURE_AT_REST_ON_TABLE 1
//#define ANNOUNCE_GESTURE_AT_REST_IN_HAND 2
//#define ANNOUNCE_GESTURE_PICK_UP 3
//#define ANNOUNCE_GESTURE_PLACE_DOWN 4
#define ANNOUNCE_GESTURE_SWING 2

#define ANNOUNCE_GESTURE_TAP_AS_LEFT 3 // 8
#define ANNOUNCE_GESTURE_TAP_AS_RIGHT 4 // 9

#define ANNOUNCE_GESTURE_SHAKE 5 // 7

#define ANNOUNCE_GESTURE_TILT_LEFT 6 // 5
#define ANNOUNCE_GESTURE_TILT_RIGHT 7 // 6
#define ANNOUNCE_GESTURE_TILT_FORWARD 8
#define ANNOUNCE_GESTURE_TILT_BACKWARD 9

#define REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT 13
#define REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT 14

#define CONFIRM_GESTURE_TAP_AS_LEFT 15
#define CONFIRM_GESTURE_TAP_AS_RIGHT 16

//#define ANNOUNCE_UNSEQUENCE 17 // Same as "ANNOUNCE_GESTURE_SHAKE" ?

#define ACTIVATE_MODULE_OUTPUT 20
#define DEACTIVATE_MODULE_OUTPUT 21

boolean classifyMostFrequentGesture = false;
#define PREVIOUS_CLASSIFIED_GESTURE_COUNT 10
int previousClassifiedGestures[PREVIOUS_CLASSIFIED_GESTURE_COUNT];
int previousClassifiedGestureCount = 0;

int previousClassifiedGestureFrequency[GESTURE_COUNT];

/**
 * Sets up gesture sensing.
 */
void setupGestureSensing() {
  for (int i = 0; i < PREVIOUS_CLASSIFIED_GESTURE_COUNT; i++) {
    previousClassifiedGestures[i] = -1;
  }
  previousClassifiedGestureCount = 0;
  
  // Reset the previous classified gesture frequency (the number of times each occured in the short history)
  for (int i = 0; i < GESTURE_COUNT; i++) {
    previousClassifiedGestureFrequency[i] = 0;
  }
}

/**
 * Calculate the deviation of the live gesture sample and the signature gesture sample along only one axis (x, y, or z).
 */
int getGestureAxisDeviation(int gestureSignatureIndex, int axis) { // (int gestureSample[GESTURE_SIGNATURE_SIZE], int gestureCandidate[GESTURE_SIGNATURE_SIZE]) {
  
  int delta = 0; // sum of difference between average x curve and most-recent x data
  
//  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size(); i++) {
//    if (i < liveSample.size() && i < gestureSample.size()) {
//        int difference = abs(gestureSample.get(i) - liveSample.get(i));
//        delta = delta + difference;
//    }
//  }

  // Compare the first 50 points of the gesture signature to the most recent 50 accelerometer data points
  //for (int point = 0; point < GESTURE_SIGNATURE_SIZE; point++) {
  for (int point = 0; point < gestureSignatureSize[gestureSignatureIndex]; point++) {
      int difference = abs(gestureSignature[gestureSignatureIndex][axis][point] - gestureCandidate[axis][point]);
      delta = delta + difference;
  }
  
  return delta;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureDeviation(int classifiedGestureIndex) {
  int deltaTotal = 0;
    
  // Compare the difference between the average sample for each axis and the live sample
  for (int axis = 0; axis < 3; axis++) {
    int delta = getGestureAxisDeviation(classifiedGestureIndex, axis); // gestureSignature[classifiedGestureIndex][axis], gestureCandidate[axis]);
    deltaTotal = deltaTotal + delta;
  }
  
  return deltaTotal;
}

/**
 * Relative instability. How relative is the live sample in comparison to a gesture's signature sample?
 */
int getGestureAxisInstability(int gestureSignatureIndex, int axis) { // (int gestureSample[GESTURE_SIGNATURE_SIZE], int gestureCandidate[GESTURE_SIGNATURE_SIZE]) {
  
  int relativeInstability = 0; // sum of difference between average x curve and most-recent x data
  
//  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size(); i++) {
//    if (i < liveSample.size() && i < gestureSample.size()) {
//        int difference = abs(gestureSample.get(i) - liveSample.get(i));
//        delta = delta + difference;
//    }
//  }

  //for (int point = 0; point < GESTURE_SIGNATURE_SIZE - 1; point++) {
  for (int point = 0; point < gestureSignatureSize[gestureSignatureIndex] - 1; point++) {
      int signatureDifference = abs(gestureSignature[gestureSignatureIndex][axis][point + 1] - gestureCandidate[axis][point]);
      int liveDifference = abs(gestureCandidate[axis][point + 1] - gestureCandidate[axis][point]);
      int instabilityDifference = abs(signatureDifference - liveDifference);
      relativeInstability = relativeInstability + instabilityDifference;
  }
  
  return relativeInstability;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureInstability(int classifiedGestureIndex) {
  int instabilityTotal = 0;
    
  // Compare the difference between the average sample for each axis and the live sample
  for (int axis = 0; axis < 3; axis++) {
    int instability = getGestureAxisInstability(classifiedGestureIndex, axis); // gestureSignature[classifiedGestureIndex][axis], gestureCandidate[axis]);
    instabilityTotal = instabilityTotal + instability;
  }
  
  return instabilityTotal;
}

/**
 * Classify the gesture. Choose the gesture that has a "signature" time series that best  
 * matches the recent window of live data.
 */
int classifyGestureFromTransitions() {
  int minimumDeviationIndex = -1;
  int minimumDeviation = MAX_INTEGER_VALUE;
  
  for (int i = 0; i < GESTURE_COUNT; i++) {
    
      int gestureSignatureIndex = gestureTransitions[classifiedGestureIndex][i]; // Get index of possible gesture
      
      if (gestureSignatureIndex != -1) { // Make sure the transition is valid before continuing
        
        // Calculate the gesture's deviation from the gesture signature
        int gestureDeviation = getGestureDeviation(gestureSignatureIndex);
        int gestureInstability = getGestureInstability(gestureSignatureIndex);

        // Check if the sample's deviation
        if (minimumDeviationIndex == -1 || (gestureDeviation + gestureInstability) < minimumDeviation) {
          minimumDeviationIndex = gestureSignatureIndex;
          minimumDeviation = gestureDeviation + gestureInstability;
        }
    }
  }

  return minimumDeviationIndex;
}

#endif