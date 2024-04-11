#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
I3Time tests
"""

import unittest
from datetime import date, datetime, timedelta, timezone

from icecube.dataclasses import I3Time, make_I3Time


class I3TimeTest(unittest.TestCase):
    def test_print(self):
        t = I3Time(2012, 116640000117197235)
        s = str(t)
        subsecond = int(s.split(" ")[-2].split(".")[-1].replace(",", ""))
        self.assertEqual(subsecond, 117197235)

    def test_date(self):
        d0 = date(2020, 2, 29)
        t0 = make_I3Time(d0)
        self.assertEqual(t0.utc_year, 2020)
        self.assertEqual(t0.utc_month, 2)
        self.assertEqual(t0.utc_day_of_month, 29)
        self.assertEqual(t0.mod_julian_sec, 0)
        self.assertEqual(t0.mod_julian_nano_sec, 0)
        self.assertEqual(d0, t0.date_time.date())

    def test_datetime(self):
        with self.assertRaises(ValueError):
            make_I3Time(datetime(2020, 1, 1, 0, 0, 0)) # noqa: DTZ001
        with self.assertRaises(ValueError):
            make_I3Time(datetime(2020, 1, 1, 0, 0, 0, tzinfo=timezone(timedelta(hours=-6))))
        with self.assertRaises(TypeError):
            print(make_I3Time(None))

        d0 = datetime(2020, 2, 29, 1, 2, 3, 456789, tzinfo=timezone.utc)
        t0 = make_I3Time(d0)
        self.assertEqual(t0.utc_year, 2020)
        self.assertEqual(t0.utc_month, 2)
        self.assertEqual(t0.utc_day_of_month, 29)
        self.assertEqual(t0.mod_julian_sec, (1 * 60 + 2) * 60 + 3)
        self.assertEqual(t0.mod_julian_micro_sec, 456789)
        self.assertEqual(t0.mod_julian_nano_sec, 456789000)
        self.assertEqual(d0, t0.date_time)
        self.assertEqual("".join(str(t0).split(",")[:2]), str(d0).split("+")[0])

if __name__ == "__main__":
    unittest.main()
