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
        
    CalculateKappa( options[1], options[2], paths, ids )
    quit( status = 0 );
}

CalculateKappa <- function(visualScoresPath, iccType, predictionPaths, ids) {
    library(irr)
    visualScores <- read.csv(visualScoresPath, header=TRUE);

    regions <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL");
    ##regions <- c("ERU", "ELU");
    regions.L1 <- sapply(regions, function(region) paste(region, 'L1', sep="."));
    regions.M1 <- sapply(regions, function(region) paste(region, 'M1', sep="."));

    cat('k,Region,Raters,kappa,p.value\n')
    for ( i in 1:length(ids) ) {
        predictions <- read.csv(predictionPaths[i], header=TRUE);
        scores <- merge(visualScores, predictions, by='IdDate');

        ## Convert predictions to classes and set 0 -> 1
        ## L1 <- scores[, regions.L1]
        ## L1[L1 == 0] <- 1;
        ## colnames(L1) <- regions;
    
        ## M1 <- scores[, regions.M1]
        ## M1[M1 == 0] <- 1;
        ## colnames(M1) <- regions;

        ## pred <- round(scores[, regions], 2);
        ## pred[0.76 <= pred & pred <= 1.00] <- 6
        ## pred[0.51 <= pred & pred <= 0.75] <- 5
        ## pred[0.26 <= pred & pred <= 0.50] <- 4
        ## pred[0.06 <= pred & pred <= 0.25] <- 3
        ## pred[0.01 <= pred & pred <= 0.05] <- 2
        ## pred[pred < 0.01]                 <- 1

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
        
        ## Calculate kappa
        for ( region in regions ) {
            ref <- cbind(L1[,region], M1[,region])
            ref.kappa <- kappa2(ref, "squared")
            cat( paste(ids[i],
                       region,
                       "L1.M1",
                       round(ref.kappa$value,2),
                       round(ref.kappa$p.value,2), sep=','), '\n' )

        
            L1.CMS <- cbind(ref[,1], pred[,region]);
            L1.CMS.kappa <- kappa2(L1.CMS, "squared")
            cat( paste(ids[i],
                       region,
                       "L1.CMS",
                       round(L1.CMS.kappa$value,2),
                       round(L1.CMS.kappa$p.value,2), sep=','), '\n' )

            M1.CMS <- cbind(ref[,2], pred[,region]);
            M1.CMS.kappa <- kappa2(M1.CMS, "squared")
            cat( paste(ids[i],
                       region,
                       "M1.CMS",
                       round(M1.CMS.kappa$value,2),
                       round(M1.CMS.kappa$p.value,2), sep=','), '\n' )
        
            ref.avg <- (ref[,1] + ref[,2])/2;
            LM.CMS <- cbind(ref.avg, pred[,region]);
            LM.CMS.kappa <- kappa2(LM.CMS, "squared")
            cat( paste(ids[i],
                       region,
                       "LM.CMS",
                       round(LM.CMS.kappa$value,2),
                       round(LM.CMS.kappa$p.value,2), sep=','), '\n' )
        }
    }
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
