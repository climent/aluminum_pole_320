void readAccel() {
  /* Get a new sensor event */
  sensors_event_t event;
  mma.getEvent(&event);
  accel = sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2));
  vectorDiff = constrain(sqrt(pow((9.8 - accel), 2)), 0, 30);
//  vectorDiff = map(vectorDiff, 0, sqrt(pow((9.8 - accel), 2)), 0, 255);
  if (vectorDiff > currentDiff) currentDiff = vectorDiff;
}
