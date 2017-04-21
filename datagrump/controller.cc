#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), initialized ( false ), RTO ( 1000 ), SRTT (0), RTTVAR (0),
    alpha( 1/8.0 ), beta( 1/4.0 ), cwnd (50)
{
  /*
  char *alpha_env = getenv("ALPHA");
  if (alpha_env != NULL) {
    double env_alpha = strtol(window_env, NULL, 10);
    if (env_alpha)
      alpha = env_alpha
  }
  cout << "Set alpha to " << alpha << endl;
  */
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
    RTO = SRTT + 4 * RTTVAR;
    initialized = true;
  }
  else {
    if (R >= RTO) {
      cerr << "timed out when the window size was " << cwnd << endl;
      cwnd = cwnd/2;
    }
    else {
      cwnd += 1/cwnd;
    }
    uint64_t rtt_diff = (SRTT > R) ? SRTT - R : R - SRTT;
    RTTVAR = (1 - beta) * RTTVAR + beta * rtt_diff;
    SRTT = (1 - alpha) * SRTT + alpha * R;
    RTO = SRTT + 4 * RTTVAR;
    cerr << "RTO is " << RTO << " SRTT is " << SRTT << " RTTVAR is " << RTTVAR
      << " and our new rtt is " << R << " with window size " << cwnd << endl;
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
