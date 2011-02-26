################################################################################
#
# File:    preludeRfiMask.tcl
# Project: OpenSonATA
# Authors: The OpenSonATA code is the result of many programmers
#          over many years
#
# Copyright 2011 The SETI Institute
#
# OpenSonATA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenSonATA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
# 
# Implementers of this code are requested to include the caption
# "Licensed through SETI" with a link to setiQuest.org.
# 
# For alternate licensing arrangements, please contact
# The SETI Institute at www.seti.org or setiquest.org. 
#
################################################################################

# NSS Permanent RFI Mask for Hat Creek
# Values generated on Dec 2006 using Ants 3dY, 3eY.
# Preliminary cutoff selection by Peter Backus.
# Note: all values are in MHz

# band center freq & width
set bandcovered  { 6000.00 12000.0 }

# mask elements
# center freq & width

set masks  {
540.0 80.0
636.0 108.0
704.0 24.0
720.0 4.0
731.0 2.0
735.0 2.0
758.0 12.0
767.0 2.0
771.0 2.0
779.0 10.0
791.0 6.0
800.0 8.0
841.0 2.0
872.0 44.0
920.0 4.0
930.0 4.0
941.0 6.0
960.0 4.0
967.0 2.0
971.0 2.0
999.0 6.0
1080.0 4.0
1089.0 2.0
1151.0 2.0
1162.0 4.0
1176.0 20.0
1207.0 14.0
1227.0 6.0
1248.0 8.0
1270.0 4.0
1279.0 2.0
1320.0 4.0
1340.0 4.0
1350.0 8.0
1359.0 2.0
1367.0 2.0
1379.0 18.0
1397.0 2.0
1565.0 90.0
1622.5 9.0
1638.0 2.0
1647.0 6.0
1676.0 4.0
1681.0 2.0
1686.0 4.0
1691.0 2.0
1751.0 2.0
1755.0 2.0
1950.0 12.0
1963.0 6.0
1971.0 2.0
2030.0 4.0
2108.0 20.0
2333.0 50.0
2362.0 4.0
3519.0 2.0
3526.0 4.0
3590.0 12.0
3619.0 2.0
3644.0 4.0
3663.0 2.0
3788.0 168.0
3890.0 24.0
3924.0 40.0
3948.0 4.0
4078.0 244.0
4241.0 2.0
4245.0 2.0
4251.0 2.0
4285.0 2.0
4291.0 2.0
4323.0 6.0
4329.0 2.0
7528.0 4.0
}