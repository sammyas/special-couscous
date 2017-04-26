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

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), initialized ( false ), RTO ( 1000 ), SRTT (0), RTTVAR (0),
    alpha( 1/8.0 ), beta( 1/4.0 ), cwnd( 4 ), rtt_gain( 6.0 ), add_gain( 0.5 ),
    mult_decr( 1.5 )
{
  alpha = read_env("ALPHA", alpha);
  beta = read_env("BETA", beta);
  rtt_gain = read_env("RTT_GAIN", rtt_gain);
  add_gain = read_env("ADD_GAIN", add_gain);
  mult_decr = read_env("MULT_DECR", mult_decr);
}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
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

  uint64_t R = timestamp_ack_received - send_timestamp_acked;
  if (!initialized) {
    SRTT = R;
    RTTVAR = R;
    RTO = SRTT + rtt_gain * RTTVAR;
    initialized = true;
  }
  else {
    if (R >= RTO) {
      if ( debug_ ) {
	cerr << "timed out when the window size was " << cwnd << endl;
      }
      cwnd = cwnd / mult_decr;
    }
    else {
      cwnd += add_gain / cwnd;
    }
    uint64_t rtt_diff = (SRTT > R) ? SRTT - R : R - SRTT;
    RTTVAR = (1 - beta) * RTTVAR + beta * rtt_diff;
    SRTT = (1 - alpha) * SRTT + alpha * R;
    RTO = SRTT + rtt_gain * RTTVAR;
    if ( debug_ ) {
      cerr << "RTO is " << RTO << " SRTT is " << SRTT << " RTTVAR is " << RTTVAR
	   << " and our new rtt is " << R << " with window size " << cwnd << endl;
    }
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
  return RTO;
}

void Controller::timed_out( void )
{
}
