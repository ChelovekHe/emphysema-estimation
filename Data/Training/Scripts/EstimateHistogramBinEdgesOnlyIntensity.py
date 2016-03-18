#!/usr/bin/python3
'''See Data/Training/README.md'''

import sys, subprocess, datetime
from Util import intersperse

def main():
    scanLungListPath = 'FileLists/ScansLungs.csv'
    outfile = 'PatchSize21mm/OnlyIntensity/HistogramEdges_bins%s_%s.txt'
    prog = '../../Build/Statistics/DetermineHistogramBinEdgesOnlyIntensity'

    params = {
        'scales' : ['1.2', '2.4', '4.8'], 
        'samples' : '10000',
        'bins' : '25' # = ceil(cuberoot(21*ceil(21/0.78)^2))
    }
    
    args = [
        prog,
        '--infile', scanLungListPath,
        '--outfile', outfile % (params['bins'], datetime.datetime.now().timestamp()),
        '--samples', params['samples'],
        '--bins', params['bins'],
    ]
        
    print(' '.join(args))        
    if subprocess.call( args ) != 0:
        print( 'Error estimating histogram bin edges' )
        return 1
             
    return 0

if __name__ == '__main__':
    sys.exit( main() )



    
