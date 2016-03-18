#!/usr/bin/Rscript

main <- function(options) {
    usage <- "Usage: <visual-scores> <weight> <predictions> <id> <region>"
    if ( length(options) < 5 ) {
        cat( usage, '\n' );
        quit( status = 1 );
    }
       
    CalculateKappa( options[1], options[2], options[3], options[4], options[5] )
    quit( status = 0 );
}


CalculateKappa <- function(visualScoresPath, weight, predictionPath, id, region) {
    library(irr)
    visualScores <- read.csv(visualScoresPath, header=TRUE);

    cat('k,Region,Raters,kappa,p.value\n')
    predictions <- read.csv(predictionPath, header=TRUE);
    scores <- merge(visualScores, predictions, by='IdDate');

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

    pred <- round(scores[, region], 2);
    pred[0.01 <= pred & pred <= 0.05] <- (0.01 + 0.05)/2;
    pred[0.06 <= pred & pred <= 0.25] <- (0.06 + 0.25)/2;
    pred[0.26 <= pred & pred <= 0.50] <- (0.26 + 0.50)/2;
    pred[0.51 <= pred & pred <= 0.75] <- (0.51 + 0.75)/2;
    pred[0.76 <= pred & pred <= 1.00] <- (0.76 + 1.00)/2;
    
    ## Calculate kapap coefficient
    ref <- cbind(L1, M1)
    ref.kappa <- kappa2(ref, weight=weight)
    cat( paste(id,
               region,
               "L1.M1",
               round(ref.kappa$value,2),
               round(ref.kappa$p.value,10), sep=','), '\n' )
        
    y <- cbind(L1, pred);
    L1.CMS.kappa <- kappa2(y, weight=weight)
    cat( paste(id,
               region,
               "L1.CMS",
               round(L1.CMS.kappa$value,2),
               round(L1.CMS.kappa$p.value,10), sep=','), '\n' )

    y <- cbind(M1, pred);
    M1.CMS.kappa <- kappa2(y, weight=weight)
    cat( paste(id,
               region,
               "M1.CMS",
               round(M1.CMS.kappa$value,2),
               round(M1.CMS.kappa$p.value,10), sep=','), '\n' )
    
    ref.avg <- (L1 + M1)/2;
    y <- cbind(ref.avg, pred);
    LM.CMS.kappa <- kappa2(y, weight=weight)
    cat( paste(id,
               region,
               "LM.CMS",
               round(LM.CMS.kappa$value,2),
               round(LM.CMS.kappa$p.value,10), sep=','), '\n' )
}



options <- commandArgs(trailingOnly=TRUE);
main(options);
