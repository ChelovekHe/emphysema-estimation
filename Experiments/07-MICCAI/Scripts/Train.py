#!/usr/bin/python3
import sys, subprocess

def main(argv=None):
    if argv is None:
        argv = sys.argv

    experiments = {
        1 : ('Continuous', 'COPD'),
        2 : ('Binary', '    COPD'),
        3 : ('Continuous', 'EmphysemaExtentLung'),
        4 : ('Binary',     'EmphysemaExtentLung'),
    }

    try:
        experiment = experiments[ int(argv[1]) ]
    except Exception as e:
        print( 'usage: prog <experiment number>' )
        return 1
    
    progs = {
        'Continuous' : '../../Build/Classification/TrainClusterModelContinuous',
        'Binary' : '../../Build/Classification/TrainClusterModelBinary',
    }

    labels = {
        'COPD' : '../../Data/Training/Labels/COPD.csv',
        'EmphysemaExtentLung' : '../../Data/Training/Labels/EmphysemaExtentLung.csv',
    }

    instances = '../../Data/Training/Instances.csv'
    bagMembership = '../../Data/Training/BagMembership.csv'
    
    params = {
        'k' : ['5', '10', '15', '20', '25', '30'],
        'histograms' : '24',
        'max-iterations' : '100',
        'branching' : '2',
        'kmeans-iterations' : '25',
    }
            
    
    for k in params['k']:
        out = 'Out/Training/%s_%s_k%s' % (experiment + (k,))
        cmd = [
            progs[experiment[0]], 
            "--instances", instances,
            '--bag-membership', bagMembership,
            "--bag-labels", labels[experiment[1]],
            "--clusters", k,
            "--histograms", params['histograms'],
            "--branching", params['branching'],
            "--kmeans-iterations", params['kmeans-iterations'],
            "--max-iterations", params['max-iterations'],
            "--output",  out,
        ]
        print( ' '.join( cmd ) )
        if subprocess.call( cmd ) != 0:
            print( 'Error running %s : %s : k = %s' % ( experiment + (k,)) )
            return 1
                
    return 0

if __name__ == '__main__':
    sys.exit( main() )


