#!/usr/bin/python3
import sys, subprocess

def main(argv=None):
    experiment =  ('Intervals', 'EmphysemaExtentRegions')
    
    progs = {
        'Intervals' : '../../Build/Classification/PredictClusterModel'
    }

    instances = {
        'ERU' : '../../Data/Test/PatchSize21mm/RegionBagsRU.csv',
        'ELU' : '../../Data/Test/PatchSize21mm/RegionBagsLU.csv',
    }

    bagMembership = '../../Data/Test/PatchSize21mm/RegionBagMembership.csv'

    modelPattern = 'Out/CombinedTrainingValidation/Models/%s_%s_%s_k%s_1.model'
    numberOfClusters = ['5', '10', '15', '20','25', '30']
    params = {
        'ERU' : {
            'k' : ['20',],
        },
        'ELU' : {
            'k' : ['15',],
        },
        'histograms' : '24',
    }

    regions = ['ERU', 'ELU',]
    for region in regions:
        for k in params[region]['k']:
            out = 'Out/NoCMAES/CombinedTrainingValidation/Predictions/%s_%s_%s_k%s' % (experiment + (region, k,))
            cmd = [
                progs['Intervals'],
                "--instances", instances[region],
                '--bag-membership', bagMembership,
                '--model', modelPattern % (experiment + (region, k,)),
                "--histograms", params['histograms'],
                "--output",  out,
            ]
            print( ' '.join( cmd ) )
            if subprocess.call( cmd ) != 0:
                print( 'Error running %s : %s : %s : k = %s' % ( experiment + (region, k,)) )
                return 1
                
    return 0

if __name__ == '__main__':
    sys.exit( main() )


