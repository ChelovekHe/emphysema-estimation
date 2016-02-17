#!/usr/bin/python3
import sys, os.path, subprocess, csv

def main():
    skip = [
        'LungsLeftRight',
        'LungsInteriorBorder',
        'Regions',
        'Scans',
        'GOLD',
        'GOLD2',
        'COPD',
        'COPD2',
    ]
    
    basedir = ''

    dirs = {
        'Instances' : os.path.join(basedir, 'Data', 'Instances'),
        'Labels' : os.path.join(basedir, 'Data', 'Labels'),
        'Statistics' : os.path.join(basedir, 'Statistics'),
        'Bin' : '../../Build',
    }
    
    files = {
        'Labels' : {
            'LungsLeftRight' : os.path.join(dirs['Labels'], 'LungsLeftRight.labels'),
            'LungsInteriorBorder' : os.path.join(dirs['Labels'], 'LungsInteriorBorder.labels'),
            'Regions' : os.path.join(dirs['Labels'], 'Regions.labels'),
            'Scans' : os.path.join(dirs['Labels'], 'Scans.labels'),
            'GOLD' : os.path.join(dirs['Labels'], 'GOLD.labels'),
            'GOLD2' : os.path.join(dirs['Labels'], 'GOLD.labels'),
            'COPD' : os.path.join(dirs['Labels'], 'COPD.labels'),
            'COPD2' : os.path.join(dirs['Labels'], 'COPD.labels'),
        },
        'Instances' : os.path.join(dirs['Instances'], 'instances500.csv'),
    }
    
    progs = {
        'ClusterInterpretation' : os.path.join(dirs['Bin'],'Experiments/04-InterpretationOfClusters/ClusterInterpretation'),
    }

    params = {
        'histograms' : 56,
        'iterations' : 1000,
        'LungsLeftRight' : {
            'branching' : 2,
            'clusters' : 2,
        },
        'LungsInteriorBorder' : {
            'branching' : 2,
            'clusters' : 2,
        },
        'Regions' : {
            'branching' : 2,
            'clusters' : 6,
        },
        'Scans' : {
            'branching' : 2,
            'clusters' : 10,
        },
        'GOLD' : {
            'branching' : 2,
            'clusters' : 4,
        },
        'GOLD2' : {
            'branching' : 2,
            'clusters' : 10,
        },
        'COPD' : {
            'branching' : 2,
            'clusters' : 2,
        },
        'COPD2' : {
            'branching' : 2,
            'clusters' : 10,
        },
    }    

    for k in files['Labels'].keys():
        if k in skip:
            print( 'Skipping:', k )
        else:
            print( 'Running:', k )
            args = [
                progs['ClusterInterpretation'],
                '--input', files['Instances'],
                '--labels', files['Labels'][k],
                '--histograms', '%d' % params['histograms'],
                '--iterations', '%d' % params['iterations'],
                '--branching', '%d' % params[k]['branching'],
                '--clusters', '%d' % params[k]['clusters'],
                '--output', os.path.join(dirs['Statistics'], k + '_stats.txt')
            ]
            
            print(' '.join(args))        
            if subprocess.call( args ) != 0:
                print( 'Error extracting labels' )
                return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
