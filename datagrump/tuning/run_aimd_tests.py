#!/usr/bin/env python
import run_tests


def main():
#    updates = [('ALPHA', 'alpha', [1/16.0, 1/8.0, 1/4.0]),
#               ('BETA', 'beta', [1/8.0, 1/4.0, 1/2.0]),
#               ('RTT_GAIN', 'rtt_gain', [2.0, 4.0, 8.0]),
#               ('ADD_GAIN', 'add_gain', [1, 2, 3]),
#               ('MULT_DECR', 'mult_decr', [1.5, 2.0, 2.5])]
#    for update in updates:
#        stat_file = open('aimd-{}.csv'.format(update[1]), 'w')
#        out_file = open('aimd-{}.txt'.format(update[1]), 'w')
#        run_tests.run_with_env('bg', update, 1, stat_file, out_file)
#        out_file.close()
#        stat_file.close()

    updates = [('ALPHA', 'alpha', [1/8.0]),
               ('BETA', 'beta', [1/4.0]),
               ('RTT_GAIN', 'rtt_gain', [4.0, 6.0]),
               ('ADD_GAIN', 'add_gain', [0.5, 1]),
               ('MULT_DECR', 'mult_decr', [1.125, 1.25, 1.5])]

    stat_file = open('aimd-{}.csv'.format('tuning'), 'w')
    out_file = open('aimd-{}.txt'.format('tuning'), 'w')
    run_tests.run_with_multi_envs('bg', updates, stat_file, out_file)
    out_file.close()
    stat_file.close()


if __name__ in '__main__':
    main()
