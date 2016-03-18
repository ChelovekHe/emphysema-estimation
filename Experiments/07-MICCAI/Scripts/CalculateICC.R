#!/usr/bin/Rscript

main <- function(options) {
    if ( length(options) < 2 ) {
        print( "Usage: <visual-scores> <predictions> [<icc-type>(c|a)]" );
        quit( status = 1 );
    }

    icc.type <- 'c'
    if (length(options) == 3 ) {
        icc.type <- options[3]
    }
       
    CalculateICC( options[1], options[2], icc.type );
    quit( status = 0 );
}

CalculateICC <- function(visualScoresPath, predictionsPath, iccType) {
    library(irr)
    visualScores <- read.csv(visualScoresPath, header=TRUE);
    predictions <- read.csv(predictionsPath, header=TRUE);

    scores <- merge(visualScores, predictions, by='IdDate');

    ## We can do a couple of things
    ## 1. Calculate the mean squared error used for the optimization. Since we
    ##    are dealing with intervals, we can calculate it in a couple of
    ##    different ways.
    ##    a) Use the mean of reference midpoints as true proportion
    ##
    ##    b) Set the error to zero for all predictions that lie in one of the
    ##       intervals, and use smallest distance from any endpoint for
    ##       predictions that dont lie in the interval.
    ##
    ##    c) Calculate the mean of the error over both references
    ##
    ## We try to combine b) and c) and see what we get

    ## Rater L1
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

    ## Rater M1
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

    ## regions <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL");
    regions <- c("ERU", "ELU");
    pred <- round(scores[, regions], 2);
    pred[0.01 <= pred & pred <= 0.05] <- (0.01 + 0.05)/2;
    pred[0.06 <= pred & pred <= 0.25] <- (0.06 + 0.25)/2;
    pred[0.26 <= pred & pred <= 0.50] <- (0.26 + 0.50)/2;
    pred[0.51 <= pred & pred <= 0.75] <- (0.51 + 0.75)/2;
    pred[0.76 <= pred & pred <= 1.00] <- (0.76 + 1.00)/2;

    ## 2. We can calculate intraclass correlation coefficient
    L1 <- (low.L1 + high.L1)/2;
    M1 <- (low.M1 + high.M1)/2;
    cat('Region,Raters,ICC,CI.low,CI.high\n')
    for ( region in regions ) {
        ##         cat(paste('--------------------', region, '--------------------\n'))
        ## ref <- cbind(L1[,paste(region, "L1", sep='.')],
        ##              M1[,paste(region, "M1", sep='.')])
        ## ref.icc <- icc(ref, model="twoway", type=iccType)
        ## cat( paste("L1/M1\t", round(ref.icc$value,2),
        ##            ' (', round(ref.icc$lbound,2),
        ##            ',', round(ref.icc$ubound,2), ')\n', sep='') )


        ## y <- cbind(ref[,1], pred[,region]);
        ## l1cms.icc <- icc(y, model="twoway", type=iccType)
        ## cat( paste("L1/CMS\t", round(l1cms.icc$value,2),
        ##            ' (', round(l1cms.icc$lbound,2),
        ##            ',', round(l1cms.icc$ubound,2), ')\n', sep='') )

        ## y <- cbind(ref[,2], pred[,region]);
        ## m1cms.icc <- icc(y, model="twoway", type=iccType)
        ## cat( paste("M1/CMS\t",round(m1cms.icc$value,2),
        ##            ' (', round(m1cms.icc$lbound,2),
        ##            ',', round(m1cms.icc$ubound,2), ')\n', sep='') )
        ref <- cbind(L1[,paste(region, "L1", sep='.')],
                     M1[,paste(region, "M1", sep='.')])
        ref.icc <- icc(ref, model="twoway", type=iccType)
        cat( paste(region, "L1.M1", round(ref.icc$value,2),
                   round(ref.icc$lbound,2),
                   round(ref.icc$ubound,2), sep=','), '\n' )


        y <- cbind(ref[,1], pred[,region]);
        l1cms.icc <- icc(y, model="twoway", type=iccType)
        cat( paste(region, "L1.CMS", round(l1cms.icc$value,2),
                   round(l1cms.icc$lbound,2),
                   round(l1cms.icc$ubound,2), sep=','), '\n' )

        y <- cbind(ref[,2], pred[,region]);
        m1cms.icc <- icc(y, model="twoway", type=iccType)
        cat( paste(region,
                   "M1.CMS",
                   round(m1cms.icc$value,2),
                   round(m1cms.icc$lbound,2),
                   round(m1cms.icc$ubound,2), sep=','), '\n' )

        ref.avg <- (ref[,1] + ref[,2])/2;
        y <- cbind(ref.avg, pred[,region]);
        LMcms.icc <- icc(y, model="twoway", type=iccType)
        cat( paste(region,
                   "LM.CMS",
                   round(LMcms.icc$value,2),
                   round(LMcms.icc$lbound,2),
                   round(LMcms.icc$ubound,2), sep=','), '\n' )
    }
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
