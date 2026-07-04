# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0
# Copyright Blueleap Contributors 2025.

import os
import shutil

class CleanCommand(WestCommand):
    def __init__(self):
        super().__init__(
            'cleanall',
            'Clean all project artifacts in the workspace',
            'Extended clean logic for all apps and logs'
        )

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(self.name)
        parser.add_argument('-f', '--full', action='store_true',
                            help='Also remove extra folders')
        return parser

    def do_run(self, args, unknown_args):
        print("Cleaning main build...")
        shutil.rmtree('build', ignore_errors=True)

        if args.full:
            print("Cleaning logs and temp dirs...")
            shutil.rmtree('logs', ignore_errors=True)
            shutil.rmtree('.pytest_cache', ignore_errors=True)
            shutil.rmtree('build-cm7', ignore_errors=True)
            shutil.rmtree('build-cm4', ignore_errors=True)
        print("Done.")
