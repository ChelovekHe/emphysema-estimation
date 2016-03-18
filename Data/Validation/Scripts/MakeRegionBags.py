#!/usr/bin/python3
'''See ../README.md'''

import sys, os.path, subprocess, csv, fileinput
from Util import intersperse 

def main():
    fileLists = {
        'Scans' : 'FileLists/Scans.csv',
        'Lungs' : 'FileLists/Lungs.csv',
        'Regions' : 'FileLists/Regions.csv'
    }

    outdirs = {
        'ROIs' : 'ROIs',
        'Bags' : 'RegionBags'
    }
    
    progs = {
        'MakeBag' : '../../Build/FeatureExtraction/MakeBag',
        'GenerateROIs' : '../../Build/ROIExtraction/GenerateROIs',
    }
    
    params = {
        'histogram-spec' : '../Training/HistogramEdges_1456312478.268112.txt',
        'scales' : ['1.2', '2.4', '4.8'],
        'num-rois' : '50',
        'roi-size-x' : '53',
        'roi-size-y' : '53',
        'roi-size-z' : '41',
    }
    
    regions = [
        ('LowerLeft'   , '1'),
        ('MiddleLeft'  , '2'),
        ('UpperLeft'   , '3'),
        ('LowerRight'  , '4'),
        ('MiddleRight' , '5'),
        ('UpperRight'  , '6'),
    ]

    # First we generate the ROIs
    print( 'Generating ROIs' )
    with open(fileLists['Regions']) as infile:
        regionList = [row[0].strip() for row in csv.reader(infile)]

    roiList = []
    for mask in regionList:
        print( "Using region mask '%s'" % mask )        
        outfilename = os.path.basename( mask ) + '.ROIInfo'
        roifiles = []
        for region, value in regions:
            outdir = os.path.join( outdirs['ROIs'], region )
            if not os.path.isdir( outdir ):
                try:
                    os.makedirs( outdir )
                except OSError as e:
                    print( 'Could not create output directory:', outdir, e )
                    return 1                
            outpath = os.path.join( outdir, outfilename )
            roifiles.append( outpath )
            args = [
                progs['GenerateROIs'],
                '--mask', mask,
                '--outfile', outpath,
                '--num-rois', params['num-rois'],
                '--roi-size-x', params['roi-size-x'],
                '--roi-size-y', params['roi-size-y'],
                '--roi-size-z', params['roi-size-z'],
                '--mask-value', value
            ]

            if subprocess.call( args ) != 0:
                print( 'Error generating ROIs' )
                return 1

        # Concatenate the ROIs into a single big ROI file
        outdir = os.path.join(outdirs['ROIs'], 'Combined' )
        if not os.path.isdir( outdir ):
            try:
                os.makedirs( outdir )
            except OSError as e:
                print( 'Could not create output directory:', outdir, e )
                return 1
            
        outpath = os.path.join( outdir, outfilename )
        roiList.append( outpath )
        with open(outpath, 'w') as outfile:
            with fileinput.input(files=roifiles) as infile:
                outfile.writelines( infile )
                    
                    
    # Now we extract the bags
    with open(fileLists['Scans']) as infile:
        scanList = [row[0].strip() for row in csv.reader(infile)]
    with open(fileLists['Lungs']) as infile:
        lungList = [row[0].strip() for row in csv.reader(infile)]

    for scan, lung, roi in zip(scanList, lungList, roiList):
        print( "Using scan '%s'" % scan )
        print( "Using lung mask '%s'" % lung )
        print( "Using ROI file '%s'" % roi )
        args = [
            progs['MakeBag'],
            '--image', scan,
            '--mask', lung,
            '--histogram-spec', params['histogram-spec'],
            '--outdir', outdirs['Bags'],
            '--prefix', os.path.basename(scan),
            '--roi-file', roi,
            '--roi-file-has-header', '0',
        ] + list(intersperse('--scale', (s for s in params['scales'])))
        
        print(' '.join(args))        
        if subprocess.call( args ) != 0:
            print( 'Error making bag' )
            return 1
            
    return 0

if __name__ == '__main__':
    sys.exit( main() )



    
