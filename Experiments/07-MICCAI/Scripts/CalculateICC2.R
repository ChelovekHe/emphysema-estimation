#!/usr/bin/Rscript

main <- function(options) {
    usage <- "Usage: <visual-scores> <icc-type> <predictions> <id> [<predictions> <id>]"
    if ( length(options) < 4 ) {
        cat( usage, '\n' );
        quit( status = 1 );
    }

    paths <- options[seq(3,length(options), 2)]
    ids <- options[seq(4,length(options), 2)]

    if ( length(paths) != length(ids) ) {
        cat( "Missing id\n", usage, '\n' )
        quit( status = 2 );
    }
        
    CalculateICC( options[1], options[2], paths, ids )
    quit( status = 0 );
}

CalculateICC <- function(visualScoresPath, iccType, predictionPaths, ids) {
    library(irr)
    visualScores <- read.csv(visualScoresPath, header=TRUE);

    regions <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL");
    ##regions <- c("ERU", "ELU");
    regions.L1 <- sapply(regions, function(region) paste(region, 'L1', sep="."));
    regions.M1 <- sapply(regions, function(region) paste(region, 'M1', sep="."));

    cat('k,Region,Raters,ICC,CI.low,CI.high\n')
    for ( i in 1:length(ids) ) {
        predictions <- read.csv(predictionPaths[i], header=TRUE);
        scores <- merge(visualScores, predictions, by='IdDate');

        ## Convert classes and predictions to midpoints of intervals
        L1 <- scores[, regions.L1]
        L1[L1 <= 1] <- 0;
        L1[L1 == 2] <- (0.01 + 0.05)/2;
        L1[L1 == 3] <- (0.06 + 0.25)/2;
        L1[L1 == 4] <- (0.26 + 0.50)/2;
        L1[L1 == 5] <- (0.51 + 0.75)/2;
        L1[L1 == 6] <- (0.76 + 1.00)/2;
        colnames(L1) <- regions;
    
        M1 <- scores[, regions.M1]
        M1[M1 <= 1] <- 0;
        M1[M1 == 2] <- (0.01 + 0.05)/2;
        M1[M1 == 3] <- (0.06 + 0.25)/2;
        M1[M1 == 4] <- (0.26 + 0.50)/2;
        M1[M1 == 5] <- (0.51 + 0.75)/2;
        M1[M1 == 6] <- (0.76 + 1.00)/2;
        colnames(M1) <- regions;

        pred <- round(scores[, regions], 2);
        pred[0.01 <= pred & pred <= 0.05] <- (0.01 + 0.05)/2;
        pred[0.06 <= pred & pred <= 0.25] <- (0.06 + 0.25)/2;
        pred[0.26 <= pred & pred <= 0.50] <- (0.26 + 0.50)/2;
        pred[0.51 <= pred & pred <= 0.75] <- (0.51 + 0.75)/2;
        pred[0.76 <= pred & pred <= 1.00] <- (0.76 + 1.00)/2;

        ## Calculate intraclass correlation coefficient
        for ( region in regions ) {
            ref <- cbind(L1[,region], M1[,region])
            ref.icc <- icc(ref, model="twoway", type=iccType)
            cat( paste(ids[i],
                       region,
                       "L1.M1",
                       round(ref.icc$value,2),
                       round(ref.icc$lbound,2),
                       round(ref.icc$ubound,2), sep=','), '\n' )

        
            y <- cbind(ref[,1], pred[,region]);
            l1cms.icc <- icc(y, model="twoway", type=iccType)
            cat( paste(ids[i],
                       region,
                       "L1.CMS",
                       round(l1cms.icc$value,2),
                       round(l1cms.icc$lbound,2),
                       round(l1cms.icc$ubound,2), sep=','), '\n' )

            y <- cbind(ref[,2], pred[,region]);
            m1cms.icc <- icc(y, model="twoway", type=iccType)
            cat( paste(ids[i],
                       region,
                       "M1.CMS",
                       round(m1cms.icc$value,2),
                       round(m1cms.icc$lbound,2),
                       round(m1cms.icc$ubound,2), sep=','), '\n' )
        
            ref.avg <- (ref[,1] + ref[,2])/2;
            y <- cbind(ref.avg, pred[,region]);
            LMcms.icc <- icc(y, model="twoway", type=iccType)
            cat( paste(ids[i],
                       region,
                       "LM.CMS",
                       round(LMcms.icc$value,2),
                       round(LMcms.icc$lbound,2),
                       round(LMcms.icc$ubound,2), sep=','), '\n' )
        }
    }
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
