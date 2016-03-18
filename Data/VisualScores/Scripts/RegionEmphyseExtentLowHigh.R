#!/usr/bin/Rscript

## Extract visual scores on region basis.

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <scores-path> <dataset-path> <out-path>" );
        quit( status = 1 );
    }
    StIds <- GetStIds( options[2] );
    RegionEmphysemaExtentLowHigh( options[1], StIds, options[3] );

    quit( status = 0 );
}

GetStIds <- function( path ) {
    StIdsAndDates <- read.csv( path, header=FALSE );
    StIdsAndDates[,1];
}

RegionEmphysemaExtentLowHigh <- function(path, StIds=NULL, outpath='lowhigh.csv') {
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
    
    low.L1 <- scores[, c("ERU.L1", "ERM.L1", "ERL.L1", "ELU.L1", "ELM.L1", "ELL.L1")]
    low.L1[low.L1 <= 1] <- 0;
    low.L1[low.L1 == 2] <- 0.01;
    low.L1[low.L1 == 3] <- 0.06;
    low.L1[low.L1 == 4] <- 0.26;
    low.L1[low.L1 == 5] <- 0.51;
    low.L1[low.L1 == 6] <- 0.76;

    high.L1 <- scores[, c("ERU.L1", "ERM.L1", "ERL.L1", "ELU.L1", "ELM.L1", "ELL.L1")]
    high.L1[high.L1 <= 1] <- 0;
    high.L1[high.L1 == 2] <- 0.05;
    high.L1[high.L1 == 3] <- 0.25;
    high.L1[high.L1 == 4] <- 0.50;
    high.L1[high.L1 == 5] <- 0.75;
    high.L1[high.L1 == 6] <- 1;

    low.M1 <- scores[, c("ERU.M1", "ERM.M1", "ERL.M1", "ELU.M1", "ELM.M1", "ELL.M1")]
    low.M1[low.M1 <= 1] <- 0;
    low.M1[low.M1 == 2] <- 0.01;
    low.M1[low.M1 == 3] <- 0.06;
    low.M1[low.M1 == 4] <- 0.26;
    low.M1[low.M1 == 5] <- 0.51;
    low.M1[low.M1 == 6] <- 0.76;

    high.M1 <- scores[, c("ERU.M1", "ERM.M1", "ERL.M1", "ELU.M1", "ELM.M1", "ELL.M1")]
    high.M1[high.M1 <= 1] <- 0;
    high.M1[high.M1 == 2] <- 0.05;
    high.M1[high.M1 == 3] <- 0.25;
    high.M1[high.M1 == 4] <- 0.50;
    high.M1[high.M1 == 5] <- 0.75;
    high.M1[high.M1 == 6] <- 1;

    low <- pmin(low.L1, low.M1);
    colnames(low) <- c("low.ERU", "low.ERM", "low.ERL", "low.ELU", "low.ELM", "low.ELL");
    high <- pmax(high.L1, high.M1);
    colnames( high ) <- c("high.ERU", "high.ERM", "high.ERL", "high.ELU", "high.ELM", "high.ELL");
           
    write.csv(list(IdDate=paste(scores$StId, gsub("-", "", scores$Dato.1), sep='_'),
                   low,
                   high),
              file=outpath, 
              quote=FALSE,
              row.names=FALSE);
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
