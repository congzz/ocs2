/*
 * SwitchedModelCostBase.h
 *
 *  Created on: Nov 23, 2017
 *      Author: farbod
 */

#include "ocs2_switched_model_interface/cost/SwitchedModelCostBase.h"

#include "ocs2_switched_model_interface/core/Rotations.h"

namespace switched_model {

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
SwitchedModelCostBase::SwitchedModelCostBase(const com_model_t& comModel, const SwitchedModelModeScheduleManager& modeScheduleManager,
                                             const state_matrix_t& Q, const input_matrix_t& R)
    : ocs2::QuadraticCostFunction(Q, R, state_matrix_t::Zero()),
      comModelPtr_(comModel.clone()),
      modeScheduleManagerPtr_(&modeScheduleManager) {}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
SwitchedModelCostBase::SwitchedModelCostBase(const SwitchedModelCostBase& rhs)
    : ocs2::QuadraticCostFunction(rhs), comModelPtr_(rhs.comModelPtr_->clone()), modeScheduleManagerPtr_(rhs.modeScheduleManagerPtr_) {}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/

SwitchedModelCostBase* SwitchedModelCostBase::clone() const {
  return new SwitchedModelCostBase(*this);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
scalar_t SwitchedModelCostBase::cost(scalar_t t, const vector_t& x, const vector_t& u) {
  if (costDesiredTrajectoriesPtr_ == nullptr) {
    throw std::runtime_error("[SwitchedModelCostBase] costDesiredTrajectoriesPtr_ is not set");
  }

  // Get stance configuration
  const auto contactFlags = modeScheduleManagerPtr_->getContactFlags(t);

  const vector_t xNominal = costDesiredTrajectoriesPtr_->getDesiredState(t);
  vector_t uNominal = costDesiredTrajectoriesPtr_->getDesiredInput(t);
  // If the input has non-zero values, don't overwrite it.
  // TODO (rgrandia) : implement a better way to switch between heuristic inputs and tracking user defined inputs.
  if (uNominal.isZero()) {
    uNominal = weightCompensatingInputs(*comModelPtr_, contactFlags, getOrientation(getComPose<scalar_t>(xNominal)));
  }

  const vector_t xDeviation = x - xNominal;
  const vector_t uDeviation = u - uNominal;
  return 0.5 * xDeviation.dot(Q_ * xDeviation) + 0.5 * uDeviation.dot(R_ * uDeviation);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
ScalarFunctionQuadraticApproximation SwitchedModelCostBase::costQuadraticApproximation(scalar_t t, const vector_t& x, const vector_t& u) {
  if (costDesiredTrajectoriesPtr_ == nullptr) {
    throw std::runtime_error("[SwitchedModelCostBase] costDesiredTrajectoriesPtr_ is not set");
  }

  // Get stance configuration
  const auto contactFlags = modeScheduleManagerPtr_->getContactFlags(t);

  const vector_t xNominal = costDesiredTrajectoriesPtr_->getDesiredState(t);
  vector_t uNominal = costDesiredTrajectoriesPtr_->getDesiredInput(t);
  // If the input has non-zero values, don't overwrite it.
  // TODO (rgrandia) : implement a better way to switch between heuristic inputs and tracking user defined inputs.
  if (uNominal.isZero()) {
    uNominal = weightCompensatingInputs(*comModelPtr_, contactFlags, getOrientation(getComPose<scalar_t>(xNominal)));
  }

  const vector_t xDeviation = x - xNominal;
  const vector_t uDeviation = u - uNominal;
  const vector_t qDeviation = Q_ * xDeviation;
  const vector_t rDeviation = R_ * uDeviation;

  ScalarFunctionQuadraticApproximation L;
  L.f = 0.5 * xDeviation.dot(qDeviation) + 0.5 * uDeviation.dot(rDeviation);
  L.dfdx = qDeviation;
  L.dfdu = rDeviation;
  L.dfdxx = Q_;
  L.dfdux.setZero(u.rows(), x.rows());
  L.dfduu = R_;
  return L;
}

}  // namespace switched_model
