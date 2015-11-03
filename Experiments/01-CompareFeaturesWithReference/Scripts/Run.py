#/usr/bin/python3

import sys, subprocess

def main():
    progs = [
        './GenerateFeatureImages.py',
        './GeneratePatches.py',
        './GenerateJointHistograms.py'
    ]

    for prog in progs:
        if subprocess.call(prog) != 0:
            print ( 'Error calling', prog )
            return 1


if __name__ == '__main__':
    sys.exit(main())
