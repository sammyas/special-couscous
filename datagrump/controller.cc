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

int prev_rtt;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), initialized( false ), curr_phase( STARTUP ),
    BtlBwFilter( 100, 10, true ), RTpropFilter( 1000000, 1, false ),
    max_rtt( 0 ), delivery_rate( 0 ), next_send_time( timestamp_ms() ),
    cwnd_gain( 2.885 ), curr_stage( 0 ), stage_start( 0 ),
    nslow_startup_rounds( 3 ), prev_btlbw( 0 ), current_delivered( 0 ),
    delivered_map()
{
}

double Controller::get_btlbw( void )
{
  double btlbw = BtlBwFilter.get_current();
  if ( btlbw < 5 / get_rtt())
    btlbw = 5 / get_rtt();
  return btlbw;
}

double Controller::get_rtt( void )
{
  double rtt = RTpropFilter.get_current();
  if ( rtt > 1e6 )
    rtt = 1;
  return rtt;
}

double Controller::get_pace_gain( void )
{
  if ( curr_phase == STARTUP )
    return startup_pacing;
  if ( curr_phase == DRAIN )
    return drain_pacing;
  return pacing_stages[curr_stage];
}

double Controller::get_cwnd_gain( void )
{
  if ( curr_phase == STARTUP )
    return startup_pacing;
  if ( curr_phase == DRAIN )
    return drain_pacing;
  return 2;
}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  double bdp = get_btlbw() * get_rtt();
  unsigned int window = get_cwnd_gain() * bdp;
  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << window << endl;
  }
  return window;
}

uint64_t Controller::get_next_send_time( void )
{
  return next_send_time;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  delivered_map[sequence_number] = current_delivered;
  next_send_time = send_timestamp + 1.0 / ( get_btlbw() * get_pace_gain());
  
  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
  }
}

void Controller::update_startup_phase( double delivery_rate )
{
  double btlbw_diff = delivery_rate - prev_btlbw;
  if ( btlbw_diff < 0.25 * prev_btlbw )
    nslow_startup_rounds++;
  else
    nslow_startup_rounds = 0;
  prev_btlbw = delivery_rate;
  if ( nslow_startup_rounds == 3 ) {
    cout << "phase change to DRAIN" << endl;
    curr_phase = DRAIN;
  }
}

void Controller::update_drain_phase( uint64_t inflight )
{
  double bdp = get_btlbw() * get_rtt();
  if ( inflight <= bdp ) {
    curr_phase = STEADY;
    cout << "phase change to STEADY" << endl;
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent ( sender's clock ) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received ( receiver's clock )*/
			       const uint64_t timestamp_ack_received,
                               /* when the ack was received ( by sender ) */
                               const uint64_t inflight )
                               /* Number of packets currently inflight */
{
  if ( sequence_number_acked >= current_delivered )
    current_delivered++;
  uint64_t rtt = timestamp_ack_received - send_timestamp_acked;
  RTpropFilter.update( timestamp_ack_received, ( double )rtt );

  delivery_rate = ( (( double ) current_delivered -
		    delivered_map[sequence_number_acked] )
		   / ( (double ) timestamp_ack_received - send_timestamp_acked ));
  delivered_map.erase( sequence_number_acked );
  BtlBwFilter.update( timestamp_ack_received, delivery_rate );

  if ( timestamp_ack_received - stage_start > get_rtt()) {
    stage_start = timestamp_ack_received;
    curr_stage = ( curr_stage + 1 ) % 8;

    if ( curr_phase == STARTUP )
      update_startup_phase( delivery_rate );
    if ( curr_phase == DRAIN )
      update_drain_phase( inflight );
  }


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
  return 1000;
}

void Controller::timed_out( void )
{
  cerr << "timed out" << endl;
}
