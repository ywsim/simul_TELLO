#ifndef DOUBLE_CONTACT_BOUNDING_MERCURY
#define DOUBLE_CONTACT_BOUNDING_MERCURY

#include <WBDC/WBDC_ContactSpec.hpp>

class RobotSystem;
class Mercury_StateProvider;

class DoubleContactBounding: public WBDC_ContactSpec{
public:
  DoubleContactBounding(RobotSystem* robot, int trans_pt);
  virtual ~DoubleContactBounding();

  void setFzUpperLimit(double lim);

protected:
  int trans_pt_;

  virtual bool _UpdateJc();
  virtual bool _UpdateJcDotQdot();
  virtual bool _UpdateUf();
  virtual bool _UpdateInequalityVector();

  void _setU(double mu, dynacore::Matrix & U);

  RobotSystem* robot_sys_;
  Mercury_StateProvider* sp_;
};


#endif