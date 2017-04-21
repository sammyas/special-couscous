#ifndef FILTER_HH
#define FILTER_HH

#include <vector>
#include <cstdint>

class WindowedFilter
{
private:
  std::vector<double> buckets;
  uint64_t size;
  int num_buckets;
  uint64_t first_bucket_time;
  double current;
  double defaultval;
  bool max;
public:
  WindowedFilter( uint64_t size, int num_buckets, bool max );

  double get_current( void );
  void update( uint64_t time, double value );
  double updatefn( double oval, double nval );
  double selectfn( void );
};

#endif
