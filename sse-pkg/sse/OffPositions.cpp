/*******************************************************************************

 File:    OffPositions.cpp
 Project: OpenSonATA
 Authors: The OpenSonATA code is the result of many programmers
          over many years

 Copyright 2011 The SETI Institute

 OpenSonATA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OpenSonATA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
 
 Implementers of this code are requested to include the caption
 "Licensed through SETI" with a link to setiQuest.org.
 
 For alternate licensing arrangements, please contact
 The SETI Institute at www.seti.org or setiquest.org. 

*******************************************************************************/



#include "OffPositions.h" 
#include "ast_const.h"  // for OMEGA, rotational angular velocity of earth, rad/s 
#include "Assert.h"
#include "SseException.h"
#include "SseAstro.h"
#include <cmath>
#include <sstream>

using namespace std;

static const double NorthPoleDecRads(0.5 * PI);
static const double SouthPoleDecRads(-0.5 * PI);
static const int PrintPrecision(6);

OffPositions::Position::Position(double raRads, double decRads)
   : raRads_(raRads),
     decRads_(decRads)
{
}

OffPositions::Position::Position()
   : raRads_(0),
     decRads_(0)
{
}

OffPositions::AvoidPosition::AvoidPosition(const string & type,
					   double raRads, double decRads)
   :
   type_(type),
   pos_(raRads, decRads)
{
}

OffPositions::OffPositions(double synthBeamsizeRads,
			   double minPointSepSynthBeamsizes,
			   double primaryCenterRaRads,
			   double primaryCenterDecRads,
			   double primaryBeamsizeRads)
   :
   synthBeamsizeRads_(synthBeamsizeRads),
   minPointSepSynthBeamsizes_(minPointSepSynthBeamsizes),
   minPointSepRads_(synthBeamsizeRads * minPointSepSynthBeamsizes),
   primaryCenterRaRads_(primaryCenterRaRads),
   primaryCenterDecRads_(primaryCenterDecRads),
   primaryBeamsizeRads_(primaryBeamsizeRads)
{
   Assert(synthBeamsizeRads > 0.0);
   Assert(minPointSepSynthBeamsizes > 0.0);
   Assert(primaryBeamsizeRads > 0.0);
}

OffPositions::~OffPositions()
{
}


double OffPositions::getMinPointSepRads() const
{
   return minPointSepRads_;
}

double OffPositions::getPrimaryBeamsizeRads() const
{
   return primaryBeamsizeRads_;
}


/*
* Check that the given off position is fully contained within
* the primary FOV.  I.e., the position must be no closer than 1/2 the 
* synthesized beamwidth from the edge of the primary FOV
* (times a small fudge factor so that points at exactly 1/2 the
* synth beamwidth are not rejected).
* Returns true if the position is within range.
*/

bool OffPositions::isOffPositionInPrimaryFov(double raRads,
					  double decRads)
{
   const double halfPrimaryBeamsizeRads(primaryBeamsizeRads_ * 0.5);
   const double halfSynthBeamsizeRads(synthBeamsizeRads_ * 0.5);
   const double maxDistAdjustFactor(0.99);

   double maxDistRads(halfPrimaryBeamsizeRads - (halfSynthBeamsizeRads
      * maxDistAdjustFactor));

   return isPositionInPrimaryFov(raRads, decRads, maxDistRads);

}

/*
* Check that the given target is contained within
* the primary FOV.    
* A small fudge factor is added that that targets
* right at the edge are not rejected.
* Returns true if the position is within range.
*/

bool OffPositions::isTargetInPrimaryFov(double raRads,
					double decRads)
{
   const double halfPrimaryBeamsizeRads(primaryBeamsizeRads_ * 0.5);
   const double maxDistAdjustFactor(1.01);
   double maxDistRads(halfPrimaryBeamsizeRads * maxDistAdjustFactor);

   return isPositionInPrimaryFov(raRads, decRads, maxDistRads);
}

/*
* Check that the given position is contained within
* the primary FOV.  I.e., the position must be no closer than 
* distToPrimaryEdgeRads from the center.
* Returns true if the position is within range.
*/
bool OffPositions::isPositionInPrimaryFov(double raRads, double decRads,
					  double distToPrimaryEdgeRads)
{
   double diffRads = SseAstro::angSepRads(
      primaryCenterRaRads_, primaryCenterDecRads_,
      raRads, decRads);

   if (diffRads <= distToPrimaryEdgeRads)
   {
      return true;
   }

   return false;
}


/*
  Verify that the position is no closer than the min separation
 from all the positions on the positionsToAvoid list.
 Returns true if the position is valid.
  */

bool OffPositions::isPositionAvailable(double raRads,
				       double decRads)
{
   // Make the minimum acceptable distance slightly smaller than 
   // the minimum pointing separation, so that the proposed position doesn't
   // get rejected for being right at the min pointing sep distance.

   const double minDistAdjustFactor(0.99);
   const double minDistRads(minPointSepRads_ * minDistAdjustFactor);

   for (vector<AvoidPosition>::const_iterator it = 
	   positionsToAvoid_.begin(); it != positionsToAvoid_.end(); ++it)
   {
      const AvoidPosition & avoidPos(*it);

      double diffRads = SseAstro::angSepRads(
         raRads, decRads, 
         avoidPos.pos_.raRads_, avoidPos.pos_.decRads_);

      if (diffRads < minDistRads)
      {
	 return false;
      }
   }

   return true;
}


void OffPositions::addPositionToAvoid(const string & positionType,
				      double raRads, double decRads)
{
   positionsToAvoid_.push_back(AvoidPosition(positionType, raRads, decRads));
}


/* 
  * Use the time since the 
  * previous observation, along with the earth's sidereal rotation rate,
  * to determine the RA that would put the beam back in the orientation
  * it had during the previous ON observation (i.e., move east, toward
  * increasing RA, from the original pointing position).
  * Note that this does not work near low/high decs, or if the delta
  * time is too short to allow the minimum pointing separation.
  * Returns true on success.
 */

bool OffPositions::computeOffBasedOnDeltaTime(
   double targetRaRads, double targetDecRads, int deltaTimeSecs,
   double *offRaRads, double *offDecRads)
{
   Assert(deltaTimeSecs > 0);

   /* OMEGA is sidereal rotation rate in rads/sec */
   double raDeltaRads(OMEGA * static_cast<double>(deltaTimeSecs));  

    // verify that the dec isn't too high or low
   if((raDeltaRads * cos(targetDecRads)) >= minPointSepRads_)
   {
      *offRaRads = targetRaRads + raDeltaRads;
      *offDecRads = targetDecRads;

      if (*offRaRads >= TWOPI) 
      {
	 *offRaRads -= TWOPI;
      }
      return true;
   }

   return false;
}

void OffPositions::moveNorth(
   double targetRaRads, double targetDecRads, 
   double arcDistRads,
   double *offRaRads, double *offDecRads)
{
   Assert(arcDistRads > 0.0);

   *offRaRads = targetRaRads;
   *offDecRads = targetDecRads + arcDistRads;

   // adjust for north pole crossing
   if(*offDecRads > NorthPoleDecRads)
   {
      *offDecRads = PI - *offDecRads;
      *offRaRads = fmod(*offRaRads + PI, TWOPI);
   }

}

void OffPositions::moveSouth(
   double targetRaRads, double targetDecRads, 
   double arcDistRads,
   double *offRaRads, double *offDecRads)
{
   Assert(arcDistRads > 0.0);

   *offRaRads = targetRaRads;
   *offDecRads = targetDecRads - arcDistRads;

   // adjust for south pole crossing
   if(*offDecRads < SouthPoleDecRads)
   {
      *offDecRads = -PI - *offDecRads;
      *offRaRads = fmod(*offRaRads + PI, TWOPI);
   }

}


void OffPositions::moveEast(double targetRaRads, double targetDecRads, 
			    double arcDistRads,
			    double *offRaRads, double *offDecRads)
{
   Assert(arcDistRads > 0.0);

   *offDecRads = asin(sin(targetDecRads) * cos(arcDistRads));
   *offRaRads = targetRaRads + asin(sin(arcDistRads) /
				    cos(*offDecRads));
   if (*offRaRads >= TWOPI)
   {
      *offRaRads -= TWOPI;
   }

}

void OffPositions::moveWest(double targetRaRads, double targetDecRads, 
			    double arcDistRads,
			    double *offRaRads, double *offDecRads)
{
   Assert(arcDistRads > 0.0);

   *offDecRads = asin(sin(targetDecRads) * cos(arcDistRads));
   *offRaRads = targetRaRads - asin(sin(arcDistRads) /
				    cos(*offDecRads));
   if (*offRaRads < 0.0)
   {
      *offRaRads += TWOPI;
   }
}


/*
 * Checks if the primary beam center is within one
  * primary beamwidth of either the north or south
  * j2000 equatorial pole.  Returns true if it is.
 */

bool OffPositions::isPrimaryBeamNearNorthOrSouthPole()
{
   const double poleCutoffDistRads(primaryBeamsizeRads_);
   const double poleRaRads(0);

   // North Pole
   double northPoleDistRads = SseAstro::angSepRads(
      primaryCenterRaRads_, primaryCenterDecRads_,
      poleRaRads, NorthPoleDecRads);

   if (northPoleDistRads < poleCutoffDistRads)
   {
      return true;
   }

   // South Pole
   double southPoleDistRads = SseAstro::angSepRads(
      primaryCenterRaRads_, primaryCenterDecRads_,
      poleRaRads, SouthPoleDecRads);

   if (southPoleDistRads <  poleCutoffDistRads)
   {
      return true;
   }

   return false;
}



/*
  Generate a square grid across the primary FOV.
  Grid starts in southwest corner, and goes up to
  the northeast corner.

  Grid spacing is the synthesized beam size, rather than the
  minPointSepRads_, to maximize the possibility that
  a particular pointing can be used.
  Note that some of the points will fall outside the primary
  FOV.

  When the primary beam center is near the north or south 
  j2000 equatorial pol, the grid is created using a rotated 
  coordinate system, and then rotated back to j2000 equatorial,
  in order to avoid having to deal with coordinate complications
  at the poles.

  For convenience, galactic coordinates are used for the rotation:
     j2000 north pol is gal long: 122.9 deg, lat 27.1 deg
     j2000 south pol is gal long: 302.9 deg, lat -27.1 deg

 */

void OffPositions::generatePrimaryFovGrid()
{
   bool rotateCoordSystem(false);

   double primaryRaRads(primaryCenterRaRads_);
   double primaryDecRads(primaryCenterDecRads_);

   if (isPrimaryBeamNearNorthOrSouthPole())
   {
      rotateCoordSystem = true; 

      // rotate primary beam center to "galactic" coords
      SseAstro::equ2000ToGal(
          primaryCenterRaRads_, primaryCenterDecRads_,
          &primaryRaRads, &primaryDecRads);
   }

   double halfPrimaryBeamsizeRads(primaryBeamsizeRads_ * 0.5);
   
   // start dec at south edge of primary fov
   double gridDecRads(primaryDecRads - 
		      halfPrimaryBeamsizeRads);

   int nDecSteps(static_cast<int>(
      ceil(primaryBeamsizeRads_ / synthBeamsizeRads_)));

   // add one more to account for the width of the 
   // last synthbeam on the east & north edges
   nDecSteps++;

   int nRaSteps(nDecSteps);

   for (int decStep=0; decStep < nDecSteps; ++decStep)
   {
      // start ra at west edge of primary FOV
      double gridRaRads;
      OffPositions::moveWest(primaryRaRads, gridDecRads,
			     halfPrimaryBeamsizeRads,
			     &gridRaRads, &gridDecRads);

      for (int raStep=0; raStep < nRaSteps; ++raStep)
      {
	 double pointRaRads(gridRaRads);
	 double pointDecRads(gridDecRads);

	 if (rotateCoordSystem)
	 {
	    // rotate back from "galactic" to true j2000 equatorial
             SseAstro::galToEqu2000(
                 gridRaRads, gridDecRads,
                 &pointRaRads, &pointDecRads);
	 }

	 primaryGridPositions_.push_back(Position(pointRaRads,
					   pointDecRads));


	 OffPositions::moveEast(gridRaRads, gridDecRads, 
				synthBeamsizeRads_,
				&gridRaRads, &gridDecRads);
      }

      OffPositions::moveNorth(gridRaRads, gridDecRads, 
			      synthBeamsizeRads_,
			      &gridRaRads, &gridDecRads);
   }

}


/* Search through the primary fov grid for 
   the first open position.

   Returns true if position is found.
*/

bool OffPositions::searchGridForOpenPosition(
   double *offRaRads, double *offDecRads)
{
   // Lazy grid creation
   if (primaryGridPositions_.empty())
   {
      generatePrimaryFovGrid();
      Assert(! primaryGridPositions_.empty());
   }

   for (vector<Position>::const_iterator it = 
	   primaryGridPositions_.begin();
	it != primaryGridPositions_.end(); ++it)
   {
      const Position & gridPos(*it);

      if (isOffPositionInPrimaryFov(gridPos.raRads_, 
				 gridPos.decRads_))
      {
	 if (isPositionAvailable(gridPos.raRads_, 
				 gridPos.decRads_))
	 {
	    *offRaRads = gridPos.raRads_;
	    *offDecRads = gridPos.decRads_;

	    return true;
	 }
      }
   }

   return false;
}



// Look for a valid off position by trying each move direction in turn.
// The first position that passes all the restrictions will be returned.
// Throws SseException if no valid position is found.

void OffPositions::findOffPosition(
   double targetRaRads, double targetDecRads, int deltaTimeSecs,
   double *offRaRads, double *offDecRads)
{
#if 0
   cout << "OffPositions::getOffPosition: "
	<< " targetRaRads = " << targetRaRads
	<< " targetDecRads = " << targetDecRads << endl;
#endif

   const double moveDistRads(minPointSepRads_);

   double newRaRads(-PI);
   double newDecRads(-PI);

#if 0
// Normal case:
   enum MoveOptions { MOVE_FIRST_CHOICE, 
		      MOVE_BY_TIME=MOVE_FIRST_CHOICE,
		      MOVE_SOUTH, MOVE_NORTH, 
		      MOVE_WEST, MOVE_EAST,
		      MOVE_SEARCH,
		      MOVE_LAST_CHOICE = MOVE_SEARCH };
#else

// reordered

   enum MoveOptions { MOVE_FIRST_CHOICE, 
		      MOVE_BY_TIME=MOVE_FIRST_CHOICE,
                      MOVE_EAST, MOVE_NORTH, 
		      MOVE_SOUTH, MOVE_WEST, 
		      MOVE_SEARCH,
		      MOVE_LAST_CHOICE = MOVE_SEARCH };
#endif

   for (MoveOptions option = MOVE_FIRST_CHOICE; option <= MOVE_LAST_CHOICE;
	option = static_cast<MoveOptions>(option+1))
   {
      bool succeeded(true);
      string positionType("");
      
      switch (option)
      {
      case MOVE_BY_TIME:

	 // First try to pick an off based on the delta time
	 positionType = "time";
	 succeeded = computeOffBasedOnDeltaTime(
	    targetRaRads, targetDecRads,
	    deltaTimeSecs, &newRaRads, &newDecRads);

	 break;

      case MOVE_SOUTH:
	 positionType = "south";
	 moveSouth(targetRaRads, targetDecRads, moveDistRads,
		   &newRaRads, &newDecRads);

	 break;

      case MOVE_NORTH:
	 positionType = "north";
	 moveNorth(targetRaRads, targetDecRads, moveDistRads,
		   &newRaRads, &newDecRads);

	 break;

      case MOVE_WEST:
	 positionType = "west";
	 moveWest(targetRaRads, targetDecRads, moveDistRads,
		  &newRaRads, &newDecRads);

	 break;

      case MOVE_EAST:
	 positionType = "east";
	 moveEast(targetRaRads, targetDecRads, moveDistRads,
		  &newRaRads, &newDecRads);

	 break;

      case MOVE_SEARCH:
	 positionType = "grid";
	 succeeded = searchGridForOpenPosition(&newRaRads, &newDecRads);

	 break;

      default:
	 AssertMsg(0, "invalid move option");

      }

      if (succeeded)
      {
	 if (isOffPositionInPrimaryFov(newRaRads, newDecRads))
	 {
	    if (isPositionAvailable(newRaRads, newDecRads))
	    {
	       // Good position found.

	       // Add the new OFF to the
	       // list of positions to be avoided next time
	       addPositionToAvoid(positionType, newRaRads, newDecRads);

	       *offRaRads = newRaRads;
	       *offDecRads = newDecRads;

	       return;
	    }
	 } 
      }
   }

   // Failed to find valid off position
   stringstream strm;
   strm.precision(PrintPrecision);  // show N places after the decimal
   strm.setf(std::ios::fixed);  // show all decimal places up to precision

   strm << "OffPositions::getOffPosition:"
	<< " failed to find valid OFF position for" 
	<< " target: raRads: " << targetRaRads 
	<< " decRads: " << targetDecRads << endl
	<< *this << endl;

   throw SseException(strm.str(), __FILE__, __LINE__,
		      SSE_MSG_INVALID_TARGET, SEVERITY_ERROR);

}

// restore class to its original "construction time" condition
void OffPositions::reset()
{
   positionsToAvoid_.clear();
}

/*
  add all the targets to the avoid list
  Throw an exception if any are outside the primary FOV.
*/

void OffPositions::addTargetsToAvoidList(const vector<Position> & targets)
{
   for (vector<Position>::const_iterator it = targets.begin();
	it != targets.end(); ++it)
   {
      const Position & target(*it);
      const string positionType("target");

      if (!isTargetInPrimaryFov(target.raRads_, target.decRads_))
      {
	 stringstream strm;

	 strm.precision(PrintPrecision); // show N places after the decimal
	 strm.setf(std::ios::fixed);  // show all decimal places up to precision

	 strm << "OffPositions::getOffPositions:"
	      << " target is outside the primary FOV:" 
	      << " target: raRads: " << target.raRads_
	      << " decRads: " << target.decRads_ << endl
	      << *this << endl;

	 throw SseException(strm.str(), __FILE__, __LINE__,
			   SSE_MSG_INVALID_TARGET, SEVERITY_ERROR);
	 
      }

      addPositionToAvoid(positionType, 
			 target.raRads_, target.decRads_);
   }

}

/* Find off positions for the given targets.
    deltaTimeSecs is the time since the last observation.
    Chosen positions are stored in the offPositions container.
 */
void OffPositions::getOffPositions(const vector<Position> & targets,
				   int deltaTimeSecs, 
				   vector<Position> & offPositions)
{
   // reset any object state information, in case this method
   // is called multiple times
   reset();

   addTargetsToAvoidList(targets);

   // find off position for each target
   for (vector<Position>::const_iterator it = targets.begin();
	it != targets.end(); ++it)
   {
      const Position & target(*it);
      Position off;

      findOffPosition(target.raRads_, target.decRads_,
		     deltaTimeSecs,
		     &off.raRads_, &off.decRads_);

      offPositions.push_back(off);
   }

}



ostream & operator<<(ostream &strm,
		     const OffPositions & offPos)
{
   stringstream info;

   info.precision(PrintPrecision); // show N places after the decimal
   info.setf(std::ios::fixed);  // show all decimal places up to precision

   info << "OffPositions:: " << endl
	<< "primaryCenter raRads: " << offPos.primaryCenterRaRads_ 
	<< " decRads: " << offPos.primaryCenterDecRads_ << endl
	<< "primaryBeamsizeRads: " << offPos.primaryBeamsizeRads_ << endl
	<< "synthBeamsizeRads: " << offPos.synthBeamsizeRads_ << endl
	<< "minPointSepSynthBeamsizes: " << offPos.minPointSepSynthBeamsizes_ << endl
	<< "minPointSepRads: " << offPos.minPointSepRads_ << endl
	<< "primaryGridPointCount: " << offPos.primaryGridPositions_.size() << endl
	<< "";

   info << "PositionsToAvoid (targets and off positions): " << endl;

   for (vector<OffPositions::AvoidPosition>::const_iterator it = 
	   offPos.positionsToAvoid_.begin(); 
	it != offPos.positionsToAvoid_.end(); ++it)
   {
      const OffPositions::AvoidPosition & avoidPos(*it);

      info << "(" << avoidPos.type_ << ")"
	   << " raRads: " << avoidPos.pos_.raRads_
	   << " decRads: " << avoidPos.pos_.decRads_;


#if 1
      // debug
      // change ra into +- range for easier plotting around zero ra
      double adjustedRaRads(avoidPos.pos_.raRads_);
      if (avoidPos.pos_.raRads_ > M_PI)
      {
	 adjustedRaRads = avoidPos.pos_.raRads_ - (TWOPI);
      }
      info << " adjustedRaRads: " << adjustedRaRads;


      // (ra,dec) format for cutting & pasting into test cases 
      info << " csvRaDecRads: (" << avoidPos.pos_.raRads_ << ", " 
	   << avoidPos.pos_.decRads_ << ") ";

#endif      
 
      info << endl;
   }

   strm << info.str();

   return strm;
}

void OffPositions::printPrimaryFovGrid(ostream &strm) const
{
   strm << "OffPositions::primaryFovGrid: " << endl;

   int count(0);

   for (vector<Position>::const_iterator it = 
	   primaryGridPositions_.begin();
	it != primaryGridPositions_.end(); ++it)
   {
      const Position & gridPos(*it);

      strm << count++ << ") "
	   << "gridPointRaRads: " << gridPos.raRads_
	   << " gridPointDecRads: " << gridPos.decRads_;
      
      // make ra into +- range for easier plotting around zero ra
      double adjustedRaRads(gridPos.raRads_);
      if (gridPos.raRads_ > M_PI)
      {
	 adjustedRaRads = gridPos.raRads_ - (M_PI * 2);
      }

      strm << " adjustedRaRads: " << adjustedRaRads;
      strm << endl;
      
   }
}