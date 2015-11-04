#!/usr/bin/python3
import sys, os.path, subprocess, csv

def main():
    skip = [
        'LungsLeftRight',
        'LungsInteriorBorder',
        'Regions',
        'Scans',
        'GOLD',
        #'COPD',
    ]
        
    basedir = ''

    dirs = {
        'FileLists' : os.path.join(basedir, 'FileLists'),
        'Labels' : os.path.join(basedir, 'Data', 'Labels'),
        'Bin' : '../../Build',
    }
    
    files = {
        'LungsLeftRight' : os.path.join(dirs['FileLists'], 'LungsROIs.csv'),
        'LungsInteriorBorder' : os.path.join(dirs['FileLists'], 'LungsBinaryROIs.csv'),
        'Regions' : os.path.join(dirs['FileLists'], 'RegionsROIs.csv'),
        'Scans' : os.path.join(dirs['FileLists'], 'BagsLabels.csv'),
        'GOLD' : os.path.join(dirs['FileLists'], 'GOLDLabels.csv'),
        'COPD' : os.path.join(dirs['FileLists'], 'COPDLabels.csv'),
    }
    
    progs = {
        'ExtractLabels' : os.path.join(dirs['Bin'],'FeatureExtraction/ExtractLabels'),
    }

    params = {
        'roi-has-header' : False,
        'LungsLeftRight' : ['--ignore',  '0' ],
        'LungsInteriorBorder' : [ '--dominant', '0' ],
        'Regions' : ['--ignore', '0'],
    }

    extractFrom = [
        'LungsLeftRight',
        'LungsInteriorBorder',
        'Regions',
    ]
    
    populateFrom = [
        'Scans',
        'GOLD',
        'COPD',
    ]
    for k in populateFrom:
        if k in skip:
            print( 'Skipping:', k )
        else:
            print('Running;', k)
            with open(files[k]) as infile:
                bagLabelList = [(row[0].strip(), row[1].strip())
                                for row in csv.reader(infile)]
            for bag, label in bagLabelList:
                print( "Using bag '%s'" % bag )
                print( "Using label '%s'" % label )

                outFile = os.path.join(dirs['Labels'], k + '_' + os.path.basename(bag) + '.labels')
                try:
                    with open( outFile, 'w') as out:
                        with open(bag) as f:
                            for _ in f: print( label, file=out )
                except Exception as e:
                    print('Error creatinglabels', k, e)
                    return 1
            
    for k in extractFrom:
        if k in skip:
            print( 'Skipping:', k )
            continue
        
        # Get the list of images/rois
        with open(files[k]) as infile:
            imageROIsList = [(row[0].strip(), row[1].strip())
                            for row in csv.reader(infile)]

        for image, rois in imageROIsList:
            print( "Using image '%s'" % image )
            print( "Using ROIs '%s'" % rois )
            args = [
                progs['ExtractLabels'],
                '--roi-has-header', '%d' % params['roi-has-header'],
                '--image', image,
                '--roi', rois,
                '--out', os.path.join(dirs['Labels'], k + '_' + os.path.basename(image) + '.labels')
            ] + params[k]
            
            print(' '.join(args))        
            if subprocess.call( args ) != 0:
                print( 'Error extracting labels',k )
                return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
