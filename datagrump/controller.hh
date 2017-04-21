#ifndef CONTROLLER_HH
#define CONTROLLER_HH

#include <cstdint>
#include <map>

#include "filter.hh"
enum Phase {STARTUP, DRAIN, STEADY};

const double pacing_stages[8] = {5.0/4, 3.0/4, 1, 1, 5.0/4, 3.0/4, 1, 1};
const double startup_pacing = 2.88;
const double drain_pacing = 0.347;
/* Congestion controller interface */

class Controller
{
private:
  bool debug_; /* Enables debugging output */
  bool initialized;

  Phase curr_phase;

  WindowedFilter BtlBwFilter;
  WindowedFilter RTpropFilter;
  double max_rtt;
  double delivery_rate;
  uint64_t next_send_time;

  double cwnd_gain;
  int curr_stage;
  uint64_t stage_start;

  int nslow_startup_rounds;

  double prev_btlbw;
  uint64_t current_delivered;
  std::map<uint64_t, uint64_t> delivered_map;

  void update_startup_phase (double delivery_rate);
  void update_drain_phase (uint64_t delivery_rate);
  double get_cwnd_gain( void );
  double get_pace_gain( void );
  double get_btlbw ( void );

public:
  /* Public interface for the congestion controller */
  /* You can change these if you prefer, but will need to change
     the call site as well (in sender.cc) */

  /* Default constructor */
  Controller( const bool debug );

  /* Get current window size, in datagrams */
  unsigned int window_size( void );

  uint64_t get_next_send_time( void );
  double get_rtt ( void );

  /* A datagram was sent */
  void datagram_was_sent( const uint64_t sequence_number,
			  const uint64_t send_timestamp );

  /* An ack was received */
  void ack_received( const uint64_t sequence_number_acked,
		     const uint64_t send_timestamp_acked,
		     const uint64_t recv_timestamp_acked,
		     const uint64_t timestamp_ack_received,
                     const uint64_t inflight_packets);

  /* How long to wait (in milliseconds) if there are no acks
     before sending one more datagram */
  unsigned int timeout_ms( void );

  void timed_out( void );
};

#endif
