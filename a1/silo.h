// A Silo


#ifndef SILO_H
#define SILO_H

#include "headers.h"
#include "gpuProgram.h"
#include "buildings.h"


class Silo : public Building {
public:

  // Constructors

  Silo() {}

  Silo( vec3 pos ) : Building( pos ) {
    maxRounds = 15;
    roundsLeft = maxRounds;
  }

  bool canShoot() {
    return (roundsLeft > 0);
  }

  void decrMissiles() {
    roundsLeft--;
  }

  vec3 position() {
    return pos;
  }

  // Draw the silo

  void draw( GPUProgram *gpuProgram ) {
    const int NUM_SEGMENTS = 45; // number of pieces of hemisphere perimeter

    vec3 *verts = new vec3[NUM_SEGMENTS+1];

    verts[0] = pos;

    for (int i=0; i<NUM_SEGMENTS; i++) {
      float theta = (i/(float)(NUM_SEGMENTS-1)) * M_PI;
      verts[i+1] = vec3( pos.x + 0.04 * cos(theta), pos.y + 0.04 * sin(theta), 0 );
    }

    vec3 colorVector;
    float percentRounds = (float) roundsLeft / maxRounds;

    if (percentRounds > 0.5) { // above half ammo
      colorVector = vec3(0.204, 0.659, 0.325); // green
    } else if (percentRounds == 0) { // out of ammo
      colorVector = vec3(0.917, 0.263, 0.208); // red
    } else { // getting low on ammo
      colorVector = vec3(0.984, 0.737, 0.02); // yellow
    }

    gpuProgram->drawVertices( verts, NUM_SEGMENTS+1, GL_TRIANGLE_FAN, colorVector );

    delete [] verts;
  }

private:
  int roundsLeft;
  int maxRounds;
};


#endif
