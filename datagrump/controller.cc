#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), initialized ( false ), prev_rtt(0), min_rtt(100), rtt_diff(0), T_low(80), T_high (200), nlow_cycles (0),
    alpha( 1/8.0 ), beta( 0.8 ), cwnd (10)
{
}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */

  //unsigned int the_window_size = 50;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << cwnd << endl;
  }
  return cwnd;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  /* Default: take no action */

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
  }
}
bool tmp;
/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  double rtt = timestamp_ack_received - send_timestamp_acked;
  if (rtt < min_rtt)
    min_rtt = rtt;

  if (!initialized)
  { 
    prev_rtt = rtt;
    initialized = true;
    return;
  }

  double new_rtt_diff = rtt - prev_rtt;
  prev_rtt = rtt;

  if (rtt_diff == 0)
    rtt_diff = new_rtt_diff;
  else
    rtt_diff = (1 - alpha) * rtt_diff + alpha * new_rtt_diff;

  double gradient = rtt_diff / T_low;

  cerr << "rtt is " << rtt << " and gradient is " << gradient << " and cwnd is " << cwnd << endl;
    if (rtt >= T_high) {
      if (cwnd >= 10)
        cwnd = cwnd/2;
      nlow_cycles = 0;
      return;
    }
    if (rtt <= T_low) {
      cwnd += 5/cwnd;
      nlow_cycles = 0;
      return;
    }
    if (gradient <= 0) {
      nlow_cycles++;
      if (nlow_cycles >= 5)
        cwnd = cwnd + 10/cwnd;
      else
        cwnd = cwnd + 5/cwnd;
    }
    else {
      nlow_cycles = 0;
      cwnd = cwnd * (1 - beta * gradient);
    }

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}

void Controller::timed_out( void )
{
  
}
