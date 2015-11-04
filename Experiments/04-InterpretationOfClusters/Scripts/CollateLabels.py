#!/usr/bin/python3
import sys, os.path, subprocess, csv, re

def main():
    skip = [
        'LungsLeftRight',
        'LungsInteriorBorder',
        'Regions',
        'Scans',
        'GOLD',
        'COPD',
    ]

    basedir = ''

    dirs = {
        'FileLists' : os.path.join(basedir, 'FileLists'),
        'Labels' : os.path.join(basedir, 'Data', 'Labels'),
    }
    
    files = {
        'Ordering' : os.path.join(dirs['FileLists'], 'LabelOrdering.csv'),
        'LungsInteriorBorder' : os.path.join(dirs['FileLists'], 'LungInteriorBorderLabels.csv'),
        'LungsLeftRight' : os.path.join(dirs['FileLists'], 'LungLeftRightLabels.csv'),        
        'Regions' : os.path.join(dirs['FileLists'], 'RegionLabels.csv'),
        'Scans' : os.path.join(dirs['FileLists'], 'ScanLabels.csv'),
        'GOLD' : os.path.join(dirs['FileLists'], 'GOLDLabelFiles.csv'),
        'COPD' : os.path.join(dirs['FileLists'], 'COPDLabelFiles.csv'),
    }

    # Get the ordering
    with open(files['Ordering']) as infile:
        ordering = [ re.compile(row[0].strip()) for row in csv.reader(infile)]

    def findOrderIndex(s):
        for i,o in enumerate(ordering):
            if o.search(s) is not None:
                return i
        raise "Key not found"
        
    labelTypes = [
        'LungsLeftRight',
        'LungsInteriorBorder',
        'Regions',
        'Scans',
        'GOLD',
        'COPD',
    ]

    for k in labelTypes:
        if k in skip:
            print( 'Skipping:', k)
            continue
        print('Collating:', k)
        with open(files[k]) as infile:
            labelFiles = [(findOrderIndex(row[0]), row[0]) for row in csv.reader(infile)]

        # Sort according to ordering and discard the ordering index
        sortedLabelFiles = [s for _,s in sorted(labelFiles)]
        try:
            with open(os.path.join(dirs['Labels'], k + '.labels'), 'w') as out:
                writer = csv.writer(out)
                for f in sortedLabelFiles:
                    with open(f) as f2:
                        writer.writerows((l for l in csv.reader(f2)))
        except Exception as e:
            print( 'Error collating labels', e )
            return 1
        
    return 0

if __name__ == '__main__':
    sys.exit( main() )
