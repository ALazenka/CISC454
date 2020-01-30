// state.cpp


#include "headers.h"

#include "state.h"


// Draw each of the objects in the world


void State::draw() 

{
  int i;

  gpuProgram->activate();

  glUniformMatrix4fv( glGetUniformLocation( gpuProgram->id(), "M"), 1, GL_TRUE, &M[0][0] );

  silos[0].draw( gpuProgram );
  
  for (i=0; i<silos.size(); i++)
    silos[i].draw( gpuProgram );

  for (i=0; i<cities.size(); i++)
    cities[i].draw( gpuProgram );

  for (i=0; i<missilesIn.size(); i++)
    missilesIn[i].draw( gpuProgram );

  for (i=0; i<missilesOut.size(); i++)
    missilesOut[i].draw( gpuProgram );

  for (i=0; i<explosions.size(); i++)
    explosions[i].draw( gpuProgram );

  gpuProgram->deactivate();
}


// Update the state of the world after some time interval, deltaT
//
// CHANGE ALL OF THIS FUNCTION


void State::updateState( float deltaT )

{
  int i, cityIndex, siloIndex, missileIndex;

  // Update the time

  currentTime += deltaT;

  // Generate some new missiles.  The rate of missile generation
  // should increase with time.
  //
  // CHANGE THIS

  float boundary = 0.995 - (currentTime/100000);

  if (randIn01() > boundary) {	// New missile 

    vec3 startPos = vec3(randIn01(), worldTop, 0);
    const float speed = 0.05;
    
    missilesIn.add( Missile( startPos, // source
			     speed * (vec3(randIn01(), -0.1, 0) - startPos),         // velocity
			     0,                              // destination y
			     vec3( 1,1,0 ) ) );             // colour
  }

   for (i = 0; i < missilesIn.size(); i++) {
    Missile currMissile = missilesIn[i];
    if (currMissile.hasReachedDestination()) {
      explosions.add(Circle(currMissile.position(), 0.1, 0.03, vec3(1, 0, 0)));
      missilesIn.remove(i);
      i--;
    }
  }

  for (i = 0; i < missilesOut.size(); i++) {
    Missile currMissile = missilesOut[i];
    if (currMissile.hasReachedDestination()) {
      explosions.add(Circle(currMissile.position(), 0.1, 0.03, vec3(1, 0, 0)));
      missilesOut.remove(i);
      i--;
    }
  }

  // Look for terminating explosions

  for (i = 0; i < explosions.size(); i++) {
    Circle currExplosion = explosions[i];
    if (currExplosion.radius() >= currExplosion.maxRadius()) {

      for (cityIndex = 0; cityIndex < cities.size(); cityIndex++) {
        City currentCity = cities[cityIndex];
        // check if city has been hit
        if (currentCity.isHit(currExplosion.position(), currExplosion.radius())) {
          cities.remove(cityIndex);
        }
      }

      for (siloIndex = 0; siloIndex < silos.size(); siloIndex++) {
        Silo currSilo = silos[siloIndex];
        // check if silo has been hit
        if ((currSilo.position() - currExplosion.position()).length() <= currExplosion.radius()) {
          silos.remove(siloIndex);
        }
      }

      for (missileIndex = 0; missileIndex < missilesIn.size(); missileIndex++) {
        Missile currMissile = missilesIn[missileIndex];
        // check for destruction of incoming missile
        if ((currMissile.position() - currExplosion.position()).length() <= currExplosion.radius()) {
          missilesIn.remove(missileIndex);
        }
      }

      explosions.remove(i);
      i--;
    }
  }

  for (i = 0; i < missilesIn.size(); i++)
    missilesIn[i].move( deltaT );

  for (i = 0; i < missilesOut.size(); i++)
    missilesOut[i].move( deltaT );

  for (i = 0; i < explosions.size(); i++)
    explosions[i].expand( deltaT );

}


// Fire a missile

void State::fireMissile( int siloIndex, float x, float y )

{

  const float speed = 0.3;
    
  if (siloIndex < silos.size() && silos[siloIndex].canShoot()) {

    silos[siloIndex].decrMissiles();

    // CHANGE THIS

    missilesOut.add( Missile( silos[siloIndex].position(),           // source
			      speed * (vec3(x,y,0) - silos[siloIndex].position()).normalize(), // velocity
			      y,		                     // destination y
			      vec3( 0,1,1 ) ) );                     // colour
  }
}



// Create the initial state of the world


void State::setupWorld()

{
  // Keep track of the time

  currentTime = 0;

  timeOfIncomingFlight = 6;	// 6 seconds for incoming missile to arrive

  // Seed the random number generator

#ifdef _WIN32
  srand( 12345 );
#else
  struct timeb t;
  ftime( &t );
  srand( t.time );
#endif

  // Silos are added in order left, middle, right so that they have
  // indices 0, 1, and 2 (respectively) in the "silos" array.

  silos.add(  Silo( vec3( 0.1, 0, 0 ) ) );
  silos.add(  Silo( vec3( 0.5, 0, 0 ) ) );
  silos.add(  Silo( vec3( 0.9, 0, 0 ) ) );

  // Cities on the left

  cities.add( City( vec3( 0.2, 0, 0 ) ) );
  cities.add( City( vec3( 0.3, 0, 0 ) ) );
  cities.add( City( vec3( 0.4, 0, 0 ) ) );

  // Cities on the right

  cities.add( City( vec3( 0.6, 0, 0 ) ) );
  cities.add( City( vec3( 0.7, 0, 0 ) ) );
  cities.add( City( vec3( 0.8, 0, 0 ) ) );
}
