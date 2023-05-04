#ifndef __TYPES_HPP__
#define __TYPES_HPP__

namespace utils {

class double3;

  class double2 {
  public:
    double x, y;
    
    double2() {};
    double2(const double x_, const double y_)
    {
      x = x_;
      y = y_;
    };
    
    ~double2() {
    };
    
    double2 operator+(double2 const &obj)
    {
      double2 res_;
      res_.x = x + obj.x;
      res_.y = y + obj.y;
      return res_;
    }
    
    double2 operator-(double2 const &obj)
    {
      double2 res_;
      res_.x = x - obj.x;
      res_.y = y - obj.y;
      return res_;
    }
    
    double2 operator*(double2 const &obj)
    {
      double2 res_;
      res_.x = x * obj.x;
      res_.y = y * obj.y;
      return res_;
    }
    
    double2 operator*(const double scal)
    {
      double2 res_;
      res_.x = x * scal;
      res_.y = y * scal;
      return res_;
    }
    
    double2 operator/(double2 const &obj)
    {
      double2 res_;
      res_.x = x / obj.x;
      res_.y = y / obj.y;
      return res_;
    }
  };


  class double3 {
  public:
    double x, y, z;
    
    double3 ()
    {};
    
    double3(const double x_, const double y_, const double z_)
    {
      x = x_;
      y = y_;
      z = z_;
    };
    
    ~double3() {
    };
    
    double3 operator+(double3 const &obj)
    {
      double3 res_;
      res_.x = x + obj.x;
      res_.y = y + obj.y;
      res_.z = z + obj.z;
      return res_;
    }
    
    double3 operator-(double3 const &obj)
    {
      double3 res_;
      res_.x = x - obj.x;
      res_.y = y - obj.y;
      res_.z = z - obj.z;
      return res_;
    }
    
    double3 operator*(double3 const &obj)
    {
      double3 res_;
      res_.x = x * obj.x;
      res_.y = y * obj.y;
      res_.z = z * obj.z;
      return res_;
    }
    
    double3 operator*(const double scal)
    {
      double3 res_;
      res_.x = x * scal;
      res_.y = y * scal;
      res_.z = z * scal;
      return res_;
    }
    
    double3 operator/(double3 const &obj)
    {
      double3 res_;
      res_.x = x / obj.x;
      res_.y = y / obj.y;
      res_.z = z / obj.z;
      return res_;
    }
  };

  class double4 {
  public:
    double x, y, z, w;
    
    double4 ()
    {};
    
    double4(const double x_, const double y_, const double z_, const double w_)
    {
      x = x_;
      y = y_;
      z = z_;
      w = w_;
    };
    
    ~double4() {
    };
    
    double4 operator+(double4 const &obj)
    {
      double4 res_;
      res_.x = x + obj.x;
      res_.y = y + obj.y;
      res_.z = z + obj.z;
      res_.w = w + obj.w;
      return res_;
    }
    
    double4 operator-(double4 const &obj)
    {
      double4 res_;
      res_.x = x - obj.x;
      res_.y = y - obj.y;
      res_.z = z - obj.z;
      res_.w = w - obj.w;
      return res_;
    }
    
    double4 operator*(double4 const &obj)
    {
      double4 res_;
      res_.x = x * obj.x;
      res_.y = y * obj.y;
      res_.z = z * obj.z;
      res_.w = w * obj.w;
      return res_;
    }
    
    double4 operator*(const double scal)
    {
      double4 res_;
      res_.x = x * scal;
      res_.y = y * scal;
      res_.z = z * scal;
      res_.w = w * scal;
      return res_;
    }
    
    double4 operator/(double4 const &obj)
    {
      double4 res_;
      res_.x = x / obj.x;
      res_.y = y / obj.y;
      res_.z = z / obj.z;
      res_.w = w / obj.w;
      return res_;
    }
  };

  inline double norm3d(double3 &a)
  {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
  }

  inline double norm3d(double x, double y, double z)
  {
    return sqrt(x * x + y * y + z * z);
  }

};
#endif
