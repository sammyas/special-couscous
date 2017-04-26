#!/usr/bin/env python
import run_tests


def main():
    stat_file = open('window_size.csv', 'w')
    out_file = open('window_size.txt', 'w')
    run_tests.run_with_envs('bg', ('WINDOW_SIZE', 'window',
                                   [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]),
                            1, stat_file, out_file)
    out_file.close()
    stat_file.close()


if __name__ in '__main__':
    main()
