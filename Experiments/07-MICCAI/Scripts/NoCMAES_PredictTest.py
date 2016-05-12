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

    modelPattern = 'Out/NoCMAES/CombinedTrainingValidation/Models/%s_%s_%s_k%s_%d.model'
    numberOfClusters = ['5', '10', '15', '20', '25', '30']
    params = {
        'ERU' : {
            'k' : ['20',],
        },
        'ELU' : {
            'k' : ['15',],
        },
        'histograms' : '24',
    }

    regions = ['ERU',]
    for region in regions:
        for k in params[region]['k']:
            for i in range(100,200):
                out = 'Out/NoCMAES/CombinedTrainingValidation/Predictions/%s_%s_%s_k%s_%d' % (experiment + (region, k, i))
                cmd = [
                    progs['Intervals'],
                    "--instances", instances[region],
                    '--bag-membership', bagMembership,
                    '--model', modelPattern % (experiment + (region, k, i)),
                    "--histograms", params['histograms'],
                    "--output",  out,
                ]
                print( ' '.join( cmd ) )
                if subprocess.call( cmd ) != 0:
                    print( 'Error running %s : %s : %s : k = %s : i = %d' % ( experiment + (region, k, i)) )
                    return 1
                
    return 0

if __name__ == '__main__':
    sys.exit( main() )


