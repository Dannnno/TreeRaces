// -*- C++ -*-
#ifndef VERSIONS_DAN_H
#define VERSIONS_DAN_H

#include "CommonDefinitions.h" 

#include "tr/boundingbox.h"
#include "tr/point3d.h"
#include "tr/octree.h"
#include "tr/pointerless_octree.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

struct PointExtractor {
  tr::Point3d operator()(const Point& p) const {
    return tr::Point3d{p[0], p[1], p[2]};
  }
};

void
findNeighbors_danPointerlessOctree(const vector<Point> & points,
    const BoundingBox & pointsBoundingBox,
    const double neighborSearchDistance,
    vector<unsigned int> * const result,
    vector<unsigned int> * const resultDelimiters,
    double * const initializationTime,
    double * const queryingTime) {
  ignoreUnusedVariable(pointsBoundingBox);

  const double searchDistanceSquared =
    neighborSearchDistance * neighborSearchDistance;
  const double paddedNeighborSearchDistance = neighborSearchDistance * 1.001;

  const high_resolution_clock::time_point initializationTic =
    high_resolution_clock::now();

  tr::PointerlessOctree<typename vector<Point>::const_iterator, PointExtractor> 
    octree(points.begin(), points.end());

  // track how long it took to initialize octree
  const high_resolution_clock::time_point initializationToc =
    high_resolution_clock::now();

  resultDelimiters->push_back(0);

  const unsigned int numberOfPoints = points.size();
  const PointExtractor p;
  for (unsigned int pointIndex = 0; pointIndex < numberOfPoints; ++pointIndex) {
    vector<typename vector<Point>::const_iterator> thisPointsNeighborhood;
    const tr::Point3d centerPoint = p(points[pointIndex]);
    const tr::Point3d maxPoint =
        {points[pointIndex][0] + paddedNeighborSearchDistance,
         points[pointIndex][1] + paddedNeighborSearchDistance,
         points[pointIndex][2] + paddedNeighborSearchDistance
        };
                            
    const tr::Point3d minPoint =
        {points[pointIndex][0] - paddedNeighborSearchDistance,
         points[pointIndex][1] - paddedNeighborSearchDistance,
         points[pointIndex][2] - paddedNeighborSearchDistance
        };
    
    tr::BoundingBox box = {minPoint, maxPoint};

    auto out = std::back_inserter(thisPointsNeighborhood);
    octree.search(box, out);

    for (const auto it : thisPointsNeighborhood) {
      const tr::Point3d point = p(*it);
      const double squaredDistance =
        std::pow(point.x - centerPoint.x, 2) 
        + std::pow(point.y - centerPoint.y, 2)
        + std::pow(point.z - centerPoint.z, 2);
      if (squaredDistance <= searchDistanceSquared) { 
        result->push_back(std::distance(points.begin(), it));
      }
    }

    resultDelimiters->push_back(result->size());
  }

  const high_resolution_clock::time_point queryingToc =
    high_resolution_clock::now();

  *initializationTime =
    duration_cast<duration<double> >(initializationToc - initializationTic).count();
  *queryingTime =
    duration_cast<duration<double> >(queryingToc - initializationToc).count();

}

void
findNeighbors_danOctree(const vector<Point> & points,
    const BoundingBox & pointsBoundingBox,
    const double neighborSearchDistance,
    vector<unsigned int> * const result,
    vector<unsigned int> * const resultDelimiters,
    double * const initializationTime,
    double * const queryingTime) {
  ignoreUnusedVariable(pointsBoundingBox);

  const double searchDistanceSquared =
    neighborSearchDistance * neighborSearchDistance;
  const double paddedNeighborSearchDistance = neighborSearchDistance * 1.001;

  const high_resolution_clock::time_point initializationTic =
    high_resolution_clock::now();

  tr::Octree<typename vector<Point>::const_iterator, PointExtractor> 
    octree(points.begin(), points.end());

  // track how long it took to initialize octree
  const high_resolution_clock::time_point initializationToc =
    high_resolution_clock::now();

  resultDelimiters->push_back(0);

  const unsigned int numberOfPoints = points.size();
  const PointExtractor p;
  for (unsigned int pointIndex = 0; pointIndex < numberOfPoints; ++pointIndex) {
    vector<typename vector<Point>::const_iterator> thisPointsNeighborhood;
    const tr::Point3d centerPoint = p(points[pointIndex]);
    const tr::Point3d maxPoint =
        {points[pointIndex][0] + paddedNeighborSearchDistance,
         points[pointIndex][1] + paddedNeighborSearchDistance,
         points[pointIndex][2] + paddedNeighborSearchDistance
        };
                            
    const tr::Point3d minPoint =
        {points[pointIndex][0] - paddedNeighborSearchDistance,
         points[pointIndex][1] - paddedNeighborSearchDistance,
         points[pointIndex][2] - paddedNeighborSearchDistance
        };
    
    tr::BoundingBox box = {minPoint, maxPoint};

    auto out = std::back_inserter(thisPointsNeighborhood);
    octree.search(box, out);

    for (const auto it : thisPointsNeighborhood) {
      const tr::Point3d point = p(*it);
      const double squaredDistance =
        std::pow(point.x - centerPoint.x, 2) 
        + std::pow(point.y - centerPoint.y, 2)
        + std::pow(point.z - centerPoint.z, 2);
      if (squaredDistance <= searchDistanceSquared) { 
        result->push_back(std::distance(points.begin(), it));
      }
    }

    resultDelimiters->push_back(result->size());
  }

  const high_resolution_clock::time_point queryingToc =
    high_resolution_clock::now();

  *initializationTime =
    duration_cast<duration<double> >(initializationToc - initializationTic).count();
  *queryingTime =
    duration_cast<duration<double> >(queryingToc - initializationToc).count();

}

#endif // VERSIONS_DAN_H
