#include <iostream>
#include "Shuriken.h"
namespace lattice_model = Shuriken;
#include "Context.hpp"
#include "mdarray.hpp"
#include "types.hpp"
#include "MonteCarlo.hpp"


/// handler to umpire allocator_
umpire::Allocator allocator_;
/// handler to umpire memory pool
umpire::Allocator memory_pool_allocator_;


class simulated_annealing {
  context &ctx_;
  monte_carlo &mc_;
  int simulated_annealing_steps_ {10000};
  std::vector<double> target_temperatures_;
  std::vector<double> current_temperatures_;
  double starting_temperature_{10.0};

public:
  simulated_annealing(context &ctx__, monte_carlo &mc__, const int simulated_annealing_steps__, const std::vector<double> &temperatures__) :
    ctx_(ctx__),
    mc_(mc__),
    simulated_annealing_steps_(simulated_annealing_steps__)
  {
    target_temperatures_ = temperatures__;
    current_temperatures_.clear();
    current_temperatures_.resize(temperatures__.size());
  };

  ~simulated_annealing() {
    current_temperatures_.clear();
    target_temperatures_.clear();
  }

  void run(std::vector<double> &temperatures__, utils::mdarray<double, 5, CblasRowMajor> &spins__) {
    target_temperatures_ = temperatures__;
    for (int step = 0; step < simulated_annealing_steps_; step++) {
      calculate_temperatures(step);
      mc_.run(current_temperatures_, spins__);
    }
  }
private:
  void calculate_temperatures(const int sim_ann_step__) {
    if (sim_ann_step__ == 0) {
      for (auto &a : current_temperatures_)
        a = starting_temperature_;
      return;
    }

    for (int T = 0; T < current_temperatures_.size(); T++) {
      const double DeltaT = (starting_temperature_ - target_temperatures_[T]) / static_cast<double>(simulated_annealing_steps_ - 1);
      current_temperatures_[T] = starting_temperature_ - DeltaT * sim_ann_step__;
    }
  }

};

int main(int argc, char **argv)
{
  int system_size_[4] = {6, 6, 6, 0};
  int number_of_temperatures_ = 4;
  int number_of_measures_ = 10;
  int simulated_annealing_steps_ = 100;
  int mc_step_ {1};
  std::vector<double> temperatures_;
  std::vector<double> energies_;

  MPI_Init(&argc, &argv);

  auto& rm_  = umpire::ResourceManager::getInstance();
  allocator_ = rm_.getAllocator("HOST");
  memory_pool_allocator_ =
    rm_.makeAllocator<umpire::strategy::AlignedAllocator>("aligned_allocator", allocator_, 256);


  context ctx_(memory_pool_allocator_, system_size_, number_of_temperatures_, 0x342);

  ctx_.initialize_temperatures("logscale", 10.0, 0.001, temperatures_);
  energies_.resize(temperatures_.size());

  monte_carlo mc_simulated_annealing(ctx_, 10, 5, 0, 0);
  monte_carlo mc_thermalization(ctx_, 20000, 5, 0, 0);
  monte_carlo mc_measure(ctx_, 2, 5, 0, 0);

  utils::mdarray<double, 2> energies_measures(temperatures_.size(), number_of_measures_);
  utils::mdarray<double, 5, CblasRowMajor> spins_(ctx_.number_of_temperatures_on_this_rank(),
                                                  3,
                                                  lattice_model::number_of_spins_per_unit_cell(),
                                                  system_size_[1],
                                                  system_size_[0]);
  simulated_annealing sim_am(ctx_, mc_simulated_annealing, simulated_annealing_steps_, temperatures_);

  ctx_.generate_random_spins(spins_);

  sim_am.run(temperatures_, spins_);
  mc_simulated_annealing.calculate_total_energy(spins_, temperatures_, energies_);

  if (ctx_.rank() == 0) {
    for (int T = 0; T < temperatures_.size(); T++) {
      std::cout << temperatures_[T] << " " << energies_[T] << std::endl;
    }
  }

  mc_thermalization.run(temperatures_, spins_);

  for (int ms = 0; ms < number_of_measures_; ms++) {
    mc_measure.run(temperatures_, spins_);
    mc_measure.calculate_total_energy(spins_, temperatures_, energies_);
    for (int i = 0; i < energies__.size(); i++)
      energy_measures_.at<utils::CPU>(i, ms) = energies__[i];
  }

  if (ctx_.rank() == 0) {
    for (int T = 0; T < temperatures_.size(); T++) {
      std::cout << temperatures_[T] << " " << energies_[T] << std::endl;
    }
  }

  spins_.clear();
  temperatures_.clear();
  energies_.clear();
  MPI_Finalize();
  exit(0);
}
