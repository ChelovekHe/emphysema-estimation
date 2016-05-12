#!/usr/bin/Rscript

main <- function(options) {
    usage <- "Usage: <visual-scores> <region> <icc-type> <predictions> [<predictions]*"
    if ( length(options) < 5 ) {
        cat( usage, '\n' );
        quit( status = 1 );
    }
       
    CalculateICC( options[1], options[2],  options[3], options[4:length(options)] )
    quit( status = 0 );
}


CalculateICC <- function(visualScoresPath, region, iccType, predictionPaths) {
    library(irr)
    scores <- read.csv(visualScoresPath, header=TRUE);


    ## Convert classes and predictions to midpoints of intervals
    L1 <- scores[, paste(region, 'L1', sep='.')]
    L1[L1 <= 1] <- 0;
    L1[L1 == 2] <- (0.01 + 0.05)/2;
    L1[L1 == 3] <- (0.06 + 0.25)/2;
    L1[L1 == 4] <- (0.26 + 0.50)/2;
    L1[L1 == 5] <- (0.51 + 0.75)/2;
    L1[L1 == 6] <- (0.76 + 1.00)/2;
    
    M1 <- scores[, paste(region, 'M1', sep='.')]
    M1[M1 <= 1] <- 0;
    M1[M1 == 2] <- (0.01 + 0.05)/2;
    M1[M1 == 3] <- (0.06 + 0.25)/2;
    M1[M1 == 4] <- (0.26 + 0.50)/2;
    M1[M1 == 5] <- (0.51 + 0.75)/2;
    M1[M1 == 6] <- (0.76 + 1.00)/2;

    perf = c()
    n = 1
    for ( path in predictionPaths ) {
        pred <- round(read.csv(path), 2);
        pred[0.01 <= pred & pred <= 0.05] <- (0.01 + 0.05)/2;
        pred[0.06 <= pred & pred <= 0.25] <- (0.06 + 0.25)/2;
        pred[0.26 <= pred & pred <= 0.50] <- (0.26 + 0.50)/2;
        pred[0.51 <= pred & pred <= 0.75] <- (0.51 + 0.75)/2;
        pred[0.76 <= pred & pred <= 1.00] <- (0.76 + 1.00)/2;
        
        ## Calculate intraclass correlation coefficient    
        ref.avg <- (L1 + M1)/2;
        y <- cbind(ref.avg, pred);
        LM.CMS.icc <- icc(y, model="twoway", type=iccType)
        ##cat( n, LM.CMS.icc$value, '\n' )
        perf = c(perf, LM.CMS.icc$value)
        n = n + 1
    }

    measures = c(mean(perf), sd(perf), min(perf), max(perf))
    cat( 'mean  sd  min  max\n' )
    cat( round(measures, 2), '\n' )
}



options <- commandArgs(trailingOnly=TRUE);
main(options);
