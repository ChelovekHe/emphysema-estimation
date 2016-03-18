#!/usr/bin/python3
import sys, subprocess

def main(argv=None):
    experiment =  ('Intervals', 'EmphysemaExtentRegions')
    
    progs = {
        'Intervals' : '../../Build/Classification/PredictClusterModel'
    }

    instances = {
        'ERU' : '../../Data/Validation/PatchSize21mm/OnlyIntensity/RegionBagsRU.csv',
        'ERM' : '../../Data/Validation/PatchSize21mm/OnlyIntensity/RegionBagsRM.csv',
        'ERL' : '../../Data/Validation/PatchSize21mm/OnlyIntensity/RegionBagsRL.csv',
        'ELU' : '../../Data/Validation/PatchSize21mm/OnlyIntensity/RegionBagsLU.csv',
        'ELM' : '../../Data/Validation/PatchSize21mm/OnlyIntensity/RegionBagsLM.csv',
        'ELL' : '../../Data/Validation/PatchSize21mm/OnlyIntensity/RegionBagsLL.csv',
    }

    bagMembership = '../../Data/Validation/PatchSize21mm/RegionBagMembership.csv'

    modelPattern = 'Out/Training/NotIntervals/%s_%s_%s_k%s_1.model'
    numberOfClusters = ['5', '10', '15', '20','25', '30']
    params = {
        'histograms' : '1',
    }

    regions = ['ERU', 'ERM', 'ERL', 'ELU', 'ELM', 'ELL']
    for region in regions:
        for k in numberOfClusters:
            out = 'Out/Validation/NotIntervals/%s_%s_%s_k%s' % (experiment + (region, k,))
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


