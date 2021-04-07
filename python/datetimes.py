#!/usr/bin/python
# @author Vsevolod Ivanov

import pytz
import datetime
import argparse

__version__ = '0.1'
__author__ = u'Vsevolod Ivanov'
__appname__ = u'Datetime PyAPI'
__description__ = u'i.e.: python datetimes.py --start $(date +%Y-%m-%d-%H:%M:%S) --end-delta 24'

DT_FMT = '%Y-%m-%d-%H:%M:%S'

def get_parser():
    parser = argparse.ArgumentParser(__appname__, description=__description__)
    parser.add_argument('--version', '-v', action='version', version='%(prog)s '+__version__)
    parser.add_argument('--timezone', required=False, type=str, default='America/Montreal',
                        help='Timezone string i.e. UTC, America/Montreal')
    parser.add_argument('--start-time', '-s', required=True,
                        type=lambda s: datetime.datetime.strptime(s, DT_FMT),
                        help="Start time i.e. '2021-04-05-14:00:00'")
    parser.add_argument('--end-delta', '-e', required=True, type=int,
                        help="Delta delay in hours added to start time: i.e. -24, 24")
    return parser

if __name__ == '__main__':
    parser = get_parser()
    args = parser.parse_args()
    start = args.start_time
    now = datetime.datetime.now(pytz.timezone(args.timezone))
    end = now + datetime.timedelta(hours=args.end_delta)
    print('\n'.join(['Now:      {}', 'Start:    {}', 'End:      {}']).format(
          now.strftime(DT_FMT), start.strftime(DT_FMT), end.strftime(DT_FMT)))
