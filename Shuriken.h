#ifndef __SHURIKEN_H__
#define __SHURIKEN_H__

#define ANTIFERRO 2
#define GENERIC 1
#define XXZ 3

namespace Shuriken {

  __inline__ constexpr int total_number_of_parallel_updates() {
    return 3;
  }

  __inline__ constexpr int number_of_sublattices_per_update() {
    return 2;
  }

  /// number of parallel for covering the full lattice.
  __inline__ constexpr int number_of_parallel_updates() {
    return 3;
  }

  __inline__ constexpr int number_of_spins_per_unit_cell() {
    return 6;
  }

  __inline__ constexpr int number_of_spins_per_primitive_cell() {
    return 6;
  }

  __inline__ int number_of_neighbors(const int *num_neighbors_, const int *offset_, const int site) {
    // return num_neighbors_[offset_[site]]
    return 4;
  }

  __inline__ constexpr int number_of_neighbors() {
    // return num_neighbors_[offset_[site]]
    return 4;
  }

  __inline__ constexpr int number_of_links() {
    // return num_neighbors_[offset_[site]]
    return 4 * 6;
  }

  // __inline__ void init_rng_table(Device &dev) {
  //   dev.InitializeRngTable(number_of_parallel_updates() * dev.NumberOfTemperatures() * number_of_sublattices_per_update());
  // }

  // __inline__ void generate_random_lattice_sequences(Device &dev) {
  //   dev.GenerateRandomNumbers<unsigned int>(number_of_parallel_updates() * dev.NumberOfTemperatures() * number_of_sublattices_per_update(), dev.Rng<unsigned int>());
  // }

  /*
   * conventions for the sub lattice indexes
   ^              x
   ^
   ^           4    3
   ^       x            2
   ^           5    1
   ^
   *             0
   ^
   */


  const int neighbors[6][4][3] = {{{0, -1, 4}, {0, 0, 1}, {0, -1, 3}, {0, 0, 5}},
                                  {{0, 0, 0}, {0, 0, 2}, {0, 0, 3}, {0, 0, 5}},
                                  {{0, 0, 1}, {0, 0, 3}, {1, 0, 4}, {1, 0, 5}},
                                  {{0, 0, 1}, {0, 0, 2}, {0, 0, 4}, {0, 1, 0}},
                                  {{0, 0, 3}, {0, 0, 5}, {-1, 0, 2}, {0, 1, 0}},
                                  {{0, 0, 0}, {0, 0, 1}, {0, 0, 4},{-1, 0, 2}}};
  const  int IndependentSubLattices[3][2] = {{0, 2}, {1, 4}, {5, 3}};
};
// extern void metropolis(Device& dev);
// extern void heat_bath(Device& dev);
// extern void over_relaxation(Device& dev);
// extern void over_relaxation_by_pi(Device& dev);
extern int Model;
extern double Jpp;
extern double Jzz;
extern double X_Parameter;
extern double Jx;
extern double Jy;
extern double Jz;
extern double D;

#endif
