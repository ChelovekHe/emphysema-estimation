#!/usr/bin/python3
import sys, os, os.path, subprocess, math

def main():
    skip = []

    basedir = './'
    
    dirs = {
        'Statistics' : os.path.join(basedir, 'Statistics'),
        'Scripts' : os.path.join(basedir, 'Scripts'),
        'Plots' : os.path.join(basedir, 'Plots'),
    }
    
    stats = ['LungsLeftRight', 'LungsInteriorBorder', 'Regions', 'Scans',
             'GOLD', 'GOLD2',
             'COPD', 'COPD2',
         ]
    
    files = {}
    for stat in stats:
        files[stat] = {
            'infile' : os.path.join(dirs['Statistics'], stat + '_stats.txt'),
            'outfile' : os.path.join(dirs['Plots'], stat + '.pdf'),
        }
    # {
    #     'LungsLeftRight' : {
    #         'infile' : os.path.join(dirs['Statistics'], 'LungsLeftRight_stats.txt'),
    #         'outfile' : os.path.join(dirs['Plots'], 'LungsLeftRight.pdf'),
    #     },
    #     'LungsInteriorBorder' : {
    #         'infile' : os.path.join(dirs['Statistics'], 'LungsInteriorBorder_stats.txt'),
    #         'outfile' : os.path.join(dirs['Plots'], 'LungsInteriorBorder.pdf'),
    #     },
    #     'Regions' : {
    #         'infile' : os.path.join(dirs['Statistics'], 'Regions_stats.txt'),
    #         'outfile' : os.path.join(dirs['Plots'], 'Regions.pdf'),
    #     },
    #     'Scans' : {
    #         'infile' : os.path.join(dirs['Statistics'], 'Scans_stats.txt'),
    #         'outfile' : os.path.join(dirs['Plots'], 'Scans.pdf'),
    #     },
    #     'GOLD' : {
    #         'infile' : os.path.join(dirs['Statistics'], 'GOLD_stats.txt'),
    #         'outfile' : os.path.join(dirs['Plots'], 'GOLD.pdf'),
    #     },
    # }
    
    progs = {
        'Plot' : os.path.join(dirs['Scripts'], 'EntropyPlots.R'),
    }

    params = {
        'LungsLeftRight' : {
            'title' : 'LungsLeftRight_stats.txt',
            'labeldim' : '%0.4f' % -math.log(1.0/2),
        },
        'LungsInteriorBorder' : {
            'title' : 'LungsInteriorBorder_stats.txt',
            'labeldim' : '%0.4f' % -math.log(1.0/2),
        },
        'Regions' : {
            'title' : 'Regions_stats.txt',
            'labeldim' : '%0.4f' % -math.log(1.0/6),
        },
        'Scans' : {
            'title' : 'Scans_stats.txt',
            'labeldim' : '%0.4f' % - math.log(1.0/10),
        },
        'GOLD' : {
            'title' : 'GOLD_stats.txt',
            'labeldim' : '%0.4f' % - math.log(1.0/4),
        },
        'GOLD2' : {
            'title' : 'GOLD2_stats.txt',
            'labeldim' : '%0.4f' % - math.log(1.0/4),
        },
        'COPD' : {
            'title' : 'COPD_stats.txt',
            'labeldim' : '%0.4f' % - math.log(1.0/2),
        },
        'COPD2' : {
            'title' : 'COPD2_stats.txt',
            'labeldim' : '%0.4f' % - math.log(1.0/2),
        },
    }
    

    
    for k in stats:
        if k in skip:
            print( 'Skipping', k )
        else:
            print( 'Running', k)
            args = [
                progs['Plot'],
                files[k]['infile'],
                files[k]['outfile'],
                params[k]['title'],
                params[k]['labeldim']
            ]
            print(' '.join(args))        
            if subprocess.call( args ) != 0:
                print( 'Error plotting', k )
                return 1
        
    return 0

if __name__ == '__main__':
    sys.exit( main() )

