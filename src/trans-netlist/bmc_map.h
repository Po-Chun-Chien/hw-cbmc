/*******************************************************************\

Module: Variable Mapping

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#ifndef CPROVER_TRANS_BMC_MAP_H
#define CPROVER_TRANS_BMC_MAP_H

#include <assert.h>

#include "netlist.h"

class bmc_mapt
{
public:
  // number of valid timeframes
  // this is number of cycles +1!
  bmc_mapt(const netlistt &, std::size_t no_timeframes, propt &);

  inline literalt
  get(std::size_t timeframe, const var_mapt::vart::bitt &bit) const
  {
    literalt l=bit.current;
    if(l.is_constant()) return l;
    return get(timeframe, l.var_no())^l.sign();
  }

  inline literalt
  get(std::size_t timeframe, const irep_idt &id, unsigned bit_nr) const
  {
    literalt l=var_map.get_current(id, bit_nr);
    if(l.is_constant()) return l;
    return get(timeframe, l.var_no())^l.sign();
  }

  // translate netlist variable to solver literal
  inline literalt get(std::size_t timeframe, unsigned var_no) const
  {
    assert(timeframe<timeframe_map.size());
    assert(var_no<timeframe_map[timeframe].size());
    return timeframe_map[timeframe][var_no].solver_literal;
  }

  // translate netlist literal to solver literal
  inline literalt translate(std::size_t timeframe, literalt l) const
  {
    if(l.is_constant()) return l;
    return get(timeframe, l.var_no())^l.sign();
  }

  // set the solver literal for a netlist variable
  void set(std::size_t timeframe, unsigned var_no, literalt l)
  {
    assert(timeframe<timeframe_map.size());
    assert(var_no<timeframe_map[timeframe].size());
    timeframe_map[timeframe][var_no].solver_literal=l;
  }

  var_mapt var_map;

  struct nodet
  {
    literalt solver_literal;
  };

  typedef std::vector<nodet> timeframet;
  typedef std::vector<timeframet> timeframe_mapt;
  timeframe_mapt timeframe_map;

  struct reverse_entryt
  {
    // this is the netlist literal
    literalt netlist_literal;
    std::size_t timeframe;
  };

  typedef std::map<literalt, reverse_entryt> reverse_mapt;
  reverse_mapt reverse_map;

  std::size_t get_no_timeframes() const
  {
    return timeframe_map.size();
  }
   
  bmc_mapt() { }
  
  virtual ~bmc_mapt()
  {
  }
  
  void clear()
  {
    timeframe_map.clear();
    reverse_map.clear();
  }
};

#endif
