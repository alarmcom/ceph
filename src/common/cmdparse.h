// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
#ifndef CEPH_COMMON_CMDPARSE_H
#define CEPH_COMMON_CMDPARSE_H

#include <string>
#include <sstream>
#include <map>
#include <boost/variant.hpp>
#include <vector>
#include <stdexcept>
#include "common/dout.h"
#include "common/Formatter.h"

/* this is handy; can't believe it's not standard */
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(*a))

typedef boost::variant<std::string, bool, int64_t, double, std::vector<std::string> > cmd_vartype;

void dump_cmds_to_json(Formatter *f, const char *cmds);
bool cmdmap_from_json(std::vector<std::string> cmd, std::map<std::string,
		      cmd_vartype> *mapp, std::stringstream &ss);

template <typename T>
bool
cmd_getval(CephContext *cct, std::map<std::string, cmd_vartype>& cmdmap, std::string k, T& val)
{
  // referencing a nonexistent key creates it, even as an rvalue;
  // we don't want that behavior for get.
  if (cmdmap.count(k)) {
    try {
      val = boost::get<T>(cmdmap[k]);
      return true;
    } catch (boost::bad_get) { 
      std::ostringstream errstr;
      errstr << "bad boost::get: key " << k << " is not type " << typeid(T).name();
      lderr(cct) << errstr.str() << dendl;
    }
  }
  // either not found or bad type, return false
  return false;
}

// with default
template <typename T>
void
cmd_getval(CephContext *cct, std::map<std::string, cmd_vartype>& cmdmap, std::string k, T& val,
       T defval)
{
  if (!cmd_getval(cct, cmdmap, k, val))
    val = defval;
}

template <typename T>
void
cmd_putval(CephContext *cct, std::map<std::string, cmd_vartype>& cmdmap, std::string k, T val)
{
  cmdmap[k] = val;
}

#endif