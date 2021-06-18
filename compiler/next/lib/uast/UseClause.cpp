/*
 * Copyright 2021 Hewlett Packard Enterprise Development LP
 * Other additional copyright holders may be indicated within.
 *
 * The entirety of this work is licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "chpl/uast/UseClause.h"

#include "chpl/uast/Builder.h"

namespace chpl {
namespace uast {


owned<UseClause> UseClause::build(Builder* builder, Location loc,
                                  owned<Expression> name,
                                  LimitationClauseKind limitationClauseKind,
                                  ASTList limitationClause) {
  assert(name.get() != nullptr);

  ASTList lst;
  int8_t limitationClauseChildNum = -1;
  int numLimitations = 0;

  lst.push_back(std::move(name));

  if (limitationClause.size()) {
    limitationClauseChildNum = lst.size();
    numLimitations = limitationClause.size();
    for (auto& ast : limitationClause) {
      lst.push_back(std::move(ast));
    }
  }

  UseClause* ret = new UseClause(std::move(lst), limitationClauseKind,
                                 limitationClauseChildNum,
                                 numLimitations);
  builder->noteLocation(ret, loc);
  return toOwned(ret);
}

owned<UseClause> UseClause::build(Builder* builder, Location loc,
                                  owned<Expression> name) {
  return build(builder, loc, std::move(name), UseClause::NONE, ASTList());
}

} // namespace uast
} // namespace chpl
