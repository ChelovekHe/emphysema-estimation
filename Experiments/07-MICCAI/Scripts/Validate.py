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
    
    prog = '../../Build/Classification/PredictClusterModel'

    labels = {
        'COPD' : '../../Data/Validation/Labels/COPD.csv',
        'EmphysemaExtentLung' : '../../Data/Validation/Labels/EmphysemaExtentLung.csv',
    }

    instances = '../../Data/Validation/Instances.csv'
    bagMembership = '../../Data/Validation/BagMembership.csv'

    modelPattern = "Out/Training/%s_%s_k%s_1.model"
    numberOfClusters = ['5', '10', '15', '20', '25', '30']
    params = {
        'histograms' : '24',
    }

    for k in numberOfClusters:
        out = 'Out/Validation/%s_%s_k%s' % (experiment + (k,))
        cmd = [
            prog,
            "--instances", instances,
            '--bag-membership', bagMembership,
            '--model', modelPattern % (experiment + (k,)),
            "--histograms", params['histograms'],
            "--output",  out,
        ]
        print( ' '.join( cmd ) )
        if subprocess.call( cmd ) != 0:
            print( 'Error running %s : %s : k = %s' % ( experiment + (k,)) )
            return 1
                
    return 0

if __name__ == '__main__':
    sys.exit( main() )


