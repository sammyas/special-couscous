#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

double read_env( const char *name, double def )
{
  double result = def;
  char *val_env = getenv( name );
  if (val_env != NULL) {
    double value = strtod( val_env, NULL );
    if (value > 0.0)
      result = value;
  }
  return result;
}

const double window_scaling = read_env("WINDOW_SCALING", 1.75);
const double timeout = read_env("TIMEOUT", 50);

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), average_delivery_rate ( 0 ), alpha ( 0.9 ), min_rtt( 0 ),
    current_delivered( 0 ), delivered_map()
{
  alpha = read_env("ALPHA", alpha);
}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{

  unsigned int window = average_delivery_rate * min_rtt * window_scaling;

  /* Initialize our window size to 10 */
  if ( window == 0 )
    window = 10;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << window << endl;
  }
  return window;

}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  delivered_map[sequence_number] = current_delivered;
  
  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent ( sender's clock ) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received ( receiver's clock )*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received ( by sender ) */
{

  if ( sequence_number_acked > current_delivered )
    current_delivered++;

  uint64_t rtt = timestamp_ack_received - send_timestamp_acked;
  if ( rtt < min_rtt || min_rtt == 0 )
    min_rtt = rtt;

  /* Calculate average delivery rate as an ewma. */
  double delivery_rate = ( ( double ) current_delivered - delivered_map[sequence_number_acked] )
                         / ( ( double )rtt );
  if ( average_delivery_rate == 0 )
    average_delivery_rate = delivery_rate;
  else
    average_delivery_rate = average_delivery_rate * ( 1 - alpha ) + alpha * delivery_rate;


  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " ( send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock )"
	 << endl;
  }
}

/* How long to wait ( in milliseconds ) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return timeout;
}

void Controller::timed_out( void )
{
  cerr << "timed out" << endl;
}
