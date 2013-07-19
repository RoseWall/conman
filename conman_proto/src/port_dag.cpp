
#include <rtt/os/main.h>

#include <ocl/DeploymentComponent.hpp>
#include <ocl/TaskBrowser.hpp>
#include <ocl/LoggingService.hpp>
#include <rtt/Logger.hpp>


namespace conman {
  typedef enum {
    UNRESTRICTED = 0,
    EXCLUSIVE,
  } ResourceType;

  // TODO: replace this with a class with friendler member functions
  typedef std::map<std::string, std::map<std::string,int> > ResourceMap;
}


class Controller : public RTT::TaskContext {
public:
  Controller(std::string const& name, std::string const& group) :
    TaskContext(name, RTT::base::TaskCore::PreOperational),
    group_(group)
  {
    // Get group name from rosparam
     
    // Store provided interfaces
    output_service_ = this->provides("out");
    input_service_ = this->provides("in");
      
    // Create RTT ports
    output_service_->provides(group)->addPort("joint_effort", effort_out_);
    input_service_->provides(group)->addPort("joint_effort", effort_in_);

    // Define resource types
    input_resource_map_[group]["joint_effort"] = conman::EXCLUSIVE;
    output_resource_map_[group]["joint_effort"] = conman::EXCLUSIVE;
  }

  const conman::ResourceMap get_input_resources() { 
    return input_resource_map_; 
  }
  const conman::ResourceMap get_output_resources() {
    return output_resource_map_; 
  }

  bool configureHook() {
    //bool ready = this->requires("in")->requires(group_)->getReferencedService->getPort("joint_effort")
    
    // Ready if the input is connected
    bool ready = effort_in_.connected();

    return ready;
  }
  
  std::string group_;

  RTT::Service::shared_ptr input_service_;
  RTT::Service::shared_ptr output_service_;

  RTT::InputPort<double> effort_in_;
  RTT::OutputPort<double> effort_out_;

  // Resource maps
  conman::ResourceMap 
    input_resource_map_,
    output_resource_map_;
};

class ControllerManager : public OCL::DeploymentComponent {
public:
  ControllerManager(std::string name="ControllerManager") :
    OCL::DeploymentComponent(name)
  {
    
  }

  //! Controllers
  // load controller (name) 
  //  This is where interfaces and ports get connected
  // enable controller (name)
  //  This is where resource exclusion is checked
  // dosable controller (name)
  // switch controllers (name)

  //! State Estimators
  // load estimator (name)
  // enable estimator (name)
  // disable estimator (name)
  // switch estimators (name)

  //! Execution
  // read from hardware ()
  // compute feedback ()
  // compute control ()
  // write to hardware ()

protected:

  // serialized feedback graph
  // serialized control graph

  std::map<std::string,std::vector<std::string> > resources_; 
};

int ORO_main(int argc, char** argv) {

  RTT::Logger::log().setStdStream(std::cerr);
  RTT::Logger::log().mayLogStdOut(true);
  RTT::Logger::log().setLogLevel(RTT::Logger::Info);

  RTT::Logger::In in("Prototype");

  std::vector<std::string> components;
  std::map<std::string,std::vector<std::string> > resources; 

  Controller c0("c0","left_arm");
  Controller c1("c1","left_arm");
  Controller c2("c2","left_arm");

  {
    OCL::DeploymentComponent deployer("ControllerManager"); 

    // Create some controllers
    deployer.connectPeers(&c0);
    deployer.connectPeers(&c1);
    deployer.connectPeers(&c2);

    // Get the control groups of a given controller
    RTT::Logger::log() << RTT::Logger::Info << "Control groups: " << RTT::endlog();

    deployer.connect("c0.out.left_arm.joint_effort","c1.in.left_arm.joint_effort",RTT::ConnPolicy());

    OCL::TaskBrowser task_browser(&deployer);

    task_browser.loop();
  }

  return 0;
}

/**

  connect("c0.left_arm__in.joint_effort","c1.left_arm__out.joint_effort",ConnPolicy())

  **/
