
# AA+ library from http://www.naughter.com/aa.html

PUBLIC_HEADER_FILES = \
	AA2DCoordinate.h \
	AA3DCoordinate.h \
	AAAberration.h \
	AAAngularSeparation.h \
	AABinaryStar.h \
	AACoordinateTransformation.h \
	AADate.h \
	AADiameters.h \
	AADynamicalTime.h \
	AAEarth.h \
	AAEaster.h \
	AAEclipses.h \
	AAEclipticalElements.h \
	AAElementsPlanetaryOrbit.h \
	AAElliptical.h \
	AAEquationOfTime.h \
	AAEquinoxesAndSolstices.h \
	AAFK5.h \
	AAGalileanMoons.h \
	AAGlobe.h \
	AA+.h \
	AAIlluminatedFraction.h \
	AAInterpolate.h \
	AAJewishCalendar.h \
	AAJupiter.h \
	AAKepler.h \
	AAMars.h \
	AAMercury.h \
	AAMoon.h \
	AAMoonIlluminatedFraction.h \
	AAMoonMaxDeclinations.h \
	AAMoonNodes.h \
	AAMoonPerigeeApogee.h \
	AAMoonPhases.h \
	AAMoslemCalendar.h \
	AANearParabolic.h \
	AANeptune.h \
	AANodes.h \
	AANutation.h \
	AAParabolic.h \
	AAParallactic.h \
	AAParallax.h \
	AAPhysicalJupiter.h \
	AAPhysicalMars.h \
	AAPhysicalMoon.h \
	AAPhysicalSun.h \
	AAPlanetaryPhenomena.h \
	AAPlanetPerihelionAphelion.h \
	AAPluto.h \
	AAPrecession.h \
	AARefraction.h \
	AARiseTransitSet.h \
	AASaturn.h \
	AASaturnMoons.h \
	AASaturnRings.h \
	AASidereal.h \
	AAStellarMagnitudes.h \
	AASun.h \
	AAUranus.h \
	AAVenus.h \
	stdafx.h

lib_LIBRARIES = libaaplus.a

libaaplus_a_SOURCES = \
	$(PUBLIC_HEADER_FILES) \
	AAAberration.cpp \
	AAAngularSeparation.cpp \
	AABinaryStar.cpp \
	AACoordinateTransformation.cpp \
	AADate.cpp \
	AADiameters.cpp \
	AADynamicalTime.cpp \
	AAEarth.cpp \
	AAEaster.cpp \
	AAEclipses.cpp \
	AAEclipticalElements.cpp \
	AAElementsPlanetaryOrbit.cpp \
	AAElliptical.cpp \
	AAEquationOfTime.cpp \
	AAEquinoxesAndSolstices.cpp \
	AAFK5.cpp \
	AAGalileanMoons.cpp \
	AAGlobe.cpp \
	AAIlluminatedFraction.cpp \
	AAInterpolate.cpp \
	AAJewishCalendar.cpp \
	AAJupiter.cpp \
	AAKepler.cpp \
	AAMars.cpp \
	AAMercury.cpp \
	AAMoon.cpp \
	AAMoonIlluminatedFraction.cpp \
	AAMoonMaxDeclinations.cpp \
	AAMoonNodes.cpp \
	AAMoonPerigeeApogee.cpp \
	AAMoonPhases.cpp \
	AAMoslemCalendar.cpp \
	AANearParabolic.cpp \
	AANeptune.cpp \
	AANodes.cpp \
	AANutation.cpp \
	AAParabolic.cpp \
	AAParallactic.cpp \
	AAParallax.cpp \
	AAPhysicalJupiter.cpp \
	AAPhysicalMars.cpp \
	AAPhysicalMoon.cpp \
	AAPhysicalSun.cpp \
	AAPlanetaryPhenomena.cpp \
	AAPlanetPerihelionAphelion.cpp \
	AAPluto.cpp \
	AAPrecession.cpp \
	AARefraction.cpp \
	AARiseTransitSet.cpp \
	AASaturn.cpp \
	AASaturnMoons.cpp \
	AASaturnRings.cpp \
	AASidereal.cpp \
	AAStellarMagnitudes.cpp \
	AASun.cpp \
	AAUranus.cpp \
	AAVenus.cpp

# public headers to include in 'make install' target
include_HEADERS = $(PUBLIC_HEADER_FILES)

check_PROGRAMS = AATest

TESTS = AATest

AATest_SOURCES = \
	AATest.cpp \
	$(libaaplus_a_SOURCES)

AATest_LDADD = -lm
