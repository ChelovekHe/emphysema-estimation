#!/usr/bin/Rscript

## Extract visual scoring and aggregate on lung basis

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <scores-path> <dataset-path> <out-path>" );
        quit( status = 1 );
    }
    StIds <- GetStIds( options[2] );
    lungExtent <- LungEmphysemaExtent( options[1], StIds );
    write.csv(lungExtent,
              file=options[3],
              quote=FALSE,
              row.names=FALSE);
    quit( status = 0 );
}

GetStIds <- function( path ) {
    StIdsAndDates <- read.csv( path, header=FALSE );
    StIdsAndDates[,1];
}

LungEmphysemaExtent <- function(path, StIds=NULL) {
    scores <- read.csv(path, header=TRUE);

    ## If we are given a set of study ids, then we only use those studies
    if ( !is.null(StIds) ) {
        scores <- subset(scores, StId %in% StIds );
        ## We might not get all the studies we want, so we print those we use
        print( subset( data.frame(list(id=StIds)), !id %in% scores$StId )[[1]] )
    }
    
    ## There are are scores for the first and the last scan for each participant
    ## There are two scorings for each scans.
    ## .L1 and .M1 for the first scans
    ## .L2 and .M2 for the last scans
    ##
    ## We want the first scans
    
    ## There are a lot of variables. We want
    ##   ERU : Emphysema Right Upper
    ##   ERM : Emphysema Right Middle
    ##   ERL : Emphysema Right Lower
    ##   ELU : Emphysema Left Upper
    ##   ELM : Emphysema Left Middle
    ##   ELL : Emphysema Left Lower
    ##
    regionScores <- scores[,c("ERU.L1", "ERU.M1",
                              "ERM.L1", "ERM.M1",
                              "ERL.L1", "ERL.M1",
                              "ELU.L1", "ELU.M1",
                              "ELM.L1", "ELM.M1",
                              "ELL.L1", "ELL.M1")];

    ## The region scores are given as a value from zero to six.
    ## The values correspond to percentage intervals indicating the extent
    ## of emphsyema in the region
    ## 0 : NA ( No emphysema in the lung )
    ## 1 : 0%
    ## 2 : 1-5%
    ## 3 : 6-25%
    ## 4 : 26-50%
    ## 5 : 51-75%
    ## 6 : 76-100%
    ##
    ## We convert the categories to the midpoints of the intervals    
    regionExtent <- regionScores;
    regionExtent[ regionExtent == 0 ] = 0;
    regionExtent[ regionExtent == 1 ] = 0;
    regionExtent[ regionExtent == 2 ] = 2.5;
    regionExtent[ regionExtent == 3 ] = 15.5;
    regionExtent[ regionExtent == 4 ] = 38;
    regionExtent[ regionExtent == 5 ] = 63;
    regionExtent[ regionExtent == 6 ] = 88;

    ## We want to combine the region extents into a lung extent
    lungExtent <- rowMeans( regionExtent ) / 100;
    
    list(StudyId=scores$StId,
         Date=scores$Dato.1,
         LungEmphysemaExtent=lungExtent);
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
