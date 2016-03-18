#!/usr/bin/python3
import sys, subprocess

def main(argv=None):
    experiment = ('Intervals', 'EmphysemaExtentRegions')
    
    progs = {
        'Intervals' : '../../Build/Classification/TrainClusterModelIntervals',
    }

    labels = {
        'ERU' : '../../Data/CombinedTrainingValidation/Labels/EmphysemaExtentERU.csv',
        'ERM' : '../../Data/CombinedTrainingValidation/Labels/EmphysemaExtentERM.csv',
        'ERL' : '../../Data/CombinedTrainingValidation/Labels/EmphysemaExtentERL.csv',
        'ELU' : '../../Data/CombinedTrainingValidation/Labels/EmphysemaExtentELU.csv',
        'ELM' : '../../Data/CombinedTrainingValidation/Labels/EmphysemaExtentELM.csv',
        'ELL' : '../../Data/CombinedTrainingValidation/Labels/EmphysemaExtentELL.csv',
    }

    instances = {
        'ERU' : '../../Data/CombinedTrainingValidation/Bags/RegionBagsRU.csv',
        'ERM' : '../../Data/CombinedTrainingValidation/Bags/RegionBagsRM.csv',
        'ERL' : '../../Data/CombinedTrainingValidation/Bags/RegionBagsRL.csv',
        'ELU' : '../../Data/CombinedTrainingValidation/Bags/RegionBagsLU.csv',
        'ELM' : '../../Data/CombinedTrainingValidation/Bags/RegionBagsLM.csv',
        'ELL' : '../../Data/CombinedTrainingValidation/Bags/RegionBagsLL.csv',
    }
    
    bagMembership = '../../Data/CombinedTrainingValidation/BagMembership.csv'
    
    params = {
        'ELU' : {
            'k' : ['15',],
        },
        'ERU' : {
            'k' : ['20',],
        },
        'histograms' : '24',
        'max-iterations' : '1000',
        'branching' : '2',
        'kmeans-iterations' : '25',
    }

    regions = ['ELU',] # ['ERU', 'ELU',]
    for region in regions:
        for k in params[region]['k']:
            out = 'Out/CombinedTrainingValidation/Models/%s_%s_%s_k%s' % (experiment + (region, k,))
            cmd = [
                progs[experiment[0]], 
                "--instances", instances[region],
                '--bag-membership', bagMembership,
                "--bag-labels", labels[region],
                "--clusters", k,
                "--histograms", params['histograms'],
                "--branching", params['branching'],
                "--kmeans-iterations", params['kmeans-iterations'],
                "--max-iterations", params['max-iterations'],
                "--output",  out,
            ]
            print( ' '.join( cmd ) )
            if subprocess.call( cmd ) != 0:
                print( 'Error running %s : %s : region = %s : k = %s' % ( experiment + (region, k,)) )
                return 1
                
    return 0

if __name__ == '__main__':
    sys.exit( main() )


