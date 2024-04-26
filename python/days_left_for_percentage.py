#!/usr/bin/python3
#
# print('days_left_for_percentage.py <end-date> <% completed>')
# print('i.e.: days_left_for_percentage.py 6/16/2024 50')
#
import sys
from datetime import datetime as dt

date_format = "%m/%d/%Y"
end_date = dt.strptime(sys.argv[1], date_format)

days = (end_date - dt.today()).days
percentage = (100 - float(sys.argv[2])) / (days)
print('days left: {}\ntempo per day: {:.2f}%'.format(days, percentage))
