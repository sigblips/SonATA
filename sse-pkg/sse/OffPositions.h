/*******************************************************************************

 File:    OffPositions.h
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


#ifndef OffPositions_H
#define OffPositions_H

/*
  Select OFF positions for multiple synthesized beams within a primary beam
  FOV, in a preferred order:  based on time since last obs; south; north;
  west; east; then any open position.
  Each OFF must be at least the minPointSepSynthBeamsizes from its
  target's ON position, any other OFFs, or any other ON positions.
 */

#include <vector>
#include <iosfwd>
#include <string>

using std::vector;
using std::string;
using std::ostream;

class OffPositions
{
 public:

    OffPositions(
       double synthBeamsizeRads,
       double minPointSepSynthBeamsizes,  // minimum pointing separation
       double primaryCenterRaRads,
       double primaryCenterDecRads,
       double primaryBeamsizeRads);
    
    virtual ~OffPositions();

    struct Position
    {
       double raRads_;
       double decRads_;

       Position();
       Position(double raRads, double decRads);
    };

    void getOffPositions(const vector<Position> & targets,
       int deltaTimeSecs, vector<Position> & offPositions);

    double getMinPointSepRads() const;
    double getPrimaryBeamsizeRads() const;

    static void moveNorth(double targetRaRads, double targetDecRads, 
			  double arcDistRads,
			  double *offRaRads, double *offDecRads);

    static void moveSouth(double targetRaRads, double targetDecRads, 
			  double arcDistRads,
			  double *offRaRads, double *offDecRads);

    static void moveEast(double targetRaRads, double targetDecRads, 
			 double arcDistRads,
			 double *offRaRads, double *offDecRads);

    static void moveWest(double targetRaRads, double targetDecRads, 
			 double arcDistRads,
			 double *offRaRads, double *offDecRads);
    

    friend ostream & operator<<(ostream &strm,
				const OffPositions & offPos);

    void printPrimaryFovGrid(std::ostream &strm) const;

 protected:

    void addTargetsToAvoidList(const vector<Position> & targets);

    void findOffPosition(
       double targetRaRads, double targetDecRads, int deltaTimeSecs,
       double *offRaRads, double *offDecRads);

    bool computeOffBasedOnDeltaTime(
       double targetRaRads, double targetDecRads, int deltaTimeSecs,
       double *offRaRads, double *offDecRads);

    bool searchGridForOpenPosition(
       double *offRaRads, double *offDecRads);

    bool isTargetInPrimaryFov(double raRads, double decRads);

    bool isOffPositionInPrimaryFov(double raRads, double decRads);

    bool isPositionInPrimaryFov(double raRads, double decRads,
				double distToPrimaryEdgeRads);

    bool isPositionAvailable(double raRads, double decRads);

    void addPositionToAvoid(const string &type,
			    double raRads, double decRads);

    bool isPrimaryBeamNearNorthOrSouthPole();

    void generatePrimaryFovGrid();

    void reset();

 private:

    struct AvoidPosition 
    {
       string type_;
       Position pos_;

       AvoidPosition(const string & type,
		     double raRads, double decRads);
    };

    double synthBeamsizeRads_;
    double minPointSepSynthBeamsizes_;
    double minPointSepRads_;

    double primaryCenterRaRads_;
    double primaryCenterDecRads_;
    double primaryBeamsizeRads_;

    vector<AvoidPosition> positionsToAvoid_;
    vector<Position> primaryGridPositions_;

    // Disable copy construction & assignment.
    // Don't define these.
    OffPositions(const OffPositions& rhs);
    OffPositions& operator=(const OffPositions& rhs);

};

#endif // OffPositions_H