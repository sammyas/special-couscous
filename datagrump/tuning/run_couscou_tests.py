#!/usr/bin/env python
import run_tests


def main():
    #updates = [('ALPHA', 'alpha', [0.05, 0.1, 0.2, 0.3, 0.5, 0.8]),
    #           ('WINDOW_SCALING', 'window_scale', [1.5, 2.0, 2.5])]
    #updates = [('ALPHA', 'alpha', [0.8, 0.9, 0.95]),
    #           ('WINDOW_SCALING', 'window_scale', [1.25, 1.5])],
    updates = [('ALPHA', 'alpha', [0.8, 0.9]),
               ('WINDOW_SCALING', 'window_scale', [1.5, 1.75]),
               ('TIMEOUT', 'timeout', [50, 75])]
    stat_file = open('couscou-{}.csv'.format('tuning'), 'w')
    out_file = open('couscou-{}.txt'.format('tuning'), 'w')
    run_tests.run_with_multi_envs('couscou', updates, stat_file, out_file)
    out_file.close()
    stat_file.close()


if __name__ in '__main__':
    main()
