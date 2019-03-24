#include "Main.h"

SerialPort port("\\\\.\\COM3");

fVec3 grav_dir;
float G = 10.2; //Average of sensor
bool first = true;

uint16_t last_time;

float adt = 0;

void readData() {
  byte* data = new byte[14];
  
  float dt = 0;

  int read = 0;

  while (read < 14) {
    read += port.readSerialPort((char*)data + read, 14 - read);
  }

  for (int i = 0; i < 7; i++) {
    swap(data[2 * i], data[2 * i + 1]);
  }

  if (read != 14) {
    throw 1;
  }

  fVec3 acc;
  for (int i = 0; i < 3; i++) {
    acc[i] = ((int16_t*)data)[i];
  }
  acc *= 2 * 9.81 / 32768.0;

  fVec3 gy;
  for (int i = 0; i < 3; i++) {
    gy[i] = ((int16_t*)data)[i + 3];
  }

  uint16_t new_time = ((uint16_t*)data)[6];

  dt = ((new_time + 65536 - last_time) % 65536) / 1000.0;

  if (first) {
    grav_dir = acc;
    first = false;
    dt = 0;
  }

  gy *= 250 * (CONS_PI / 180.0) / 32768.0;

  float conf = 0.3 / (0.3 + pow(acc.length() - G, 2)) * 0.3 / (0.3 + gy.sqrlen()) * 0.03;

  grav_dir = grav_dir * (1 - conf) + acc * conf;
  G = G * (1 - conf) + acc.length() * conf;

  float ang = -gy.length();
  if (abs(ang) > 0.0001) {
    ang *= dt;
    gy.norm();
    grav_dir = grav_dir * cos(ang) + crs(gy, grav_dir) * sin(ang) + gy * dot(gy, grav_dir) * (1 - cos(ang));
  }

  grav_dir *= (G / grav_dir.length());

  fVec3 linac = grav_dir - acc;

  adt = adt * 0.99 + dt * 0.01;
  //cout << setprecision(3) << showpos << grav_dir.x << "     \t     " << grav_dir.y << "      \t    " << grav_dir.z << "      \r";
  cout << adt << "    \r";

  last_time = new_time;

  delete[] data;
}

int main() {
  while (true) {
    readData();
  }

  return 0;
}