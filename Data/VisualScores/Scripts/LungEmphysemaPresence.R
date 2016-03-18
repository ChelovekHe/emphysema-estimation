#!/usr/bin/Rscript

## 

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <scores-path> <dataset-path> <out-path>" );
        quit( status = 1 );
    }
    StIds <- GetStIds( options[2] );
    lungEmphysemaPresence <- LungEmphysemaPresence( options[1], StIds );
    write.csv(lungEmphysemaPresence,
              file=options[3],
              quote=FALSE,
              row.names=FALSE);
    quit( status = 0 );
}

GetStIds <- function( path ) {
    StIdsAndDates <- read.csv( path, header=FALSE );
    StIdsAndDates[,1];
}

LungEmphysemaPresence <- function(path, StIds=NULL) {
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
    ##   Emf : Emphysema presence
    ## Emf is given as
    ##   1 => No emphysema
    ##   2 => Emphysema
    ## We add the scores, so we get
    ##   2 => No emhysema
    ##   3 => Emphysema acording one rater
    ##   4 => EMphysema according to both raters
    ## We want 0 => no emphysema and 1 => emphysema
    emphysemaPresence <- as.integer((scores[,"Emf.L1"] + scores[, "Emf.M1"]) > 2 ) ;
    
    list(StudyId=scores$StId,
         Date=scores$Dato.1,
         EmphysemaPresence=emphysemaPresence);
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
