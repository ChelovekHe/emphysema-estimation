#!/usr/bin/python3
import sys

def main(argv=None):
    if argv is None:
        argv = sys.argv

    if len(argv) != 4:
        print( 'Usage: <bag-size> <number-of-bags> <out>' )
        return 1

    bagSize = int(argv[1])
    numBags = int(argv[2])
    outFile = argv[3]
    
    membership = ''.join( [('%d\n' % i)* bagSize for i in range(numBags)] )
    with open( outFile, 'w') as out:
        print( membership, file=out )
                
    return 0

if __name__ == '__main__':
    sys.exit( main() )
