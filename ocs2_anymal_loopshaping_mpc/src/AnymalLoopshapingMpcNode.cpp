//
// Created by rgrandia on 13.02.20.
//

#include <ros/init.h>

#include <ocs2_ddp/DDP_Settings.h>
#include <ocs2_mpc/MPC_Settings.h>
#include <ocs2_quadruped_loopshaping_interface/QuadrupedLoopshapingMpcNode.h>
#include <ocs2_quadruped_loopshaping_interface/QuadrupedLoopshapingSlqMpc.h>

#include "ocs2_anymal_loopshaping_mpc/AnymalLoopshapingInterface.h"

int main(int argc, char* argv[]) {
  std::vector<std::string> programArgs{};
  ::ros::removeROSArgs(argc, argv, programArgs);
  if (programArgs.size() < 3) {
    throw std::runtime_error("No robot name and config folder specified. Aborting.");
  }
  const std::string robotName(programArgs[1]);
  const std::string configName(programArgs[2]);

  // Initialize ros node
  ros::init(argc, argv, "anymal_loopshaping_mpc");
  ros::NodeHandle nodeHandle;

  auto anymalInterface =
      anymal::getAnymalLoopshapingInterface(anymal::stringToAnymalModel(robotName), anymal::getConfigFolderLoopshaping(configName));
  const auto mpcSettings = ocs2::mpc::loadSettings(anymal::getTaskFilePathLoopshaping(configName));
  const auto ddpSettings = ocs2::ddp::loadSettings(anymal::getTaskFilePathLoopshaping(configName));

  auto mpcPtr = getMpc(*anymalInterface, mpcSettings, ddpSettings);
  quadrupedLoopshapingMpcNode(nodeHandle, *anymalInterface, std::move(mpcPtr));

  return 0;
}
