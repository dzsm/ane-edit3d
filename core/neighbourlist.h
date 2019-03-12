#ifndef NEIGHBOURLIST
#define NEIGHBOURLIST

#include <iostream>
//#include <ostream>
//#include <sstream>
#include <vector>
//#include <algorithm>
//#include <iomanip>
//#include <string>
//#include <algorithm>
//#include <iomanip>
//#include <list>
//#include <set>
#include <map>
#include <unordered_map>

//#include <cstdarg>
//#include <tuple>
//#include <stdexcept>

#include <Eigen/Eigen>
#include <Eigen/StdVector>

// O(n) neighbor list method using voxels
inline void generateNeighbourList(
    std::vector<std::vector<size_t> >& neighbourList,
    const std::vector<Eigen::Vector3d>& atomLocations,
    const std::vector<bool>& exclusions,
    double maxDistance,
    double minDistance = 0.0)
{
    assert( atomLocations.size() > 0 );
    assert( atomLocations.size() == exclusions.size() );
    assert( maxDistance > minDistance && minDistance >= 0.0 );

    neighbourList.clear();
    neighbourList.resize(atomLocations.size());

    size_t nAtoms = atomLocations.size();

    double maxDistanceSquared = maxDistance * maxDistance;
    double minDistanceSquared = minDistance * minDistance;

    Eigen::Vector3i voxelSize;
    voxelSize[0] = maxDistance;
    voxelSize[1] = maxDistance;
    voxelSize[2] = maxDistance;

    Eigen::Vector3i dIndex;
    dIndex[0] = int(maxDistance / voxelSize[0]) + 1; // How may voxels away do we have to look?
    dIndex[1] = int(maxDistance / voxelSize[1]) + 1;
    dIndex[2] = int(maxDistance / voxelSize[2]) + 1;

    std::map<std::vector<int>, std::vector<size_t> > voxelMap;

    for (size_t referenceAtom = 0; referenceAtom < nAtoms; ++referenceAtom) {
        // 1) Find other atoms that are close to this one
        const size_t               atomI = referenceAtom;
        const Eigen::Vector3d& locationI = atomLocations[referenceAtom];

        if( exclusions[atomI] ) continue;

        Eigen::Vector3i voxelIndexAtomI;

        voxelIndexAtomI[0] = int(floor(locationI[0] / voxelSize[0]));
        voxelIndexAtomI[1] = int(floor(locationI[1] / voxelSize[1]));
        voxelIndexAtomI[2] = int(floor(locationI[2] / voxelSize[2]));

        Eigen::Vector3i first = voxelIndexAtomI-dIndex;
        Eigen::Vector3i last  = voxelIndexAtomI+dIndex;

        for (int x = first[0]; x <= last[0]; ++x)
            for (int y = first[1]; y <= last[1]; ++y)
                for (int z = first[2]; z <= last[2]; ++z) {
                    Eigen::Vector3i voxelIndexAtomJ(x, y, z);
                    std::vector<int> voxelIndexAtomJJ(3);
                    for (int index=0; index<3; index++)
                        voxelIndexAtomJJ[index] = voxelIndexAtomJ[index];

                    if (voxelMap.find(voxelIndexAtomJJ) == voxelMap.end()) continue; // no such voxel; skip

                    const std::vector<size_t>& voxelAtomJ = voxelMap.find(voxelIndexAtomJJ)->second;

                    for (size_t i = 0; i<voxelAtomJ.size(); i++) {
                        const size_t atomJ = voxelAtomJ[i];

                        if ( atomI == atomJ )    continue;
                        if ( exclusions[atomJ] ) continue;

                        const Eigen::Vector3d& locationJ = atomLocations[atomJ];

                        double dSquared = (locationI - locationJ).squaredNorm();
                        if (dSquared > maxDistanceSquared) continue;
                        if (dSquared < minDistanceSquared) continue;

                        //pairList.push_back( std::pair<size_t,size_t>(atomI, atomJ) );
                        neighbourList[atomI].push_back(atomJ);
                        neighbourList[atomJ].push_back(atomI);
                    }
                }
        // 2) Add this atom to the voxelHash
        std::vector<int> voxelIndexAtomII(3);
        for (int index=0; index<3; index++)
            voxelIndexAtomII[index] = voxelIndexAtomI[index];

        if (voxelMap.find(voxelIndexAtomII) == voxelMap.end())
            voxelMap[voxelIndexAtomII] = std::vector<size_t>() ;

        voxelMap.find(voxelIndexAtomII)->second.push_back(atomI);

    }
}

#endif // NEIGHBOURLIST

