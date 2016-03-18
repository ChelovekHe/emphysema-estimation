#!/usr/bin/python3
'''See ../README.md'''

import sys, os.path, subprocess, csv, fileinput, shutil, os
from Util import intersperse 

def main():
    fileLists = {
        'Bags' : 'FileLists/CombinedBagsPatchSize21mm.csv',
    }

    outdirs = {
        'ERU' : 'PatchSize21mm/RightUpper',
        'ERM' : 'PatchSize21mm/RightMiddle',
        'ERL' : 'PatchSize21mm/RightLower',
        'ELU' : 'PatchSize21mm/LeftUpper',
        'ELM' : 'PatchSize21mm/LeftMiddle',
        'ELL' : 'PatchSize21mm/LeftLower',
    }
       
    regions = [
        ('ELL', '00'),
        ('ELM', '01'),
        ('ELU', '02'),
        ('ERL', '03'),
        ('ERM', '04'),
        ('ERU', '05') ]

    params = {
        'num-rois' : '50',
    }
    
    with open(fileLists['Bags']) as infile:
        bagList = [row[0].strip() for row in csv.reader(infile)]


        
    for bag in bagList:
        prefix = os.path.basename( bag )[:-8] + '-'
        args = [
            'split', bag, prefix,
            '-d',
            '-l', params['num-rois'],
            '--additional-suffix=.dcm.bag',
            ]
        if subprocess.call( args ) != 0:
            print( 'Error splitting bag', bag )
            return 1

        for region, pattern in regions:
            src = '%s%s.dcm.bag' % (prefix, pattern)
            dst = outdirs[region]
            if not os.path.isdir( dst ):
                try:
                    os.makedirs( dst )
                except OSError as e:
                    print( "Could not make directory:", dst, e )
                    return 1
            try:
                shutil.move( src, dst )
            except shutil.Error as e:
                print( 'Error moving', src, 'to', dst, e )
                return 1
                    
    return 0

if __name__ == '__main__':
    sys.exit( main() )



    
