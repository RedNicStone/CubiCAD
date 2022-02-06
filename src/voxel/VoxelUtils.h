//
// Created by nic on 13/10/2021.
//

#ifndef CUBICAD_VOXELUTILS_H
#define CUBICAD_VOXELUTILS_H

#include <memory>
#include <boost/multiprecision/cpp_int.hpp>

#include "LibMorton.h"


#ifdef USE_UINT32_INDICES
typedef uint_fast32_t size_v;
typedef uint_fast16_t size_n;
#else
typedef uint_fast64_t size_v;
typedef uint_fast32_t size_n;

namespace libmorton {
  inline size_v morton2D_encode(const size_n x, const size_n y) {
      return morton2D_64_encode(x, y);
  }

  inline size_v morton3D_encode(const size_n x, const size_n y, const size_n z) {
      return morton3D_64_encode(x, y, z);
  }

  inline void morton2D_decode(const size_v morton, size_n &x, size_n &y) {
      return morton2D_64_decode(morton, x, y);
  }

  inline void morton3D_decode(const size_v morton, size_n &x, size_n &y, size_n &z) {
      return morton3D_64_decode(morton, x, y, z);
  }
}
#endif

#endif //CUBICADVOXELTYPES_VOXELUTILS_H
