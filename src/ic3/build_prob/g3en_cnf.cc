/******************************************************

Module: CNF Generation (Part 4)

Author: Eugene Goldberg, eu.goldberg@gmail.com

******************************************************/
#include <util/invariant.h>

#include "ccircuit.hh"
#include "dnf_io.hh"
#include "m0ic3.hh"

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>

#include "minisat/core/Solver.h"
#include "minisat/simp/SimpSolver.h"

/*=======================================

     M A R K _ C O N S T R _ G A T E S

  ======================================*/
void CompInfo::mark_constr_gates(CUBE &Gates,bool tran_flag,bool fun_flag)
{
  for (size_t i=0; i < Gates.size(); i++) {
    int gate_ind = Gates[i];
    Gate &G = N->get_gate(gate_ind);
    if (tran_flag) G.flags.tran_constr = 1;
    if (fun_flag) G.flags.fun_constr = 1;
  }

} /* end of function mark_constr_gates */

/*==============================================

          G E N _ C O N S T R _ C O I

  ===============================================*/
void CompInfo::gen_constr_coi(CUBE &Gates,bool &tran_flag,bool &fun_flag,
                             CUBE &Stack)
{

  tran_flag = false;
  fun_flag = false;

  assert(Stack.size() == 1);
  Gate &G = N->get_gate(Stack.back());
  INVARIANT(G.flags.label == 0, "Gate label should be zero.");

  CUBE Labelled;

  while (Stack.size() > 0) {
    int gate_ind = Stack.back();
    Stack.pop_back();
    Gate &G = N->get_gate(gate_ind);
    if (G.flags.label > 0) continue;
    
    G.flags.label = 1;
    Labelled.push_back(gate_ind);

    bool skip = false;
    if (G.flags.output_function > 0) {
      fun_flag = true;
      skip = true;
    }

    if (G.flags.transition > 0) 
      tran_flag = true;
     
    if (skip) continue;
    Gates.push_back(gate_ind);    

    // add fanin gates to the stack
    for (size_t i=0; i < G.Fanin_list.size(); i++) 
      Stack.push_back(G.Fanin_list[i]);
  }

  // remove label of Labelled
  for (size_t i=0; i < Labelled.size(); i++) {
    Gate &G = N->get_gate(Labelled[i]);
    G.flags.label = 0;
  }

} /* end of function gen_constr_coi */
