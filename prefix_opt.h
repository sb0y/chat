#ifndef PREFIX_OPT_H
#define PREFIX_OPT_H

#include <boost/program_options.hpp>

typedef std::map<std::string, std::vector<std::string>> p_options_type;
p_options_type get_p_options(const boost::program_options::variables_map& vm, const std::string prefix);

#endif
