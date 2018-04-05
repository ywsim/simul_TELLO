#include "CoMzRxRyRzCtrl.hpp"
#include <Configuration.h>
#include <Mercury_Controller/Mercury_StateProvider.hpp>
#include <Mercury_Controller/TaskSet/CoMBodyOriTask.hpp>
#include <Mercury_Controller/ContactSet/DoubleContact.hpp>
#include <WBDC/WBDC.hpp>
#include <Mercury/Mercury_Model.hpp>
#include <Mercury/Mercury_Definition.h>
#include <ParamHandler/ParamHandler.hpp>

CoMzRxRyRzCtrl::CoMzRxRyRzCtrl(RobotSystem* robot): Controller(robot),
                                  dim_ctrl_(6),
                                  end_time_(100.),
                                  body_pos_ini_(4),
                                  b_set_height_target_(false),
                                  ctrl_start_time_(0.)
{
  amp_ = dynacore::Vector::Zero(dim_ctrl_);
  freq_= dynacore::Vector::Zero(dim_ctrl_);
  phase_= dynacore::Vector::Zero(dim_ctrl_);

  body_task_ = new CoMBodyOriTask(robot);
  double_contact_ = new DoubleContact(robot);

  std::vector<bool> act_list;
    act_list.resize(mercury::num_qdot, true);
    for(int i(0); i<mercury::num_virtual; ++i) act_list[i] = false;

  wbdc_ = new WBDC(act_list);

  wbdc_data_ = new WBDC_ExtraData();
  wbdc_data_->tau_min = dynacore::Vector(mercury::num_act_joint);
  wbdc_data_->tau_max = dynacore::Vector(mercury::num_act_joint);

  for(int i(0); i<mercury::num_act_joint; ++i){
    wbdc_data_->tau_max[i] = 150.0;
    wbdc_data_->tau_min[i] = -150.0;
  }
  sp_ = Mercury_StateProvider::getStateProvider();
}

CoMzRxRyRzCtrl::~CoMzRxRyRzCtrl(){
  delete body_task_;
  delete double_contact_;
  delete wbdc_;
  delete wbdc_data_;
}

void CoMzRxRyRzCtrl::OneStep(dynacore::Vector & gamma){
  _PreProcessing_Command();
    state_machine_time_ = sp_->curr_time_ - ctrl_start_time_;
  gamma.setZero();
  _double_contact_setup();
  _body_task_setup();
  _body_ctrl(gamma);

  _PostProcessing_Command();
}

void CoMzRxRyRzCtrl::_body_ctrl(dynacore::Vector & gamma){
  wbdc_->UpdateSetting(A_, Ainv_, coriolis_, grav_);
  wbdc_->MakeTorque(task_list_, contact_list_, gamma, wbdc_data_);

  for(int i(0); i<6; ++i)
    sp_->reaction_forces_[i] = wbdc_data_->opt_result_[i];
}

void CoMzRxRyRzCtrl::_body_task_setup(){
  dynacore::Vector pos_des(3 + 4);
  dynacore::Vector vel_des(body_task_->getDim());
  dynacore::Vector acc_des(body_task_->getDim());
  pos_des.setZero(); vel_des.setZero(); acc_des.setZero();

  // CoM Pos
  pos_des.head(3) = ini_com_pos_;
  if(b_set_height_target_)
    pos_des[2] = des_com_height_;

  double amp, omega, phase;
  for(int i(0); i<3; ++i){
    omega = 2. * M_PI * freq_[i];
    amp = amp_[i];
    phase = phase_[i];

    pos_des[i] += amp * sin(omega * state_machine_time_ + phase);
    vel_des[i] = amp * omega * cos(omega * state_machine_time_ + phase);
    acc_des[i] = -amp * omega * omega * sin(omega * state_machine_time_ + phase);
  }

  // Orientation
  dynacore::Vect3 rpy_des;
  rpy_des.setZero();

  for(int i(0); i<3; ++i){
    omega = 2. * M_PI * freq_[i+3];
    amp = amp_[i+3];
    phase = phase_[i+3];

    rpy_des[i] += amp * sin(omega * state_machine_time_ + phase);
    vel_des[i + 3] = amp * omega * cos(omega * state_machine_time_ + phase);
    acc_des[i + 3] = -amp * omega * omega * sin(omega * state_machine_time_ + phase);
  }
  dynacore::Quaternion quat_des;
  dynacore::convert(rpy_des, quat_des);

  pos_des[3] = quat_des.w();
  pos_des[4] = quat_des.x();
  pos_des[5] = quat_des.y();
  pos_des[6] = quat_des.z();

  body_task_->UpdateTask(&(pos_des), vel_des, acc_des);

  // set relaxed op direction
  // cost weight setup
  // bool b_height_relax(false);
  std::vector<bool> relaxed_op(body_task_->getDim(), true);
  body_task_->setRelaxedOpCtrl(relaxed_op);

  int prev_size(wbdc_data_->cost_weight.rows());
  wbdc_data_->cost_weight.conservativeResize( prev_size + dim_ctrl_);
  wbdc_data_->cost_weight[prev_size] = 0.0001;
  wbdc_data_->cost_weight[prev_size+1] = 0.0001;
  wbdc_data_->cost_weight[prev_size+2] = 100.;
  wbdc_data_->cost_weight[prev_size+3] = 100.;
  wbdc_data_->cost_weight[prev_size+4] = 100.;
  wbdc_data_->cost_weight[prev_size+5] = 0.001;

  // Push back to task list
  task_list_.push_back(body_task_);
  // dynacore::pretty_print(wbdc_data_->cost_weight,std::cout, "cost weight");
}
void CoMzRxRyRzCtrl::_double_contact_setup(){
  double_contact_->UpdateContactSpec();
  contact_list_.push_back(double_contact_);
  wbdc_data_->cost_weight = dynacore::Vector::Zero(double_contact_->getDim());
  for(int i(0); i<double_contact_->getDim(); ++i){
    wbdc_data_->cost_weight[i] = 1.;
  }
  wbdc_data_->cost_weight[2] = 0.0001;
  wbdc_data_->cost_weight[5] = 0.0001;
}

void CoMzRxRyRzCtrl::FirstVisit(){
  // printf("[CoMzRxRyRz] Start\n");
  ctrl_start_time_ = sp_->curr_time_;
}

void CoMzRxRyRzCtrl::LastVisit(){
  // printf("[CoMzRxRyRz] End\n");
}

void CoMzRxRyRzCtrl::setAmp(const std::vector<double> & amp){
  for(int i(0); i< dim_ctrl_; ++i) amp_[i] = amp[i];
}
void CoMzRxRyRzCtrl::setFrequency(const std::vector<double> & freq){
  for(int i(0); i< dim_ctrl_; ++i) freq_[i] = freq[i];

}
void CoMzRxRyRzCtrl::setPhase(const std::vector<double> & phase){
  for(int i(0); i< dim_ctrl_; ++i) phase_[i] = phase[i];
}

bool CoMzRxRyRzCtrl::EndOfPhase(){
  if(state_machine_time_ > end_time_){
    return true;
  }
  return false;
}
void CoMzRxRyRzCtrl::CtrlInitialization(const std::string & setting_file_name){
  robot_sys_->getCoMPosition(ini_com_pos_);
  std::vector<double> tmp_vec;

  ParamHandler handler(MercuryConfigPath + setting_file_name + ".yaml");

  // Feedback Gain
  handler.getVector("Kp", tmp_vec);
  for(int i(0); i<tmp_vec.size(); ++i){
    ((CoMBodyOriTask*)body_task_)->Kp_vec_[i] = tmp_vec[i];
  }
  handler.getVector("Kd", tmp_vec);
  for(int i(0); i<tmp_vec.size(); ++i){
    ((CoMBodyOriTask*)body_task_)->Kd_vec_[i] = tmp_vec[i];
  }
}