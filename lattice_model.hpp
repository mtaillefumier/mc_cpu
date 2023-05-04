class lattice_model {
  int total_number_of_parallel_updates_;
  int number_of_sublattices_per_update_;
  int number_of_parallel_updates_;
  int number_of_spins_per_unit_cell_;
  int number_of_spins_per_primitive_cell_;
  int number_of_neighbors_per_spin_;
  int number_of_links_;
  mdarrray<int, 2> independent_sub_lattices_;
  mdarray<int, 3> neighbors_;

  lattice_model() {
  };

  lattice_model(int number_of_spins_per_unit_cell__, int number_of_spins_per_primitive_cell__, int number_of_neighbors__, int dim__) {
    number_of_spins_per_unit_cell_ = number_of_spins_per_unit_cell__;
    number_of_spins_per_primitive_cell_ = number_of_spins_per_primitive_cell__;
  }
}
