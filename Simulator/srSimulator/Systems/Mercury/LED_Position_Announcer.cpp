#include "LED_Position_Announcer.hpp"
#include "Mercury_Dyn_environment.hpp"
#include <Addition/Data_Manager/data_protocol.h>
#include <Utils/comm_udp.hpp>


LED_Position_Announcer::LED_Position_Announcer(Mercury_Dyn_environment* dyn_env):
    socket_(0){
    dyn_env_ = dyn_env;
}

void LED_Position_Announcer::run(){
     printf("[LED Position Announcer] Start \n");
     
     mercury_message mercury_msg;
     led_link_idx_list_.clear();

     // Link idx list 
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("body_led0")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("body_led1")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("body_led2")->second) );

     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("rleg_led0")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("rleg_led1")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("rleg_led2")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("rleg_led3")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("rleg_led4")->second) );

     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("lleg_led0")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("lleg_led1")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("lleg_led2")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("lleg_led3")->second) );
     led_link_idx_list_.push_back( (dyn_env_->m_Mercury->link_idx_map_.find("lleg_led4")->second) );



     while(true){
         for(int j(0); j<NUM_MARKERS; ++j){
             mercury_msg.visible[j] = 1.;
             for (int i(0); i<3; ++i){
                 mercury_msg.data[3*j + i] = 
                     dyn_env_->m_Mercury->link_[ led_link_idx_list_[j] ]->GetMassCenter()[i]*1000.0;
             }
         }
         COMM::send_data(socket_, MOCAP_DATA_PORT, &mercury_msg, sizeof(mercury_message), IP_ADDR_MYSELF);
         usleep(1000);
     }
}

