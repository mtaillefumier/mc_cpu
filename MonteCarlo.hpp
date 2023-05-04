#ifndef __MONTECARLO_HPP__
#define __MONTECARLO_HPP__

#include "Context.hpp"
#include "mdarray.hpp"
#include "MonteCarloUpdate.hpp"

class monte_carlo {
  context &ctx_;
  int heat_bath_steps_{1};
  int metropolis_steps_{1};
  int over_relaxation_steps_{0};
  int mc_steps_{1};

public:
  monte_carlo(context &ctx__, int mc_steps__, int heat_bath_steps__, int metropolis_steps__, int over_relaxation_steps__): ctx_(ctx__) {
    mc_steps_ = mc_steps__;
    heat_bath_steps_= heat_bath_steps__;
    metropolis_steps_ = metropolis_steps__;
    over_relaxation_steps_= over_relaxation_steps__;
  };

  ~monte_carlo() {
  };

  void calculate_total_energy(utils::mdarray<double, 5, CblasRowMajor> &spins_, std::vector<double> &temperatures__, std::vector<double> &Energy_);
  void heat_bath(const std::vector<double> &temperatures_, utils::mdarray<double, 5, CblasRowMajor> &spins_);
  void calculate_local_exchange_field(double *__restrict Sx, double *__restrict Sy, double *__restrict Sz, const int spin_index, utils::double3 &ex);
  void run(std::vector<double> &temperatures__, utils::mdarray<double, 5, CblasRowMajor> &spins__);
  void heat_bath_cpu(const double temperature__, const int temperature_index__, utils::mdarray<double, 5, CblasRowMajor> &spins__);
  void metropolis_cpu(const double temperature__, const int temperature_index__, utils::mdarray<double, 5, CblasRowMajor> &spins__);
  double calculate_total_energy_cpu(const int T__, utils::mdarray<double, 5, CblasRowMajor> &spins);
};

__inline__ void monte_carlo::calculate_local_exchange_field(double *__restrict Sx, double *__restrict Sy, double *__restrict Sz, const int spin_index, utils::double3 &ex)
{
  ex.x = 0.0;
  ex.y = 0.0;
  ex.z = 0.0;
  for (int nei = 0; nei < lattice_model::number_of_neighbors(); nei++) {
    const int spin_index_neighbors = ctx_.neighbors(spin_index, nei);
    ex.x += Sx[spin_index_neighbors];
    ex.y += Sy[spin_index_neighbors];
    ex.z += Sz[spin_index_neighbors];
  }
}

__inline__ void monte_carlo::heat_bath_cpu(const double temperature__, const int temperature_index__, utils::mdarray<double, 5, CblasRowMajor> &spins__)
{
  double *Sx, *Sy, *Sz;
  int *rng_int = static_cast<int*>(ctx_.allocate(ctx_.number_of_spins() * sizeof(int) * heat_bath_steps_));
  double *rng_double = static_cast<double*>(ctx_.allocate(2 * ctx_.number_of_spins() * heat_bath_steps_ * sizeof(double)));
  const double inverse_temperature = 1.0 / temperature__;
  Sx = spins__.at<utils::CPU>(temperature_index__, 0, 0, 0, 0);
  Sy = spins__.at<utils::CPU>(temperature_index__, 1, 0, 0, 0);
  Sz = spins__.at<utils::CPU>(temperature_index__, 2, 0, 0, 0);
  ctx_.generate_rng_int(ctx_.number_of_spins() * heat_bath_steps_, rng_int);
  ctx_.generate_rng_double(2 * ctx_.number_of_spins() * heat_bath_steps_, rng_double);

  for (int step = 0; step < heat_bath_steps_; step++) {
    utils::double3 ExchangeField = {0, 0, 0};
    for (int i = 0; i < ctx_.number_of_spins(); i++) {
      const int spin_index = rng_int[i + step * ctx_.number_of_spins()] % ctx_.number_of_spins();
      calculate_local_exchange_field(Sx, Sy, Sz,
                                     spin_index,
                                     ExchangeField);
      heat_bath_update2(spin_index,
                        inverse_temperature,
                        ExchangeField,
                        ((utils::double2 *)(rng_double))[i + step * ctx_.number_of_spins()],
                        Sx, Sy, Sz);
    }
  }
  ctx_.deallocate(rng_int);
  ctx_.deallocate(rng_double);
}

void monte_carlo::metropolis_cpu(const double temperature__, const int temperature_index__, utils::mdarray<double, 5, CblasRowMajor> &spins__)
{
  double *Sx, *Sy, *Sz;
  int *rng_int = static_cast<int*>(ctx_.allocate(ctx_.number_of_spins() * sizeof(int)));
  double *rng_double = static_cast<double*>(ctx_.allocate(4 * ctx_.number_of_spins() * sizeof(double)));
  const double inverse_temperature = 1.0 / temperature__;
  Sx = spins__.at<utils::CPU>(temperature_index__, 0, 0, 0, 0);
  Sy = spins__.at<utils::CPU>(temperature_index__, 1, 0, 0, 0);
  Sz = spins__.at<utils::CPU>(temperature_index__, 2, 0, 0, 0);
  ctx_.generate_rng_int(ctx_.number_of_spins(), rng_int);
  ctx_.generate_rng_double(4 * ctx_.number_of_spins(), rng_double);

  utils::double3 ExchangeField = {0, 0, 0};
  for (int step = 0; step < this->heat_bath_steps_; step++) {
    for (int i = 0; i < ctx_.number_of_spins(); i++) {
      calculate_local_exchange_field(Sx, Sy, Sz, rng_int[i], ExchangeField);
      metropolis_update(false,
                        rng_int[i],
                        ExchangeField,
                        inverse_temperature,
                        ((utils::double4*)(rng_double))[i],
                        -1,
                        Sx,
                        Sy,
                        Sz);
    }
  }
  ctx_.deallocate(rng_int);
  ctx_.deallocate(rng_double);
}

double monte_carlo::calculate_total_energy_cpu(const int T__, utils::mdarray<double, 5, CblasRowMajor> &spins__)
{
  double *exchange_field_ = static_cast<double*>(ctx_.allocate(ctx_.number_of_spins() * 3 * sizeof(double)));
  double *Ex_ = exchange_field_;
  double *Ey_ = Ex_ + ctx_.number_of_spins();
  double *Ez_ = Ey_ + ctx_.number_of_spins();
  double *Sx_ = spins__.at<utils::CPU>(T__, 0, 0, 0, 0);
  double *Sy_ = spins__.at<utils::CPU>(T__, 1, 0, 0, 0);
  double *Sz_ = spins__.at<utils::CPU>(T__, 2, 0, 0, 0);
  utils::double3 ex_tmp;
  memset(exchange_field_, 0, sizeof(double) * ctx_.number_of_spins() * 3);
  for (int i = 0; i < ctx_.number_of_spins(); i++) {
    calculate_local_exchange_field(Sx_, Sy_, Sz_, i, ex_tmp);
    Ex_[i] = ex_tmp.x;
    Ey_[i] = ex_tmp.y;
    Ez_[i] = ex_tmp.z;
  }

  double res = cblas_ddot(ctx_.number_of_spins() * 3, spins__.at<utils::CPU>(T__, 0, 0, 0, 0), 1, exchange_field_, 1);
  ctx_.deallocate(exchange_field_);
  return res;
}


void monte_carlo::calculate_total_energy(utils::mdarray<double, 5, CblasRowMajor> &spins__, std::vector<double> &temperatures__, std::vector<double> &Energy__)
{
  const int number_of_temperatures = ctx_.number_of_temperatures_per_rank(ctx_.rank());
  double *Energy_tmp = static_cast<double *>(ctx_.allocate(temperatures__.size() * sizeof(double)));

  for (int T = 0; T < ctx_.number_of_temperatures_on_this_rank(); T++) {
    Energy_tmp[T] = calculate_total_energy_cpu(T, spins__);
  }

  //  if (ctx_.size() > 1)
  MPI_Allgather(Energy_tmp,
                ctx_.number_of_temperatures_on_this_rank(),
                MPI_DOUBLE,
                Energy__.data(),
                ctx_.number_of_temperatures_on_this_rank(),
                MPI_DOUBLE,
                MPI_COMM_WORLD);
  
  ctx_.index_sort_temperatures(temperatures__);
  ctx_.sort_total_energies(temperatures__, Energy__);
  ctx_.deallocate(Energy_tmp);
}

void monte_carlo::heat_bath(const std::vector<double> &temperatures_, utils::mdarray<double, 5, CblasRowMajor> &spins__)
{
  ctx_.temperatures(temperatures_);
  for (int T = 0; T < ctx_.number_of_temperatures_on_this_rank(); T++) {
    heat_bath_cpu(ctx_.temperatures(T), T, spins__);
  }
}

void monte_carlo::run(std::vector<double> &temperatures__, utils::mdarray<double, 5, CblasRowMajor> &spins__)
{
  ctx_.temperatures(temperatures__);
  #pragma omp parallel for
  for (int T = 0; T < ctx_.number_of_temperatures_on_this_rank(); T++) {
    for (int i = 0; i < mc_steps_; i++) {
      if (heat_bath_steps_) {
        heat_bath_cpu(ctx_.temperatures(T), T, spins__);
      }
      if (metropolis_steps_) {
        metropolis_cpu(ctx_.temperatures(T), T, spins__);
      }
    }
  }
}

#endif
