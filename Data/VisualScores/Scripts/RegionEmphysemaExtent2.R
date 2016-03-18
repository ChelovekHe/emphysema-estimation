#!/usr/bin/Rscript

## Extract visual scores on region basis.

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <scores-path> <dataset-path> <out-prefix>" );
        quit( status = 1 );
    }
    StIds <- GetStIds( options[2] );
    RegionEmphysemaExtent2( options[1], StIds, options[3] );

    quit( status = 0 );
}

GetStIds <- function( path ) {
    StIdsAndDates <- read.csv( path, header=FALSE );
    StIdsAndDates[,1];
}

RegionEmphysemaExtent2 <- function(path, StIds=NULL, pathPrefix='') {
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
    
    regions <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")
    for ( region in regions ) {
        r1 <- paste(region, "L1", sep=".");
        r2 <- paste(region, "M1", sep=".");
                    
        regionScores <- scores[, c(r1, r2) ];

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
        
        low <- regionScores;
        low[ low == 0 ] = 0;
        low[ low == 1 ] = 0;
        low[ low == 2 ] = 0.01;
        low[ low == 3 ] = 0.06;
        low[ low == 4 ] = 0.26;
        low[ low == 5 ] = 0.51;
        low[ low == 6 ] = 0.76;
        low <- pmin(low[,r1], low[,r2])

        high <- regionScores;
        high[ high == 0 ] = 0;
        high[ high == 1 ] = 0;
        high[ high == 2 ] = 0.05;
        high[ high == 3 ] = 0.25;
        high[ high == 4 ] = 0.50;
        high[ high == 5 ] = 0.75;
        high[ high == 6 ] = 1;
        high <- pmax(high[,r1], high[,r2])
        
    
        ## We want values in [0,1]
        intervals <- list(low=low, high=high);
        write.table(intervals,
                    file=paste(pathPrefix, region, '.csv', sep=''),
                    sep=',',
                    quote=FALSE,
                    row.names=FALSE,
                    col.names=FALSE);
    }
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
