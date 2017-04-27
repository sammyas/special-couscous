#!/usr/bin/env python
import run_tests


def main():
    stat_file = open('delay_thresh.csv', 'w')
    out_file = open('delay_thresh.txt', 'w')
    run_tests.run_with_envs('bg', ('DELAY', 'delay',
                                   [40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260]),
                            1, stat_file, out_file)
    out_file.close()
    stat_file.close()


if __name__ in '__main__':
    main()
