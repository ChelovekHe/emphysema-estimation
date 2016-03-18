#!/usr/bin/python3
import sys, subprocess

def main(argv=None):
    experiment = ('Intervals', 'EmphysemaExtentRegions')
    
    progs = {
        'Intervals' : '../../Build/Classification/TrainClusterModelIntervals',
    }

    labels = {
        'ERU' : '../../Data/Training/Labels/EmphysemaExtentERU.csv',
        'ERM' : '../../Data/Training/Labels/EmphysemaExtentERM.csv',
        'ERL' : '../../Data/Training/Labels/EmphysemaExtentERL.csv',
        'ELU' : '../../Data/Training/Labels/EmphysemaExtentELU.csv',
        'ELM' : '../../Data/Training/Labels/EmphysemaExtentELM.csv',
        'ELL' : '../../Data/Training/Labels/EmphysemaExtentELL.csv',
    }

    instances = {
        'ERU' : '../../Data/Training/PatchSize21mm/RegionBagsRU.csv',
        'ERM' : '../../Data/Training/PatchSize21mm/RegionBagsRM.csv',
        'ERL' : '../../Data/Training/PatchSize21mm/RegionBagsRL.csv',
        'ELU' : '../../Data/Training/PatchSize21mm/RegionBagsLU.csv',
        'ELM' : '../../Data/Training/PatchSize21mm/RegionBagsLM.csv',
        'ELL' : '../../Data/Training/PatchSize21mm/RegionBagsLL.csv',
    }
    
    bagMembership = '../../Data/Training/PatchSize21mm/RegionBagMembership.csv'
    
    params = {
        'k' : ['5', '10', '15', '20', '25', '30'],
        'histograms' : '24',
        'max-iterations' : '1000',
        'branching' : '2',
        'kmeans-iterations' : '25',
    }

    regions = ['ERU', 'ERM', 'ERL', 'ELU', 'ELM', 'ELL']
    for region in regions:
        for k in params['k']:
            out = 'Out/Training/PatchSize21mm/%s_%s_%s_k%s' % (experiment + (region, k,))
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


