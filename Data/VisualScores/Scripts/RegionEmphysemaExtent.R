#!/usr/bin/Rscript

## Extract visual scores on region basis.

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <scores-path> <dataset-path> <out-path>" );
        quit( status = 1 );
    }
    StIds <- GetStIds( options[2] );
    regionExtent <- RegionEmphysemaExtent( options[1], StIds );
    write.csv(regionExtent,
              file=options[3],
              quote=FALSE,
              row.names=FALSE);
    quit( status = 0 );
}

GetStIds <- function( path ) {
    StIdsAndDates <- read.csv( path, header=FALSE );
    StIdsAndDates[,1];
}

RegionEmphysemaExtent <- function(path, StIds=NULL) {
    scores <- read.csv(path, header=TRUE);

    ## If we are given a set of study ids, then we only use those studies
    if ( !is.null(StIds) ) {
        scores <- subset(scores, StId %in% StIds );
        ## We might not get all the studies we want, so we print those we dont use
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
    regionExtent[ regionExtent == 2 ] = 3;
    regionExtent[ regionExtent == 3 ] = 15.5;
    regionExtent[ regionExtent == 4 ] = 38;
    regionExtent[ regionExtent == 5 ] = 63;
    regionExtent[ regionExtent == 6 ] = 88;

    ## We want values in [0,1]
    regionExtent <- regionExtent / 100;
    avgRegionExtent <-
        (regionExtent[, c(1,3,5,7,9,11)] + regionExtent[, c(2,4,6,8,10,12)]) / 2;
    colnames( avgRegionExtent ) <- c("ERU", "ERM", "ERL",
                                     "ELU", "ELM", "ELL");
    
    list(StudyId=scores$StId,
         Date=scores$Dato.1,
         RegionEmphysemaExtent=avgRegionExtent);
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
