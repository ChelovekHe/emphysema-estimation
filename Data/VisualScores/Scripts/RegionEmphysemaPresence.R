#!/usr/bin/Rscript

## 

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <scores-path> <dataset-path> <out-path>" );
        quit( status = 1 );
    }
    StIds <- GetStIds( options[2] );
    regionEmphysemaPresence <- RegionEmphysemaPresence( options[1], StIds );
    write.csv(regionEmphysemaPresence,
              file=options[3],
              quote=FALSE,
              row.names=FALSE);
    quit( status = 0 );
}

GetStIds <- function( path ) {
    StIdsAndDates <- read.csv( path, header=FALSE );
    StIdsAndDates[,1];
}

RegionEmphysemaPresence <- function(path, StIds=NULL) {
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
    ##   ERU : Emphysema score in right upper region
    ## The region scores are given as a value from zero to six.
    ## The values correspond to percentage intervals indicating the extent
    ## of emphsyema in the region. 
    ## 0 : NA ( No emphysema in the lung )
    ## 1 : 0% ( In the region )
    ## 2 : 1-5%
    ## 3 : 6-25%
    ## 4 : 26-50%
    ## 5 : 51-75%
    ## 6 : 76-100%
    ## We want a binary presence/absence label so (0,1) maps to 0 and > 1 to 1
    emphysemaPresence <- as.integer((scores[,"ERU.L1"] > 1) | (scores[, "ERU.M1"] > 1) ) ;
    
    list(StudyId=scores$StId,
         Date=scores$Dato.1,
         EmphysemaPresence=emphysemaPresence);
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
