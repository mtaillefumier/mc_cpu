#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <mpi.h>
#include <mkl.h>
#include <mkl_blas.h>
#include <mkl_cblas.h>
#include <mkl_lapack.h>
#include <mkl_service.h>
#include <mkl_types.h>
#include <mkl_vml_functions.h>
#include <umpire/Umpire.hpp>
#include <umpire/ResourceManager.hpp>
#include <umpire/Allocator.hpp>
#include <umpire/util/wrap_allocator.hpp>
#include <umpire/strategy/DynamicPoolList.hpp>
#include <umpire/strategy/AlignedAllocator.hpp>
#include <gsl/gsl_sort.h>
#include "mdarray.hpp"

class context {
  /// handler to umpire allocator_
  // umpire::Allocator allocator_;
  /// handler to umpire memory pool
  umpire::Allocator memory_pool_allocator_;
  std::vector<size_t> temperatures_index_;
  int rank_{0};
  int size_{1};
  std::vector<double> temperatures_;
  std::vector<int> number_of_temperatures_per_rank_;
  struct {int x,y,z,w;} system_size_;
  VSLStreamStatePtr RngStream_;
  std::vector<int> temperatures_offset_;
  int number_of_spins_;
  utils::mdarray<int, 2> neighbors_;
public:
  context() {
  };
  
  context(umpire::Allocator &memory_pool_allocator__, const int *const n_, const int number_of_temperatures__, const int Rng_) :
    memory_pool_allocator_(memory_pool_allocator__)
  {
    
    system_size_.x = n_[0];
    system_size_.y = n_[1];
    system_size_.z = n_[2];
    system_size_.w = n_[3];
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
    MPI_Comm_size(MPI_COMM_WORLD, &size_);

    // auto& rm_  = umpire::ResourceManager::getInstance();

    // allocator_ = rm_.getAllocator("HOST");
    // memory_pool_allocator_ =
    //   rm_.makeAllocator<umpire::strategy::AlignedAllocator>("aligned_allocator", allocator_, 256);

    number_of_spins_ = system_size_.x * system_size_.y * lattice_model::number_of_spins_per_unit_cell();

    temperatures_.resize(number_of_temperatures__);
    temperatures_index_.resize(number_of_temperatures__);

    neighbors_ = utils::mdarray<int, 2>(number_of_spins_, lattice_model::number_of_neighbors());
    vslNewStream(&this->RngStream_, VSL_BRNG_MT2203 + rank_, Rng_);

    number_of_temperatures_per_rank_.resize(size_);

    for (auto &t : number_of_temperatures_per_rank_)
      t = number_of_temperatures__ / size_;

    if ((rank_ == (size_ - 1)) && (number_of_temperatures__ % size_)) {
      number_of_temperatures_per_rank_[size_ - 1] = temperatures_.size() % size_;
    }

    temperatures_offset_.resize(size_);
    temperatures_offset_[0] = 0;

    for (int i = 1; i < size_; i++)
      temperatures_offset_[i] = number_of_temperatures_per_rank_[i - 1] + temperatures_offset_[i - 1];

    for (int sl = 0; sl < lattice_model::number_of_spins_per_unit_cell(); sl++) {
      for (int y = 0; y < system_size_.y; y++) {
        for (int x = 0; x < system_size_.x; x++) {
          const int spin_index_ = sl * system_size_.x * system_size_.y + (y * system_size_.x + x);
          for (int nei = 0; nei < lattice_model::number_of_neighbors(); nei++) {
            const int y1 = (y + lattice_model::neighbors[sl][nei][1] + system_size_.y) % system_size_.y;
            const int x1 = (x + lattice_model::neighbors[sl][nei][0] + system_size_.x) % system_size_.x;
            const int nei_spin_index = lattice_model::neighbors[sl][nei][2] * system_size_.x * system_size_.y + (y1 * system_size_.x + x1);
            neighbors_(spin_index_, nei) = nei_spin_index; 
            if (nei_spin_index >= system_size_.x * system_size_.y * lattice_model::number_of_spins_per_unit_cell()) {
              std::cout << "Spin " << x << y << " " << x1 << " " << y1 << std::endl;
              exit(1);
            }
          }
        }
      }
    }
  }
  
  ~context() {
    temperatures_.clear();
    neighbors_.clear();
    temperatures_index_.clear();
    number_of_temperatures_per_rank_.clear();
    vslDeleteStream(&RngStream_);
  }

  const int number_of_temperatures_on_this_rank(){
    return number_of_temperatures_per_rank_[rank_];
  }

  const int number_of_temperatures_per_rank(const int rank__){
    return number_of_temperatures_per_rank_[rank__];
  }

  const int number_of_temperatures() {
    return temperatures_.size();
  }

  const int number_of_spins() {
    return number_of_spins_;
  }

  const void temperatures(std::vector<double> temperatures__) {
    temperatures_ = temperatures__;
  }


  const double temperatures(const int index_) {
    return temperatures_[temperatures_offset_[rank_] + index_];
  }

  const int temperatures_offset() {
    return temperatures_offset_[rank_];
  }

  const int rank() const {
    return rank_;
  }

  const double temperatures(const int rank__, const int index_) {
    return temperatures_[temperatures_offset_[rank__] + index_];
  }

  void *allocate(const size_t size__) {
    return memory_pool_allocator_.allocate(size__);
  }

  void deallocate(void *ptr__) {
    memory_pool_allocator_.deallocate(ptr__);
  }

  void generate_rng_double(const int number_, double *rng__) {
    vdRngUniform(VSL_RNG_METHOD_UNIFORM_STD_ACCURATE, RngStream_, number_, rng__, 0, 1.0);
  }

  void generate_rng_int(const int number_, int *rng__) {
    viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, RngStream_, number_, rng__, 0, number_of_spins_);
  }

  void generate_random_spins(utils::mdarray<double, 5, CblasRowMajor> &spins_) {
    utils::mdarray<double, 5> rng(spins_.size(0), spins_.size(2), spins_.size(3), spins_.size(4), 2);
    vdRngUniform(VSL_RNG_METHOD_UNIFORM_STD_ACCURATE, RngStream_, rng.size(), rng.at<utils::CPU>(0, 0, 0, 0, 0), 0, 1.0);
    memset(spins_.at<utils::CPU>(), 0, sizeof(double) * spins_.size());
    for(int T = 0; T < spins_.size(0); T++) {
      for (int ns = 0; ns < spins_.size(2); ns++) {
        for (int y = 0; y < spins_.size(3); y++) {
          for (int x = 0; x < spins_.size(4); x++) {
            double si[3];
            double pc, ps, ts, tc;
            sincos(2.0 * rng(T, ns, y, x, 0) * M_PI, &ps, &pc);
            tc = 2.0 * rng(T, ns, y, x, 1) - 1.0;
            // 1 - tc ^ 2
            ts = sqrt(fma(tc, -tc, 1.0));
            spins_(T, 0, ns, y, x) = pc * ts;
            spins_(T, 1, ns, y, x) = ps * ts;
            spins_(T, 2, ns, y, x) = tc;
          }
        }
      }
    }
    rng.clear();
  }

  void generate_ferromagnetic_spins(utils::mdarray<double, 5, CblasRowMajor> &spins_) {
    for(int T = 0; T < spins_.size(0); T++) {
      for (int ns = 0; ns < spins_.size(2); ns++) {
        for (int y = 0; y < spins_.size(3); y++) {
          for (int x = 0; x < spins_.size(4); x++) {
            spins_(T, 0, ns, y, x) = 0.0;
            spins_(T, 1, ns, y, x) = 0.0;
            spins_(T, 2, ns, y, x) = 1.0;
          }
        }
      }
    }
  }
  void index_sort_temperatures(std::vector<double> &temperatures__) {
    gsl_sort_largest_index(this->temperatures_index_.data(), temperatures__.size(), temperatures__.data(), 1, temperatures__.size());
  }

  void sort_temperatures(std::vector<double> &temperatures__) {
    gsl_sort_largest_index(this->temperatures_index_.data(), temperatures__.size(), temperatures__.data(), 1, temperatures__.size());
    std::vector<double> temp_(temperatures__.size());
    for (int i = 0; i < temp_.size(); i++)
      temp_[i] = temperatures__[this->temperatures_index_[i]];
    memcpy(temperatures__.data(), temp_.data(), sizeof(double) * temp_.size());
  }

  void sort_total_energies(std::vector<double> &temperatures__, std::vector<double> &energies__) {
    index_sort_temperatures(temperatures__);
    std::vector<double> energy_(energies__.size());
    for (int i = 0; i < energies__.size(); i++)
      energy_[i] = energies__[temperatures_index_[i]];
    memcpy(energies__.data(), energy_.data(), sizeof(double) * energy_.size());
  }

  void sort_total_energies(std::vector<double> &energies__) {
    index_sort_temperatures(temperatures_);
    std::vector<double> energy_(energies__.size());
    for (int i = 0; i < energies__.size(); i++)
      energy_[i] = energies__[temperatures_index_[i]];
    memcpy(energies__.data(), energy_.data(), sizeof(double) * energy_.size());
  }

  const int neighbors(const int i, const int j) {
    return neighbors_(i, j);
  }

  void initialize_temperatures(const std::string &distribution__, const double TMax__, const double TMin__, std::vector<double> &temperatures__){
    temperatures__.clear();
    temperatures__.resize(temperatures_.size());

    if (distribution__ == "linear") {
      temperatures__[0] = TMax__;
      const double TSteps = (TMax__ - TMin__) / temperatures__.size();
      for (auto T = 0u; T < temperatures__.size(); T++) {
        temperatures__[T] = TMax__ - T * TSteps;
      }
      return;
    }

    if (distribution__ == "logscale") {
      for (auto T = 0u; T < temperatures__.size(); T++) {
        temperatures__[T] = exp(log(TMax__) - (log(TMax__) - log(TMin__)) * T / ((double)(temperatures__.size() - 1)));
      }
      return;
    }
  }

  int size() {
    return size_;
  }
};
#endif
