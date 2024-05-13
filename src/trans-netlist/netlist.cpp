/*******************************************************************\

Module: Graph representing Netlist

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <ctype.h>

#include <solvers/flattening/boolbv_width.h>

#include "netlist.h"

/*******************************************************************\

Function: netlistt::print

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void netlistt::print(std::ostream &out) const
{
  var_map.output(out);

  out << '\n';
  out << "Next state functions:" << '\n';

  for(var_mapt::mapt::const_iterator
      it=var_map.map.begin();
      it!=var_map.map.end(); it++)
  {
    const var_mapt::vart &var=it->second;

    for(unsigned i=0; i<var.bits.size(); i++)
    {
      if(var.is_latch())
      {
        out << "  NEXT(" << it->first;
        if(var.bits.size()!=1) out << "[" << i << "]";
        out << ")=";

        print(out, var.bits[i].next);

        out << '\n';
      }
    }
  }

  out << '\n';

  out << "Initial state: " << '\n';

  for(auto &c : initial)
  {
    out << "  ";
    print(out, c);
    out << '\n';
  }

  out << '\n';

  out << "State invariant constraints: " << '\n';

  for(auto &c : constraints)
  {
    out << "  ";
    print(out, c);
    out << '\n';
  }

  out << '\n' << std::flush;

  out << "Transition constraints: " << '\n';

  for(auto &c : transition)
  {
    out << "  ";
    print(out, c);
    out << '\n';
  }
  
  out << '\n' << std::flush;
}

/*******************************************************************\

Function: dot_id

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

static std::string dot_id(const std::string &id)
{
  std::string in=id;

  std::string::size_type pos;

  pos=in.rfind("::");

  if(pos!=std::string::npos)
    in=std::string(in, pos+2, std::string::npos);

  pos=in.rfind(".");

  if(pos!=std::string::npos)
    in=std::string(in, pos+1, std::string::npos);

  std::string result;

  result.reserve(in.size());

  for(unsigned i=0; i<in.size(); i++)
  {
    char ch=in[i];
    if(isalnum(ch) || ch=='(' || ch==')' || ch==' ' || ch=='.')
      result+=ch;
    else
      result+='_';
  }

  return result;
}

/*******************************************************************\

Function: netlistt::label

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::string netlistt::label(unsigned v) const
{
  PRECONDITION(v < number_of_nodes());
  PRECONDITION(nodes[v].is_var());
  const bv_varidt &varid=var_map.reverse(v);
  const var_mapt::mapt::const_iterator v_it=var_map.map.find(varid.id);
  if(v_it!=var_map.map.end() && v_it->second.bits.size()!=1)
    return id2string(varid.id)+'['+std::to_string(varid.bit_nr)+']';
  else
    return id2string(varid.id);
}

/*******************************************************************\

Function: netlistt::dot_label

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::string netlistt::dot_label(unsigned v) const
{
  return dot_id(label(v));
}

/*******************************************************************\

Function: netlistt::output_dot

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void netlistt::output_dot(std::ostream &out) const
{
  aigt::output_dot(out);

  // add the sinks
  for(var_mapt::mapt::const_iterator
      it=var_map.map.begin();
      it!=var_map.map.end();
      it++)
  {
    const var_mapt::vart &var=it->second;

    if(var.is_latch())
    {
      DATA_INVARIANT(var.bits.size() == 1, "");
      unsigned v=var.bits.front().current.var_no();
      literalt next=var.bits.front().next;

      out << "next" << v << " [shape=box,label=\""
          << dot_id(id2string(it->first)) << "'\"]" << '\n';

      if(next.is_constant())
        out << "TRUE";
      else
        out << next.var_no();

      out << " -> next" << v;
      if(next.sign()) out << " [arrowhead=odiamond]";
      out << '\n';
    }
  }
}

/*******************************************************************\

Function: netlistt::output_smv

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void netlistt::output_smv(std::ostream &out) const
{
  out << "MODULE main" << '\n';

  out << '\n';
  out << "-- Variables" << '\n';
  out << '\n';

  for(var_mapt::mapt::const_iterator
      it=var_map.map.begin();
      it!=var_map.map.end();
      it++)
  {
    const var_mapt::vart &var=it->second;

    for(unsigned i=0; i<var.bits.size(); i++)
    {
      if(var.is_latch())
      {
        out << "VAR " << id2smv(it->first);
        if(var.bits.size()!=1) out << "[" << i << "]";
        out << ": boolean;" << '\n';
      }
    }
  }

  out << '\n';
  out << "-- Inputs" << '\n';
  out << '\n';

  for(var_mapt::mapt::const_iterator
      it=var_map.map.begin();
      it!=var_map.map.end();
      it++)
  {
    const var_mapt::vart &var=it->second;

    for(unsigned i=0; i<var.bits.size(); i++)
    {
      if(var.is_input())
      {
        out << "VAR " << id2smv(it->first);
        if(var.bits.size()!=1) out << "[" << i << "]";
        out << ": boolean;" << '\n';
      }
    }
  }

  out << '\n';
  out << "-- AND Nodes" << '\n';
  out << '\n';

  for(unsigned node_nr=0; node_nr<nodes.size(); node_nr++)
  {
    const aig_nodet &node=nodes[node_nr];

    if(node.is_and())
    {
      out << "DEFINE node" << node_nr << ":=";
      print_smv(out, node.a);
      out << " & ";
      print_smv(out, node.b);
      out << ";" << '\n';
    }
  }

  out << '\n';
  out << "-- Next state functions" << '\n';
  out << '\n';

  for(var_mapt::mapt::const_iterator
      it=var_map.map.begin();
      it!=var_map.map.end();
      it++)
  {
    const var_mapt::vart &var=it->second;

    for(unsigned i=0; i<var.bits.size(); i++)
    {
      if(var.is_latch())
      {
        out << "ASSIGN next(" << id2smv(it->first);
        if(var.bits.size()!=1) out << "[" << i << "]";
        out << "):=";
        print_smv(out, var.bits[i].next);
        out << ";" << '\n';
      }
    }
  }

  out << '\n';
  out << "-- Initial state" << '\n';
  out << '\n';

  for(auto &initial_l : initial)
  {
    if(!initial_l.is_true())
    {
      out << "INIT ";
      print_smv(out, initial_l);
      out << '\n';
    }
  }

  out << '\n';
  out << "-- TRANS" << '\n';
  out << '\n';

  for(auto &trans_l : transition)
  {
    if(!trans_l.is_true())
    {
      out << "TRANS ";
      print_smv(out, trans_l);
      out << '\n';
    }
  }

  out << '\n';
  out << "-- Properties" << '\n';
  out << '\n';

  for(auto &[id, property] : properties)
  {
    if(std::holds_alternative<Gpt>(property))
    {
      out << "-- " << id << '\n';
      out << "LTLSPEC G ";
      print_smv(out, std::get<Gpt>(property).p);
      out << '\n';
    }
    else if(std::holds_alternative<GFpt>(property))
    {
      out << "-- " << id << '\n';
      out << "LTLSPEC G F ";
      print_smv(out, std::get<GFpt>(property).p);
      out << '\n';
    }
  }
}

/*******************************************************************\

Function: netlistt::id2smv

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::string netlistt::id2smv(const irep_idt &id)
{
  std::string result;
  
  for(unsigned i=0; i<id.size(); i++)
  {
    const bool first = i == 0;
    char ch=id[i];
    if(
      isalnum(ch) || ch == '_' || (ch == '.' && !first) ||
      (ch == '#' && !first) || (ch == '-' && !first))
    {
      result+=ch;
    }
    else if(ch==':')
    {
      result+='.';
      if((i-1)<id.size() && id[i+1]==':') i++;
    }
    else
    {
      result+='$';
      result+=std::to_string(ch);
      result+='$';
    }
  }
  
  return result;
}

/*******************************************************************\

Function: netlistt::print_smv

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void netlistt::print_smv(
  std::ostream& out,
  literalt a) const
{
  if(a==const_literal(false))
  {
    out << "0";
    return;
  }
  else if(a==const_literal(true))
  {
    out << "1";
    return;
  }

  unsigned node_nr=a.var_no();
  DATA_INVARIANT(node_nr < number_of_nodes(), "node_nr in range");

  if(a.sign()) out << "!";

  if(nodes[node_nr].is_and())
    out << "node" << node_nr;
  else if(nodes[node_nr].is_var())
  {
    const bv_varidt &varid=var_map.reverse(node_nr);
    out << id2smv(varid.id);
    const var_mapt::mapt::const_iterator v_it=var_map.map.find(varid.id);
    if(v_it!=var_map.map.end() && v_it->second.bits.size()!=1)
      out << '[' << varid.bit_nr << ']';
  }
  else
    out << "unknown";
}

