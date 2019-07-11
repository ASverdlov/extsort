#!/usr/bin/env python3
import os
import string
import random

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

def random_string(length=10):
    """Generate a random string of fixed length """
    letters = string.ascii_lowercase
    return ''.join(random.choice(letters) for i in range(length))

output_filepath = os.path.join(SCRIPT_DIR, 'output_file')

with open(output_filepath, 'w') as out:
    for i in range(10000000):
        line = random_string()
        out.write(line)
        out.write('\n')
