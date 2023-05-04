
#include "types.hpp"

__inline__ void
__Generate3DVector(const double rng1, const double rng2, double* __restrict__ spin)
{
  double pc, ps, ts, tc;
  sincos(2.0 * rng1 * M_PI, &ps, &pc);
  tc = 2.0 * rng2 - 1.0;
  // 1 - tc ^ 2
  ts = sqrt(fma(tc, -tc, 1.0));

  spin[0] = pc * ts;
  spin[1] = ps * ts;
  spin[2] = tc;
}


__inline__ int
metropolis(const utils::double3 &ExchangeField, const double InverseTemperature, const utils::double3* __restrict__ tmp,
           const double rng, utils::double3* __restrict__ out)
{
    /* Energy = - sum_j  (J sum_<i,j> s_i) s_j */
    const double LocalEnergyBefore = ExchangeField.x * tmp->x + ExchangeField.y * tmp->y + ExchangeField.z * tmp->z;
    /* // call the RandomSpin routine */
    const double LocalEnergyAfter = ExchangeField.x * out->x + ExchangeField.y * out->y + ExchangeField.z * out->z;
    const double DE               = LocalEnergyBefore - LocalEnergyAfter;

    const double BoltzmanWeight = exp(DE * InverseTemperature);
    const int test              = rng < std::min(1.0, BoltzmanWeight);

    out->x = out->x * test + tmp->x * (!test);
    out->y = out->y * test + tmp->y * (!test);
    out->z = out->z * test + tmp->z * (!test);

    return test;
}

__inline__ void metropolis_update(const bool ReduceAngle,
                                  const int spin_index,
                                  const utils::double3 &ExchangeField,
                                  const double InverseTemperature,
                                  const utils::double4 &rng,
                                  const double CosTheta,
                                  double* __restrict__ Sx,
                                  double* __restrict__ Sy,
                                  double *__restrict__ Sz)
{
  utils::double3 out;
  utils::double3 tmp;

  tmp.x = Sx[spin_index];
  tmp.y = Sy[spin_index];
  tmp.z = Sz[spin_index];

  __Generate3DVector(rng.x, rng.y, (double*)&out);

  metropolis(ExchangeField, InverseTemperature, &tmp, rng.z, &out);
  //_CudaRotate3DVector(2 * M_PI * rng.w, ExchangeField, &out);

  Sx[spin_index] = out.x;
  Sy[spin_index] = out.y;
  Sz[spin_index] = out.z;
}

__inline__ void
heat_bath_update(const int spin_index,
                 const double InverseTemperature,
                 utils::double3 &ExchangeField,
                 const utils::double2 &rg,
                 double* __restrict__ Sx,
                 double* __restrict__ Sy,
                 double *__restrict__ Sz)
{
  double h = utils::norm3d(ExchangeField.x, ExchangeField.y, ExchangeField.z);
  
  double cosx1, cosx2, cosy2, sinx1, siny1, sinx2, siny2;
  // SINCOS(theta, &ts, &tc);
  {
    const double tp = exp(-2.0 * h * InverseTemperature);
    cosx1           = -1.0 - log(tp * (1.0 - rg.x) + rg.x) / (h * InverseTemperature);
  }
  sinx1 = sqrt(1.0 - cosx1 * cosx1);
  sincos(2.0 * rg.y * M_PI, &sinx2, &cosx2);
  h = 1.0 / h;
  ExchangeField.x *= h;
  ExchangeField.y *= h;
  ExchangeField.z *= h;
  siny1 = sqrt(1.0 - ExchangeField.x * ExchangeField.x);
  cosy2 = ExchangeField.y / siny1;
  siny2 = ExchangeField.z / siny1;
  
  double o0   = cosx1 * ExchangeField.x - cosx2 * sinx1 * siny1;
  double o1   = cosx1 * ExchangeField.y + cosy2 * sinx1 * cosx2 * ExchangeField.x - sinx1 * sinx2 * siny2;
  double o2   = cosx1 * ExchangeField.z + cosx2 * sinx1 * siny2 * ExchangeField.x + sinx1 * sinx2 * cosy2;
  double norm = 1.0 / utils::norm3d(o0, o1, o2);
  
  Sx[spin_index] = o0 * norm;
  Sy[spin_index] = o1 * norm;
  Sz[spin_index] = o2 * norm;
}

__inline__ void
heat_bath_update2(const int spin_index,
                  const double InverseTemperature,
                  utils::double3 &in,
                  const utils::double2 rg,
                  double *__restrict__ Sx,
                  double *__restrict__ Sy,
                  double *__restrict__ Sz)
{
  double h = utils::norm3d(in.x, in.y, in.z);
  double ts, tc, ps, pc;
  // SINCOS(theta, &ts, &tc);
  {
    const double tp = exp(-2.0 * h * InverseTemperature);
    tc              = -1.0 - log(tp * (1.0 - rg.x) + rg.x) / (h * InverseTemperature);
  }
  ts = sqrt(1.0 - tc * tc);
  sincos(2.0 * rg.y * M_PI, &ps, &pc);
  h = 1.0 / h;
  in.x *= h;
  in.y *= h;
  in.z *= h;
  
  const bool test = (in.z > -0.9999999999999999999999);
  double N0       = (in.x + !test);
  double N1       = in.y;
  double N2       = (in.z + test);
  double r1       = ((test) ? (ts * ps) : (-tc));
  double r2       = ((test) ? (tc) : (ts * ps));
  double r0       = ts * pc;
  
  // dot product
  //
  
  // this->RS[0] = 2*N[0]*(N[0]*rt[0]+N[1]*rt[1]+N[2]*rt[2])-rt[0];
  //             = 2*N[0]*(N . rt) -rt[0];
  // this->RS    = 2*(N . rt) N - rt;
  double scal = 1.0 / (1.0 + ((test) ? in.z : in.x));
  scal *= N0 * r0 + N1 * r1 + N2 * r2;
  
  double o0 = fma(N0, scal, -r0);
  double o1 = fma(N1, scal, -r1);
  double o2 = fma(N2, scal, -r2);
  
  double norm = 1.0 / utils::norm3d(o0, o1, o2);
  
  Sx[spin_index] = o0 * norm;
  Sy[spin_index] = o1 * norm;
  Sz[spin_index] = o2 * norm;
}

__inline__ void
rotate_vector3d(const double angle, const utils::double3 &b, utils::double3 &a)
{
  // using quaternions for the rotation
  // q = n * sin (theta/2)
  // v' = v + 2 q x (qxv + cos(theta/2) v)

  const double vx = a.x;
  const double vy = a.y;
  const double vz = a.z;

  double ct; // COS(angle*0.5 * 2 * M_PI);
  double st; // SIN(angle*0.5 * 2 * M_PI);

  sincos(angle * M_PI, &st, &ct);

  const double normN = st * utils::norm3d(b.x, b.y, b.z);

  const double qx = b.x * normN;
  const double qy = b.y * normN;
  const double qz = b.z * normN;

  const double tx = fma(ct, vx, fma(qy, vz, -qz * vy));
  const double ty = fma(ct, vy, fma(qz, vx, -qx * vz));
  const double tz = fma(ct, vz, fma(qx, vy, -qy * vx));

  a.x = vx + 2.0 * fma(qy, tz, -qz * ty);
  a.y = vy + 2.0 * fma(qz, tx, -qx * tz);
  a.z = vz + 2.0 * fma(qx, ty, -qy * tx);
}

__inline__ void over_relaxation_update(const int spin_index,
                                                  const double angle,
                                                  const utils::double3 &ExchangeField,
                                                  double *__restrict__ Sx,
                                                  double *__restrict__ Sy,
                                                  double *__restrict__ Sz)
{
  utils::double3 out;
  out.x = Sx[spin_index];
  out.y = Sy[spin_index];
  out.z = Sz[spin_index];

  rotate_vector3d(angle, ExchangeField, out);

  Sx[spin_index] = out.x;
  Sy[spin_index] = out.y;
  Sz[spin_index] = out.z;
}


/*
__inline__ void over_relaxation_update_by_pi(const int spin_index,
                                                        const utils::double3 &ExchangeField,
                                                        double* __restrict__ Sx,
                                                        double* __restrict__ Sy,
                                                        double *__restrict__ Sz)
{
  utils::double3 out;
  out.x = Sx[spin_index];
  out.y = Sy[spin_index];
  out.z = Sz[spin_index];

  _CudaRotate3DVectorByPi(ExchangeField, &out);

  Sx[spin_index] = out.x;
  Sy[spin_index] = out.y;
  Sz[spin_index] = out.z;
}*/
