
"use strict";

let FiducialDetection = require('./FiducialDetection.js');
let MarkerDetection = require('./MarkerDetection.js');
let MarkerWithCovariance = require('./MarkerWithCovariance.js');
let Fiducial = require('./Fiducial.js');
let MarkerStamped = require('./MarkerStamped.js');
let MarkerWithCovarianceArray = require('./MarkerWithCovarianceArray.js');
let Marker = require('./Marker.js');
let MarkerWithCovarianceStamped = require('./MarkerWithCovarianceStamped.js');

module.exports = {
  FiducialDetection: FiducialDetection,
  MarkerDetection: MarkerDetection,
  MarkerWithCovariance: MarkerWithCovariance,
  Fiducial: Fiducial,
  MarkerStamped: MarkerStamped,
  MarkerWithCovarianceArray: MarkerWithCovarianceArray,
  Marker: Marker,
  MarkerWithCovarianceStamped: MarkerWithCovarianceStamped,
};
