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
        'ROIs' : 'PatchSize21mm/OnlyIntensity/ROIs',
        'Bags' : 'PatchSize21mm/OnlyIntensity/RegionBags'
    }

    if not os.path.isdir( outdirs['Bags'] ):
        try:
            os.makedirs( outdirs['Bags'] )
        except OSError as e:
            print( 'Could not create bag out dir:', outdirs['Bags'], e )
            return 1
    
    progs = {
        'MakeBag' : '../../Build/FeatureExtraction/MakeBagOnlyIntensity',
        'GenerateROIs' : '../../Build/ROIExtraction/GenerateROIs',
    }
    
    params = {
        'histogram-spec' : '../Training/PatchSize21mm/OnlyIntensity/HistogramEdges_bins25_1457608873.641941.txt',
        'num-rois' : '50',
        'roi-size-x' : '27',
        'roi-size-y' : '27',
        'roi-size-z' : '21',
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

            #if subprocess.call( args ) != 0:
            #    print( 'Error generating ROIs' )
            #    return 1

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
        ]
        
        print(' '.join(args))        
        if subprocess.call( args ) != 0:
            print( 'Error making bag' )
            return 1
            
    return 0

if __name__ == '__main__':
    sys.exit( main() )



    
