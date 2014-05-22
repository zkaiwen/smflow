#!/bin/bash
./xfpgeniusDB database/primitives database/c_small database/db_small

./xfpgeniusRef circuits/testbench/adder4.g database/primitives database/db_small
