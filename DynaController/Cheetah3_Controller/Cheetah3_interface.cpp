#include "Cheetah3_interface.hpp"
#include <stdio.h>
#include <math.h>
#include <string>
#include <Utils/utilities.hpp>
#include <Utils/pseudo_inverse.hpp>
#include <Utils/DataManager.hpp>
#include <Utils/wrap_eigen.hpp>
#include "Cheetah3_StateProvider.hpp"
#include "Cheetah3_StateEstimator.hpp"
#include <ParamHandler/ParamHandler.hpp>
#include <Cheetah3/Cheetah3_Model.hpp>

// Walking Test
#include <Cheetah3_Controller/TestSet/WalkingTest.hpp>

// Body Ctrl Test
#include <Cheetah3_Controller/TestSet/BodyCtrlTest.hpp>

Cheetah3_interface::Cheetah3_interface():
    interface(),
    jjvel_(cheetah3::num_act_joint),
    jjpos_(cheetah3::num_act_joint),
    jtorque_(cheetah3::num_act_joint),
    torque_command_(cheetah3::num_act_joint),
    jpos_command_(cheetah3::num_act_joint),
    jvel_command_(cheetah3::num_act_joint),
    waiting_count_(2)
{
    robot_sys_ = new Cheetah3_Model();
    jjvel_.setZero();
    jjpos_.setZero();
    jtorque_.setZero();

    torque_command_.setZero();
    jpos_command_.setZero();
    jvel_command_.setZero();
 
    test_cmd_ = new Cheetah3_Command();
    sp_ = Cheetah3_StateProvider::getStateProvider();
    state_estimator_ = new Cheetah3_StateEstimator(robot_sys_);  
    
    DataManager::GetDataManager()->RegisterData(
            &running_time_, DOUBLE, "running_time");
    DataManager::GetDataManager()->RegisterData(
            &jjpos_, DYN_VEC, "jjpos", cheetah3::num_act_joint);
    DataManager::GetDataManager()->RegisterData(
            &jjvel_, DYN_VEC, "jjvel", cheetah3::num_act_joint);
     DataManager::GetDataManager()->RegisterData(
            &jtorque_, DYN_VEC, "torque", cheetah3::num_act_joint);
  
    DataManager::GetDataManager()->RegisterData(
            &jpos_command_, DYN_VEC, "jpos_des", cheetah3::num_act_joint);
    DataManager::GetDataManager()->RegisterData(
            &jvel_command_, DYN_VEC, "jvel_des", cheetah3::num_act_joint);
    DataManager::GetDataManager()->RegisterData(
            &torque_command_, DYN_VEC, "command", cheetah3::num_act_joint);

    _ParameterSetting();
    
    printf("[Cheetah3_interface] Contruct\n");
}

Cheetah3_interface::~Cheetah3_interface(){
    delete test_;
}

void Cheetah3_interface::GetCommand( void* _data, void* _command){
    Cheetah3_Command* cmd = ((Cheetah3_Command*)_command);
    Cheetah3_SensorData* data = ((Cheetah3_SensorData*)_data);

    if(!_Initialization(data)){
        state_estimator_->Update(data);
        test_->getCommand(test_cmd_);
    }
    
    // Update Command (and Data)
    for(int i(0); i<cheetah3::num_act_joint; ++i){
        torque_command_[i] = test_cmd_->jtorque_cmd[i];
        jpos_command_[i] = test_cmd_->jpos_cmd[i];
        jvel_command_[i] = test_cmd_->jvel_cmd[i];

        jjvel_[i] = data->jvel[i];
        jjpos_[i] = data->jpos[i];
        jtorque_[i] = data->jtorque[i];
    }

    for(int i(0); i< cheetah3::num_act_joint; ++i){
        cmd->jtorque_cmd[i] = torque_command_[i];
        cmd->jpos_cmd[i] = jpos_command_[i];
        cmd->jvel_cmd[i] = jvel_command_[i];
    }

    running_time_ = (double)(count_) * cheetah3::servo_rate;
    ++count_;
    // When there is sensed time
    sp_->curr_time_ = running_time_;
    
    ////// Stepping forward
    double walking_start(3.);
    double walking_duration(8.);
    double walking_distance(2.5);
    if(sp_->curr_time_ > walking_start){
        double walking_time = sp_->curr_time_ - walking_start;
        sp_->des_location_[0] = walking_distance * 
            walking_time/walking_duration;
            //(1-cos(walking_time/walking_duration * M_PI))/2.;
    }
    if(sp_->curr_time_ > walking_start + walking_duration){
        sp_->des_location_[0] = walking_distance;
    }
}

bool Cheetah3_interface::_Initialization(Cheetah3_SensorData* data){
    static bool test_initialized(false);
    if(!test_initialized) {
        test_->TestInitialization();
        test_initialized = true;
        printf("[Cheetah3 Interface] Test initialization is done\n");
    }
    if(count_ < waiting_count_){
        for(int i(0); i<cheetah3::num_act_joint; ++i){
            test_cmd_->jtorque_cmd[i] = 0.;
            test_cmd_->jpos_cmd[i] = data->jpos[i];
            test_cmd_->jvel_cmd[i] = 0.;
        }
        state_estimator_->Initialization(data);

        DataManager::GetDataManager()->start();
        return true;
    }
    return false;
}

void Cheetah3_interface::_ParameterSetting(){
    ParamHandler handler(Cheetah3ConfigPath"INTERFACE_setup.yaml");
    std::string tmp_string;
    bool b_tmp;
    // Test SETUP
    handler.getString("test_name", tmp_string);
    // Walking Test ***********************************
    if(tmp_string == "walking_config_test"){
        test_ = new WalkingTest(robot_sys_);
        // Body Ctrl Test ***********************************
    }else if(tmp_string == "body_ctrl_test"){
        test_ = new BodyCtrlTest(robot_sys_);    
        // Stance and Swing Test ***********************************
    }else {
        printf("[Interfacce] There is no test matching with the name\n");
        exit(0);
    }
    printf("[Cheetah3_interface] Parameter setup is done\n");
}
