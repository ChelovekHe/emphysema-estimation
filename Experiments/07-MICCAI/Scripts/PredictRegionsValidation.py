#!/usr/bin/python3
import sys, subprocess, csv, os.path, re

def main(argv=None):
    if argv is None:
        argv = sys.argv

    experiments = {
        1 : ('Continuous', 'EmphysemaExtentLung'),
        2 : ('Binary',     'EmphysemaExtentLung'),
    }

    try:
        experiment = experiments[ int(argv[1]) ]
    except Exception as e:
        print( 'usage: prog <experiment number>' )
        return 1
    
    prog = '../../Build/Classification/PredictInstancesCMS'
    bagListPath = "FileLists/RegionBags.csv"
   
    modelPattern = "Out/Training/MaxIterations1000/%s_%s_k%s_1.model"    
    numberOfClusters = ['5', '10', '15', '20', '25', '30']
    params = {
        'histograms' : '24',
        'num-rois' : 50,
    }

    regions = [
        ('LowerLeft'   , 1),
        ('MiddleLeft'  , 2),
        ('UpperLeft'   , 3),
        ('LowerRight'  , 4),
        ('MiddleRight' , 5),
        ('UpperRight'  , 6),
    ]
    
    with open( bagListPath ) as infile:
        bagList = [ row[0].strip() for row in csv.reader( infile ) ]

    bagIdPattern = re.compile("vol([0-9]{1,4}_[0-9]{8})")
        
    # First we make instance predictions
    for k in numberOfClusters:
        regionPredictions = [
            ['IdDate', 'ELL', 'ELM', 'ELU', 'ERL', 'ERM', 'ERU'],
        ]
        for bag in bagList:
            bagIdMatch = bagIdPattern.search( bag )
            bagId = bagIdMatch.group(1)
            prediction = [bagId]
            
            outfilename = os.path.basename( bag ) + \
                          "%s_%s_k%s.instance_predictions" % (experiment + (k,))
            outpath = os.path.join('Out/Validation/RegionBags/InstancePredictions', outfilename)
            cmd = [
                prog,
                "--instances", bag,
                '--model', modelPattern % (experiment + (k,)),
                "--histograms", params['histograms'],
                "--output",  outpath,
            ]
            print( ' '.join( cmd ) )
            if subprocess.call( cmd ) != 0:
                print( 'Error running %s : %s : k = %s' % ( experiment + (k,)) )
                return 1

            # Make region predictions for this bag
            with open( outpath ) as infile:
                instancePredictions = [float(row[0]) for row in csv.reader(infile)]
            for region,v in regions:
                low = (v-1) * params['num-rois']
                high = v * params['num-rois']
                prediction.append( sum(instancePredictions[low:high])/params['num-rois'] )
            regionPredictions.append( prediction )

        outfilename = "%s_%s_k%s.region_predictions" % (experiment + (k,))
        outpath = os.path.join('Out/Validation/RegionBags/RegionPredictions', outfilename)
        with open( outpath, 'w' ) as outfile:
            csv.writer( outfile ).writerows( regionPredictions )
    return 0

if __name__ == '__main__':
    sys.exit( main() )


