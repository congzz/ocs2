/******************************************************************************
Copyright (c) 2020, Ruben Grandia. All rights reserved.

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

#include <ocs2_core/loopshaping/cost/LoopshapingCostInputPattern.h>

namespace ocs2 {

ScalarFunctionQuadraticApproximation LoopshapingCostInputPattern::costQuadraticApproximation(scalar_t t, const vector_t& x,
                                                                                             const vector_t& u) {
  const scalar_t gamma = loopshapingDefinition_->gamma_;
  const auto& s_filter = loopshapingDefinition_->getInputFilter();
  const vector_t x_system = loopshapingDefinition_->getSystemState(x);
  const vector_t u_system = loopshapingDefinition_->getSystemInput(x, u);
  const vector_t x_filter = loopshapingDefinition_->getFilterState(x);
  const vector_t u_filter = loopshapingDefinition_->getFilteredInput(x, u);
  const auto& L_system = systemCost_->costQuadraticApproximation(t, x_system, u_system);
  const auto& L_filter = systemCost_->costQuadraticApproximation(t, x_system, u_filter);

  ScalarFunctionQuadraticApproximation L;
  L.f = gamma * L_filter.f + (1.0 - gamma) * L_system.f;

  L.dfdx.resize(x.rows());
  L.dfdx.head(x_system.rows()) = gamma * L_filter.dfdx + (1.0 - gamma) * L_system.dfdx;
  L.dfdx.tail(x_filter.rows()).setZero();

  L.dfdxx.setZero(x.rows(), x.rows());
  L.dfdxx.topLeftCorner(x_system.rows(), x_system.rows()) = gamma * L_filter.dfdxx + (1.0 - gamma) * L_system.dfdxx;

  L.dfdu.resize(u_system.rows() + u_filter.rows());
  L.dfdu.head(u_system.rows()) = (1.0 - gamma) * L_system.dfdu;
  L.dfdu.tail(u_filter.rows()) = gamma * L_filter.dfdu;

  L.dfduu.resize(u.rows(), u.rows());
  L.dfduu.topLeftCorner(u_system.rows(), u_system.rows()) = (1.0 - gamma) * L_system.dfduu;
  L.dfduu.topRightCorner(u_system.rows(), u_filter.rows()).setZero();
  L.dfduu.bottomLeftCorner(u_filter.rows(), u_system.rows()).setZero();
  L.dfduu.bottomRightCorner(u_filter.rows(), u_filter.rows()) = gamma * L_filter.dfduu;

  L.dfdux.resize(u.rows(), x.rows());
  L.dfdux.topLeftCorner(u_system.rows(), x_system.rows()) = (1.0 - gamma) * L_system.dfdux;
  L.dfdux.topRightCorner(u_system.rows(), x_filter.rows()).setZero();
  L.dfdux.bottomLeftCorner(u_filter.rows(), x_system.rows()) = gamma * L_filter.dfdux;
  L.dfdux.bottomRightCorner(u_filter.rows(), x_filter.rows()).setZero();

  return L;
}

}  // namespace ocs2
