#include "filter.hh"
#include <iostream>
using namespace std;

WindowedFilter::WindowedFilter( uint64_t size_,
				int num_buckets_,
				bool max_ )
  : buckets( 0 ), size( size_ ), num_buckets( 0 ),
    first_bucket_time( 0 ), current( 0 ), defaultval( 0 ),
    max( false ) {
  size = size_;
  num_buckets = num_buckets_;
  first_bucket_time = 0;
  max = max_;
  if ( max )
    defaultval = 0;
  else
    defaultval = 1e10;
  current = defaultval;
  for ( int i = 0; i < num_buckets; i++ ) {
    buckets.push_back( defaultval );
  }
}

double WindowedFilter::get_current() {
  return current;
}

void WindowedFilter::update( uint64_t time, double value ) {
  if ( first_bucket_time == 0 ) {
    first_bucket_time = time;
    buckets[0] = value;
    current = value;
    return;
  }
  int bucket_idx =( int ) ( double( time - first_bucket_time ) /
		double( size ) * double( num_buckets ));
  if ( bucket_idx >= num_buckets ) {
    for ( int i = num_buckets; i <= bucket_idx; i++ ) {
      buckets.push_back( defaultval );
    }
    std::vector<double> newvec( buckets.end() - num_buckets,
			  buckets.end() );
    buckets = newvec;
    bucket_idx = num_buckets - 1;
  }
  buckets[bucket_idx] = WindowedFilter::updatefn( buckets[bucket_idx], value );
  current = WindowedFilter::selectfn();
}

double WindowedFilter::updatefn( double oval, double nval ) {
  if ( max ) {
    return oval > nval ? oval : nval;
  } else {
    return oval < nval ? oval : nval;
  }
}

double WindowedFilter::selectfn() {
  double val = defaultval;
  for ( int i = 0; i < num_buckets; i++ ) {
    val = WindowedFilter::updatefn( val, buckets[i] );
  }
  return val;
}
