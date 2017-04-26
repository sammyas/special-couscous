#!/usr/bin/env python
import os
import signal
import subprocess


def process_url(url):
    if not url:
        return ''
    return 'http://' + url.strip()


STATS_MARKERS = [
    ('capacity', 'Average capacity: ', ' Mbits/s', float),
    ('throughput', 'Average throughput: ', ' Mbits/s', float),
    ('queue_delay', '95th percentile per-packet queueing delay: ', ' ms', int),
    ('signal_delay', '95th percentile signal delay: ', ' ms', int),
    ('url', 'http://', '\n', process_url),
]


def run_with_env(username, env):
    """ Returns output """
    lines = []
    process = subprocess.Popen(
        ["./run-contest {}".format(username)],
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        preexec_fn=os.setsid,
        env=env
    )
    def read_output():
        try:
            for line in process.stdout:
                lines.append(line)
        except KeyboardInterrupt:
            try:
                pid = int(subprocess.check_output(["pidof", "sender"]))
                os.kill(pid, signal.SIGINT)
                print "Killing sender process"
            except subprocess.CalledProcessError:
                os.killpg(os.getpgid(process.pid), signal.SIGINT)
                print "Killing child process group"
            except ValueError:
                os.killpg(os.getpgid(process.pid), signal.SIGINT)
                print "Killing child process group"
            read_output()
        except IOError:
            pass    
    read_output()
    return ''.join(lines)


def process_output(stats_markers, output):
    """ stats_markers is [(key, start, end, parse)]. Returns {key: val} """
    result = {}
    for stat in stats_markers:
        key, start, end, parse = stat
        start_idx = output.find(start)
        end_idx = output.find(end, start_idx)
        try:
            value = parse(output[start_idx + len(start):end_idx])
        except ValueError:
            value = parse()
        result[key] = value
    return result


def run_with_envs(username, updates, repeat_times, stat_file, out_file):
    """ Writes stats as csv """
    base_env = os.environ
    env_field, key, values = updates
    stat_file.write("{},{},{},{},{},{}\n".format(
        key, 'capacity', 'throughput',
        'queue_delay', 'signal_delay',
        'url'
    ))

    for value in values:
        for time in xrange(repeat_times):
            print "Running with", key, "=", value
            env = base_env.copy()
            env[env_field] = str(value)
            output = run_with_env(username, env)
            stats = process_output(STATS_MARKERS, output)
            stat_file.write("{},{},{},{},{},{}\n".format(
                value, stats['capacity'], stats['throughput'],
                stats['queue_delay'], stats['signal_delay'],
                stats['url']
            ))
            out_file.write("###############\n{}\n".format(output))
            stat_file.flush()
            out_file.flush()
