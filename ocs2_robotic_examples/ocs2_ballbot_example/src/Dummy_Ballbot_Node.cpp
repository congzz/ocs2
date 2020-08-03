/******************************************************************************
Copyright (c) 2017, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

 * Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include <ocs2_comm_interfaces/SystemObservation.h>
#include <ocs2_comm_interfaces/ocs2_ros_interfaces/mrt/MRT_ROS_Dummy_Loop.h>
#include <ocs2_comm_interfaces/ocs2_ros_interfaces/mrt/MRT_ROS_Interface.h>
#include <ros/init.h>

#include "ocs2_ballbot_example/BallbotInterface.h"
#include "ocs2_ballbot_example/definitions.h"
#include "ocs2_ballbot_example/ros_comm/BallbotDummyVisualization.h"

int main(int argc, char** argv) {
  const std::string robotName = "ballbot";

  // task file
  std::vector<std::string> programArgs{};
  ::ros::removeROSArgs(argc, argv, programArgs);
  if (programArgs.size() <= 1) {
    throw std::runtime_error("No task file specified. Aborting.");
  }
  std::string taskFileFolderName = std::string(programArgs[1]);

  // Initialize ros node
  ros::init(argc, argv, robotName + "_mrt");
  ros::NodeHandle nodeHandle;

  // ballbotInterface
  ocs2::ballbot::BallbotInterface ballbotInterface(taskFileFolderName);

  // MRT
  ocs2::MRT_ROS_Interface mrt(robotName);
  mrt.initRollout(&ballbotInterface.getRollout());
  mrt.launchNodes(nodeHandle);

  // Visualization
  std::shared_ptr<ocs2::ballbot::BallbotDummyVisualization> ballbotDummyVisualization(
      new ocs2::ballbot::BallbotDummyVisualization(nodeHandle));

  // Dummy ballbot
  ocs2::MRT_ROS_Dummy_Loop dummyBallbot(mrt, ballbotInterface.mpcSettings().mrtDesiredFrequency_,
                                        ballbotInterface.mpcSettings().mpcDesiredFrequency_);
  dummyBallbot.subscribeObservers({ballbotDummyVisualization});

  // initial state
  ocs2::SystemObservation initObservation;
  initObservation.state() = ballbotInterface.getInitialState();
  initObservation.input().setZero(ocs2::ballbot::INPUT_DIM_);
  initObservation.time() = 0;

  // initial command
  ocs2::CostDesiredTrajectories initCostDesiredTrajectories({initObservation.time()}, {initObservation.state()}, {initObservation.input()});

  // Run dummy (loops while ros is ok)
  dummyBallbot.run(initObservation, initCostDesiredTrajectories);

  // Successful exit
  return 0;
}
