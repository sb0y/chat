#include "prefix_opt.h"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

p_options_type get_p_options(
  const boost::program_options::variables_map& vm,
  const std::string prefix)
{
  p_options_type p_options;

  const std::size_t prefix_size = prefix.size();

  boost::iterator_range<std::string::const_iterator> range;
  namespace po = boost::program_options;
  BOOST_FOREACH(const po::variables_map::value_type& pair, vm)
  {
    const std::string& key = pair.first;

    // Key is too small to contain prefix and a value, continue to next.
    if (key.substr(0, prefix.size()) != prefix) continue;

    range = boost::make_iterator_range(key.begin() + prefix_size, key.end());

    // Create pair and insert into map.
    p_options.insert(
      std::make_pair(
        boost::copy_range<std::string>(range),
        pair.second.as<std::vector<std::string>>())); 
  }
  return p_options;
}
