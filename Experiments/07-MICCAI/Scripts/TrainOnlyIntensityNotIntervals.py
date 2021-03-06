#!/usr/bin/python3
import sys, subprocess

def main(argv=None):
    experiment = ('Intervals', 'EmphysemaExtentRegions')
    
    progs = {
        'Intervals' : '../../Build/Classification/TrainClusterModelBinary',
    }

    labels = {
        'ERU' : '../../Data/Training/Labels/AverageEmphysemaExtentERU.csv',
        'ERM' : '../../Data/Training/Labels/AverageEmphysemaExtentERM.csv',
        'ERL' : '../../Data/Training/Labels/AverageEmphysemaExtentERL.csv',
        'ELU' : '../../Data/Training/Labels/AverageEmphysemaExtentELU.csv',
        'ELM' : '../../Data/Training/Labels/AverageEmphysemaExtentELM.csv',
        'ELL' : '../../Data/Training/Labels/AverageEmphysemaExtentELL.csv',
    }

    instances = {
        'ERU' : '../../Data/Training/PatchSize21mm/OnlyIntensity/RegionBagsRU.csv',
        'ERM' : '../../Data/Training/PatchSize21mm/OnlyIntensity/RegionBagsRM.csv',
        'ERL' : '../../Data/Training/PatchSize21mm/OnlyIntensity/RegionBagsRL.csv',
        'ELU' : '../../Data/Training/PatchSize21mm/OnlyIntensity/RegionBagsLU.csv',
        'ELM' : '../../Data/Training/PatchSize21mm/OnlyIntensity/RegionBagsLM.csv',
        'ELL' : '../../Data/Training/PatchSize21mm/OnlyIntensity/RegionBagsLL.csv',
    }
    
    bagMembership = '../../Data/Training/PatchSize21mm/RegionBagMembership.csv'
    
    params = {
        'k' : ['5', '10', '15', '20', '25', '30'],
        'histograms' : '1',
        'max-iterations' : '1',
        'branching' : '2',
        'kmeans-iterations' : '25',
    }

    regions = ['ERU', 'ERM', 'ERL', 'ELU', 'ELM', 'ELL']
    for region in regions:
        for k in params['k']:
            out = 'Out/Training/NotIntervals/%s_%s_%s_k%s' % (experiment + (region, k,))
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


